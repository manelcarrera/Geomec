
#include <stdexcept>

#include "BaseEntryTypes.h"
#include "ElementGroup.h"
#include "FaultParameters.h"
#include "HorizonBase.h"
#include "InterfaceElement.h"
#include "ModelBase.h"
#include "ObjectFault.h"
#include "ValueTypes.h"

namespace cora {

CObjectFault::CObjectFault(CModelBase *modelBase, CHorizonBase *horizonBase)
    : CObjectBase(), m_horizonBase(horizonBase), m_parameters(getParameters(modelBase, horizonBase)), m_owner(false),
      m_elementSet(createElementSet(m_owner, modelBase, horizonBase)), m_sequenceState(0) {}

CObjectFault::~CObjectFault() {
  if (m_owner && (m_elementSet != 0)) {
    delete m_elementSet;
  }
}

const QString &CObjectFault::name() const { return m_horizonBase->Name(); }

const TParameters &CObjectFault::getParameters() const { return m_parameters; }

namespace {

const QString PREFIX = "fault:";

} // anonymous namespace

const QString &CObjectFault::prefix() const { return PREFIX; }

const COpenGLNode *CObjectFault::object() const { return m_horizonBase; }

namespace {

const QString TYPE_NAME = "Fault";

} // anonymous namespace

const QString &CObjectFault::typeName() const { return TYPE_NAME; }

const geo::IElement *CObjectFault::getFirstElement() {
  m_sequenceState = 0;

  return getNextElement();
}

const geo::IElement *CObjectFault::getNextElement() {
  if (m_sequenceState < m_horizonBase->InterfaceElementGroup()->ElementSize()) {
    return &(m_horizonBase->InterfaceElementGroup()->Element(m_sequenceState++));
  }

  return 0;
}

std::ostream &CObjectFault::operator()(std::ostream &os) const {
  os << PREFIX.toStdString() << m_horizonBase->Name().toStdString() << std::endl;
  os << m_parameters.size() << std::endl;

  for (size_t s = 0; s < m_parameters.size(); ++s) {
    os << m_parameters[s];
  }

  return os;
}

CElementSet *CObjectFault::getElementSet() const { return m_elementSet; }

// private

// static

TParameters CObjectFault::getParameters(CModelBase *modelBase, CHorizonBase *horizonBase) {
  CFaultParameters faultParameters;

  return faultParameters.getParameters(modelBase, horizonBase);
}

namespace {

const QString NUMBER_OF_NODES = "Only 3 or 4 nodes can be handled, detected %1 nodes";

} // anonymous namespace

CElementSet *CObjectFault::createElementSet(bool &owner, CModelBase *modelBase, CHorizonBase *horizonBase) {
  CElementSet *elementSet = elementSetExists(modelBase, horizonBase);

  if (elementSet != 0) {
    return elementSet;
  }

  const geo::CElementGroup *elementGroup = horizonBase->InterfaceElementGroup();
  std::vector<const geo::IElement *> elements(elementGroup->ElementSize());

  for (int element = 0; element < elementGroup->ElementSize(); ++element) {
    elements[element] = &((static_cast<const geo::IInterfaceElement &>(elementGroup->Element(element))).Front());
  }

  assert(!elements.empty());

  if (elements[0]->NrOfNodes() == 4) {
    owner = true;
    elementSet = new CElementSet(elements, horizonBase->Name(), CElementSet::QUAD_3D, *modelBase);
  } else if (elements[0]->NrOfNodes() == 3) {
    owner = true;
    elementSet = new CElementSet(elements, horizonBase->Name(), CElementSet::TRIANGLE_3D, *modelBase);
  } else {
    // We do not have always a summary result file to our disposal, hence
    // throwing an exception.

    QString message = QString(NUMBER_OF_NODES).arg(elements[0]->NrOfNodes());

    throw std::runtime_error(message.toStdString());
  }

  return elementSet;
}

CElementSet *CObjectFault::elementSetExists(CModelBase *modelBase, CHorizonBase *horizonBase) {
  TPointSetEntry *pointSetEntry = dynamic_cast<TPointSetEntry *>(modelBase->GraphEntry(MD_BASE_POINTSET));
  TPointSetEntry::TSortedNodeSet entryNodes = pointSetEntry->SortedEntryNodes();

  for (TPointSetEntry::TSortedNodeSet::iterator node = entryNodes.begin(); node != entryNodes.end(); ++node) {
    if ((*node)->TypeId() == IDT_ELEMENTSET) {
      CElementSet *elementSet = static_cast<CElementSet *>(*node);

      if ((elementSet->Name() == horizonBase->Name()) && ((elementSet->ElementType() == CElementSet::QUAD_3D) ||
                                                          (elementSet->ElementType() == CElementSet::TRIANGLE_3D))) {
        return elementSet;
      }
    }
  }

  return 0;
}

} // namespace cora
