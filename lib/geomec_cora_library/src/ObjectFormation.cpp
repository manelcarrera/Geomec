#include <stdexcept>

#include "FilterValueTypes.h"
#include "FormationBase.h"
#include "GetFormationParameterInfo.h"
#include "IElementSet.h"
#include "ObjectFormation.h"
#include "Pressure.h"
#include "Utilities4ValueVector.h"
#include "getPressureParameters.h"

namespace cora {

CObjectFormation::CObjectFormation(CFormationBase *formationBase, CModelBase *modelBase)
    : CObjectBase(), m_formationBase(formationBase), m_parameters(getParameters(m_formationBase, modelBase)),
      m_owner(false), m_elementSet(createElementSet(m_owner, modelBase, formationBase)),
      m_sequenceState(std::make_pair<int, int>(0, 0)) {}

CObjectFormation::~CObjectFormation() {
  if (m_owner && (m_elementSet != 0)) {
    delete m_elementSet;
  }
}

const QString &CObjectFormation::name() const { return m_formationBase->Name(); }

const TParameters &CObjectFormation::getParameters() const { return m_parameters; }

namespace {

const QString PREFIX = "formation:";

} // anonymous namespace

const QString &CObjectFormation::prefix() const { return PREFIX; }

const COpenGLNode *CObjectFormation::object() const { return m_formationBase; }

namespace {

const QString TYPE_NAME = "Formation";

} // anonymous namespace

const QString &CObjectFormation::typeName() const { return TYPE_NAME; }

const geo::IElement *CObjectFormation::getFirstElement() {
  m_sequenceState = std::make_pair<int, int>(0, 0);

  return getNextElement();
}

const geo::IElement *CObjectFormation::getNextElement() {
  for (; m_sequenceState.first < m_formationBase->ElementSetSize(); ++(m_sequenceState.first)) {
    const IFormationElementSet &formationElementSet = m_formationBase->ElementSet(m_sequenceState.first);

    if (m_sequenceState.second < formationElementSet.ElementSet().ElementSize()) {
      return &(formationElementSet.ElementSet().Element((m_sequenceState.second)++));
    }

    m_sequenceState.second = 0;
  }

  return 0;
}

std::ostream &CObjectFormation::operator()(std::ostream &os) const {
  os << PREFIX.toStdString() << m_formationBase->Name().toStdString() << std::endl;
  os << m_parameters.size() << std::endl;

  for (size_t s = 0; s < m_parameters.size(); ++s) {
    os << m_parameters[s];
  }

  return os;
}

CElementSet *CObjectFormation::getElementSet() const { return m_elementSet; }

// private

namespace {

void getMaterialParameters(TParameters &parameters, CFormationBase *formationBase, CModelBase * /*modelBase*/,
                           const CDepletionStage &depletionStage) {
  const CValueTypeFactory *valueTypeFactory = CValueTypeFactory::instance();

  CMaterialServer &materialServer = formationBase->Material(depletionStage);
  IMaterialRock *materialRock = materialServer.LibraryMaterial();
  std::set<CValueType *> valueTypes = materialServer.Links<CValueType>();

  if (materialRock != 0) {
    CLibraryMaterial &libraryMaterial = materialRock->LibraryMaterial();
    CGetFormationParameterInfo getFormationParameterInfo(formationBase);
    CFilterValueTypes filterValueTypes(libraryMaterial.MaterialModel());

    for (size_t i = 0; i < libraryMaterial.ParameterSize(); ++i) {
      if (filterValueTypes.isValueTypeAllowed(libraryMaterial.Parameter(i).ValueTypeID())) {
        std::pair<geo::CValue, geo::CValue> range =
            getFormationParameterInfo.getRange(libraryMaterial.Parameter(i), valueTypes);
        double mean = getFormationParameterInfo.getMean(libraryMaterial.Parameter(i), valueTypes);
        QString formationParameterName =
            QString("%1_D%2")
                .arg(valueTypeFactory->getImportTag(libraryMaterial.Parameter(i).ValueTypeID()))
                .arg(depletionStage.Index());

        parameters.push_back(TParameter(new CParameter(CParameter::formationParameter4Material, formationParameterName,
                                                       range.first, range.second, mean, formationBase,
                                                       depletionStage.Index(), libraryMaterial.Parameter(i))));
      }
    }
  }
}

const QString PRESSURE_TYPE = "a pressure";
const QString PRESSURE_CHANGE_TYPE = "a pressure change";

const int FIRST_ELEMENT_SET = 0;

} // anonymous namespace

// static

TParameters CObjectFormation::getParameters(CFormationBase *formationBase, CModelBase *modelBase) {
  TParameters parameters;
  CDepletionStageEntry *depletionStages =
      dynamic_cast<CDepletionStageEntry *>(modelBase->GraphEntry(MD_BASE_DEPLETION_STAGE));

  for (CDepletionStageEntry::iterator depletionStage = depletionStages->begin();
       depletionStage != depletionStages->LastStage(); ++depletionStage) {
    if ((*depletionStage).IsPhaseStartStage()) {
      getMaterialParameters(parameters, formationBase, modelBase, *depletionStage);
      getPressureParameters<CFormationBase, geo::IElementSet, CParameter::TFormationParameter4Pressure, CPressure>(
          parameters, modelBase, *depletionStage, formationBase,
          formationBase->ElementSet(FIRST_ELEMENT_SET).ElementSet(), CParameter::formationParameter4Pressure,
          IDT_VALUETYPE_PRESSURE, IDS_VALUENAME_PRESSURE, PRESSURE_TYPE);

      if (!(*depletionStage).IsMarkedAsInitial()) {
        getPressureParameters<CFormationBase, geo::IElementSet, CParameter::TFormationParameter4PressureChange,
                              CPressure>(parameters, modelBase, *depletionStage, formationBase,
                                         formationBase->ElementSet(FIRST_ELEMENT_SET).ElementSet(),
                                         CParameter::formationParameter4PressureChange, IDT_VALUETYPE_PRESSURE,
                                         IDS_VALUENAME_PRESSURE, PRESSURE_CHANGE_TYPE);
      }
    }
  }

  return parameters;
}

namespace {

const QString NUMBER_OF_NODES = "Only 4 or 8 nodes can be handled, detected %1 nodes";

const int HEXAHEDRON_NODES_COUNT = 8;
const int TETRAHEDRON_NODES_COUNT = 4;

const size_t FIRST_ELEMENT = 0;

} // anonymous namespace

CElementSet *CObjectFormation::createElementSet(bool &owner, CModelBase *modelBase, CFormationBase *formationBase) {
  CElementSet *elementSet = elementSetExists(modelBase, formationBase);

  if (elementSet != 0) {
    return elementSet;
  }

  std::vector<const geo::IElement *> elements;

  for (int s = 0; s < formationBase->ElementSetSize(); ++s) {
    const geo::IElementSet &iElementSet = formationBase->ElementSet(s).ElementSet();

    for (int element = 0; element < iElementSet.ElementSize(); ++element) {
      elements.push_back(&((static_cast<const geo::IElement &>(iElementSet.Element(element)))));
    }
  }

  assert(!elements.empty());

  if (elements[FIRST_ELEMENT]->NrOfNodes() == HEXAHEDRON_NODES_COUNT) {
    owner = true;
    elementSet = new CElementSet(elements, formationBase->Name(), CElementSet::HEXA, *modelBase);
  } else if (elements[FIRST_ELEMENT]->NrOfNodes() == TETRAHEDRON_NODES_COUNT) {
    owner = true;
    elementSet = new CElementSet(elements, formationBase->Name(), CElementSet::TETRA, *modelBase);
  } else {
    // We do not have always a summary result file to our disposal, hence
    // throwing an exception.

    QString message = QString(NUMBER_OF_NODES).arg(elements[FIRST_ELEMENT]->NrOfNodes());

    throw std::runtime_error(message.toStdString());
  }

  return elementSet;
}

CElementSet *CObjectFormation::elementSetExists(CModelBase *modelBase, CFormationBase *formationBase) {
  TPointSetEntry *pointSetEntry = dynamic_cast<TPointSetEntry *>(modelBase->GraphEntry(MD_BASE_POINTSET));
  TPointSetEntry::TSortedNodeSet entryNodes = pointSetEntry->SortedEntryNodes();

  for (TPointSetEntry::TSortedNodeSet::iterator node = entryNodes.begin(); node != entryNodes.end(); ++node) {
    if ((*node)->TypeId() == IDT_ELEMENTSET) {
      CElementSet *elementSet = static_cast<CElementSet *>(*node);

      if ((elementSet->Name() == formationBase->Name()) &&
          ((elementSet->ElementType() == CElementSet::HEXA) || (elementSet->ElementType() == CElementSet::TETRA))) {
        return elementSet;
      }
    }
  }

  return 0;
}

} // namespace cora
