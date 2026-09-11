
#include "LoadPropertyBase.h"
#include "ElementValueSet.h"
#include "FaultPressure.h"
#include "FormationBase.h"
#include "FractureMatrixPressure.h"
#include "HorizonBase.h"
#include "IInterfaceElement.h"
#include "IParallelInitializationCallback.h"
#include "MaterialFractureApertureBase.h"
#include "ModelBase.h"
#include "Pressure.h"
#include "RGInterface.h"
#include "RockMechProcessor.h"
#include "Temperature.h"
#include "propertyValues.h"

namespace GeomecRGI {

CLoadPropertyBase::CLoadPropertyBase(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                                     CRockMechProcessor &rmp)
    : m_RGProperty(rgProperty), m_RGI(rgi), m_ModelBase(modelBase), m_RMP(rmp),
      m_ValueTypeName(QString("%1 %2")
                          .arg(m_RMP.PropertyName(m_RGProperty))
                          .arg(m_RGI.getCurrentDepletionStage().getDepletionStage())) {
  setAnisotropic(m_IsAnisotropic, m_AnisotropicIsSet, m_RGI, m_ModelBase, m_RMP);
}

CLoadPropertyBase::~CLoadPropertyBase() {}

bool CLoadPropertyBase::loadProperty(unsigned int uValueType) {
  QString strPropertyName = m_RMP.PropertyName(m_RGProperty);
  std::vector<geo::CValue> vcValues;

  if (!loadValues(vcValues, strPropertyName)) {
    return false;
  }

  return loadProperty(vcValues, strPropertyName, uValueType);
}

bool CLoadPropertyBase::loadProperty(const std::vector<geo::CValue> &vcValues, const QString &strPropertyName,
                                     unsigned int uValueType) {
  std::vector<std::vector<geo::CValue>> values;

  for (std::vector<double>::size_type s = 0; s < vcValues.size(); ++s) {
    const geo::IElement &element = m_ModelBase.Mesh().Mesh().Element(s);
    int numberOfNodes = element.NrOfNodes();
    std::vector<geo::CValue> nodalValues(numberOfNodes, vcValues[s]);

    for (int n = 0; n < numberOfNodes; ++n) {
      convertValue(nodalValues[n]);
    }

    values.push_back(nodalValues);
  }

  return loadProperty(values, strPropertyName, uValueType);
}

bool CLoadPropertyBase::loadProperty(const std::vector<std::vector<geo::CValue>> &vcValues,
                                     const QString &strPropertyName, unsigned int uValueType) {
  if (valueSetIsPresent(m_ValueTypeName)) {
    return false;
  }

  return privateLoadProperty(vcValues, strPropertyName, uValueType);
}

// protected

void CLoadPropertyBase::linkValueTypes(CFormationBase &formation, CValueType *pVT, const CDepletionStage &stage,
                                       bool /*fluidPressureFracDetected*/) const {
  if ((formation.ConnectedMaterial(stage) != 0) && formation.ConnectedMaterial(stage)->CanConnectItem(*pVT) &&
      !formation.ConnectedMaterial(stage)->IsLinkedTo(*pVT)) {
    formation.ConnectedMaterial(stage)->LinkTo(*pVT);
  }
}

void CLoadPropertyBase::convertValue(geo::CValue & /*dValue*/) const {}

bool CLoadPropertyBase::addDelta(std::vector<geo::CValue> & /*vcNodalValues*/, const QString & /*strPropertyName*/,
                                 const CFormationBase * /*pFormation*/, const geo::IElement & /*elm*/,
                                 int /*nNod*/) const {
  return true;
}

IValueDomainScalar::TValueVec CLoadPropertyBase::addDeltaSpecific(const CDepletionStage & /*prevstage*/,
                                                                  const CFormationBase * /*pFormation*/,
                                                                  const geo::IElement & /*elm*/, int nNod) const {
  assert(false);

  IValueDomainScalar::TValueVec valueVec(nNod, 0);

  return valueVec;
}

bool CLoadPropertyBase::isFractureApertureModel(const CFormationBase &formationBase,
                                                const CDepletionStage &depletionStage) const {
  bool isFractureApertureModel = true;

  try {
    const IMaterialRock &materialRock = *formationBase.Material(depletionStage).LibraryMaterial();
    /*const CMaterialFractureApertureBase& materialFractureApertureBase =*/
    dynamic_cast<const CMaterialFractureApertureBase &>(materialRock);
  }

  catch (const std::bad_cast &) {
    isFractureApertureModel = false;
  }

  return isFractureApertureModel;
}

bool CLoadPropertyBase::isFractureApertureModel(const CFormationBase &formationBase,
                                                const CDepletionStage &depletionStage,
                                                const geo::IElement &element) const {
  bool isFractureApertureModel = true;

  try {
    const CFFMaterial &cffMaterial = formationBase.Material(depletionStage).Material(element);
    /*const CMaterialFractureApertureBase& materialFractureApertureBase =*/
    dynamic_cast<const CMaterialFractureApertureBase &>(cffMaterial.Material());
  }

  catch (const std::bad_cast &) {
    isFractureApertureModel = false;
  }

  return isFractureApertureModel;
}

bool CLoadPropertyBase::addDeltaBase(std::vector<geo::CValue> &vcNodalValues, const QString &strPropertyName,
                                     const CFormationBase *pFormation, const geo::IElement &elm, int nNod) const {
  if (m_RGI.getCurrentDepletionStage().getDepletionStage() == 0) {
    QString l = QString("Property '%1': delta property assigned in initial "
                        "depletion stage")
                    .arg(strPropertyName);

    m_RMP.AddLogLine(l, &m_RGI, false, true);

    return false;
  }

  CDepletionStage &prevstage =
      m_ModelBase.DepletionStageEntry().StageByIndex(m_RGI.getCurrentDepletionStage().getDepletionStage() - 1);
  IValueDomainScalar::TValueVec vcPrevValues = addDeltaSpecific(prevstage, pFormation, elm, nNod);

  assert(vcNodalValues.size() == vcPrevValues.size());

  for (int j = 0; j < nNod; ++j) {
    if (vcNodalValues[j].Valid())
      vcNodalValues[j] += vcPrevValues[j].Value();
  }

  return true;
}

bool CLoadPropertyBase::isAnisotropic() const { return m_IsAnisotropic; }

bool CLoadPropertyBase::enforceUniqueness(unsigned int uValueType) const { return false; }

// private

bool CLoadPropertyBase::m_IsAnisotropic = false;
bool CLoadPropertyBase::m_AnisotropicIsSet = false;

bool CLoadPropertyBase::privateLoadProperty(const std::vector<std::vector<geo::CValue>> &vcValues,
                                            const QString &strPropertyName, unsigned int uValueType) const {
  int nSet = 0;
  static int volumetricStrainSet = 0;
  bool firstVolumetricStrain = true;
  bool fluidPressureFracDetected = false;
  CValueType *pVT = 0;

  if ((uValueType == IDT_VALUETYPE_VOLUMETRICSTRAIN) && (volumetricStrainSet != 0)) {
    nSet = volumetricStrainSet;
    firstVolumetricStrain = false;
  } else if ((uValueType == IDT_VALUETYPE_VOLUMETRICSTRAIN) && (volumetricStrainSet == 0)) {
    nSet = m_ModelBase.Mesh().AddElementValueSet();
    volumetricStrainSet = nSet;
  } else {
    nSet = m_ModelBase.Mesh().AddElementValueSet();
  }

  CElementValueSet &elementValueSet = m_ModelBase.Mesh().ElementValueSet(nSet);
  CDepletionStage &stage =
      m_ModelBase.DepletionStageEntry().StageByIndex(m_RGI.getCurrentDepletionStage().getDepletionStage());

  if (enforceUniqueness(uValueType)) {
    unlinkValueTypes(uValueType, stage);
  }

  if (!assignValues2Elements(elementValueSet, fluidPressureFracDetected, vcValues, stage, strPropertyName, uValueType,
                             firstVolumetricStrain)) {
    return false;
  }

  buildValueTypes(&pVT, elementValueSet, uValueType);
  linkValueTypes(pVT, stage, fluidPressureFracDetected);

  return true;
}

bool CLoadPropertyBase::valueSetIsPresent(const QString &valueTypeName) const {
  // make sure this valueset is not already present

  for (size_t i = 0; i < m_ModelBase.Mesh().referenceSize(); ++i) {
    const CGraphNode &ref = m_ModelBase.Mesh().referenceAt(i);
    const CValueType *pVT = dynamic_cast<const CValueType *>(&ref);

    if ((pVT != 0) && (pVT->Name() == valueTypeName)) {
      QString l = QString("A property with name '%1' is already present in the "
                          "model")
                      .arg(valueTypeName);

      m_RMP.AddLogLine(l, &m_RGI, false, true);

      return true;
    }
  }

  return false;
}

bool CLoadPropertyBase::loadValues(std::vector<geo::CValue> &vcValues, const QString &strPropertyName) const {
  std::vector<double> values;

  m_RGI.loadProperty(m_RGProperty, values);
  convert2geoCValue(vcValues, values);

  if (vcValues.size() != m_ModelBase.Mesh().Mesh().ElementSize()) {
    QString l = QString("Property '%1', stage %2, expected %3 values, got %4 "
                        "values")
                    .arg(strPropertyName)
                    .arg(m_RGI.getCurrentDepletionStage().getDepletionStage())
                    .arg(m_ModelBase.Mesh().Mesh().ElementSize())
                    .arg(vcValues.size());

    m_RMP.AddLogLine(l, &m_RGI, false, true);

    return false;
  }

  return true;
}

bool CLoadPropertyBase::assignValues2Elements(CElementValueSet &elementValueSet, bool &fluidPressureFracDetected,
                                              const std::vector<std::vector<geo::CValue>> &vcValues,
                                              const CDepletionStage &stage, const QString &strPropertyName,
                                              unsigned int uValueType, bool firstVolumetricStrain) const {
  bool fractureApertureFormations = hasFractureApertureFormations();

  for (int i = 0; i < vcValues.size(); ++i) {
    const geo::IElement &elm = m_ModelBase.Mesh().Mesh().Element(i);

    if (dynamic_cast<const geo::IInterfaceElement *>(&elm))
      continue;

    int nNod = elm.NrOfNodes();
    std::vector<geo::CValue> vcNodalValues = vcValues[i];
    std::vector<double> emptyValues(nNod);

    const CFormationBase *pFormation = m_ModelBase.Mesh().Formation(elm);

    if (!addDelta(vcNodalValues, strPropertyName, pFormation, elm, nNod)) {
      return false;
    }

    if (!firstVolumetricStrain && (uValueType == IDT_VALUETYPE_VOLUMETRICSTRAIN)) {
      addNodalValues(elementValueSet, i, vcNodalValues);
    } else if ((uValueType == IDT_VALUETYPE_PRESSURE) && fractureApertureFormations) {
      if (isFractureApertureModel(*pFormation, stage, elm)) {
        fluidPressureFracDetected = true;
      }

      elementValueSet.PushBack(vcNodalValues);
    } else {
      elementValueSet.PushBack(vcNodalValues);
    }
  }

  return true;
}

unsigned int CLoadPropertyBase::OverruleValueType(unsigned int uValueType) const { return uValueType; }

void CLoadPropertyBase::buildValueTypes(CValueType **pVT, CElementValueSet &elementValueSet,
                                        unsigned int uValueType) const {
  if ((*pVT) == 0) {
    (*pVT) = CValueTypeFactory::instance()->BuildValueType(m_ModelBase.Mesh(), OverruleValueType(uValueType),
                                                           m_ValueTypeName);
    (*pVT)->MapType(CValueType::MT_NONE);
  }

  if (!elementValueSet.IsLinkedTo((*pVT)->Component())) {
    elementValueSet.LinkTo((*pVT)->Component());
  }

  assert((*pVT)->IsLinkedTo(*m_ModelBase.GraphEntry(MD_BASE_VALUE_COMPOSITE)));
}

bool CLoadPropertyBase::hasFractureApertureFormations() const {
  TFormationBaseEntry *pFormationEntry = dynamic_cast<TFormationBaseEntry *>(m_ModelBase.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stNodes = pFormationEntry->EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator it;
  CDepletionStage &stage =
      m_ModelBase.DepletionStageEntry().StageByIndex(m_RGI.getCurrentDepletionStage().getDepletionStage());
  bool hasFractureApertureFormations = false;

  for (it = stNodes.begin(); it != stNodes.end(); ++it) {
    hasFractureApertureFormations = hasFractureApertureFormations || isFractureApertureModel(*(*it), stage);
  }

  return hasFractureApertureFormations;
}

void CLoadPropertyBase::linkValueTypes(CValueType *pVT, const CDepletionStage &stage,
                                       bool fluidPressureFracDetected) const {
  TFormationBaseEntry *pFormationEntry = dynamic_cast<TFormationBaseEntry *>(m_ModelBase.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stNodes = pFormationEntry->EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator it;

  for (it = stNodes.begin(); it != stNodes.end(); ++it) {
    linkValueTypes(*(*it), pVT, stage, fluidPressureFracDetected);
  }
}

void CLoadPropertyBase::addNodalValues(CElementValueSet &elementValueSet, int elementIndex,
                                       const std::vector<geo::CValue> &nodalValues) const {
  geo::CValueSet &valueSet = elementValueSet.getValueSet();
  geo::IValueSet::TValueVec valueVec(nodalValues.size());

  valueSet.ElementValues(valueVec, elementIndex, geo::IParallelInitializationCallback::Sequential);

  for (size_t i = 0; i < nodalValues.size(); ++i) {
    if (nodalValues[i].Valid())
      valueVec[i] += nodalValues[i];
  }

  valueSet.ElementValues(elementIndex, valueVec, geo::IParallelInitializationCallback::Sequential);
}

void CLoadPropertyBase::setAnisotropic(bool &isAnisotropic, bool &anisotropicIsSet, RGInterface &rgi,
                                       const CModelBase &modelBase, CRockMechProcessor &rmp) {
  if (!anisotropicIsSet && rmp.EnsureDepletionStageAvailable(rgi)) {
    const TFormationBaseEntry *pFormationEntry =
        dynamic_cast<const TFormationBaseEntry *>(modelBase.GraphEntry(MD_BASE_FORMATION));
    TFormationBaseEntry::TNodeSet stNodes = pFormationEntry->EntryNodes();
    TFormationBaseEntry::TNodeSet::const_iterator it;
    const CDepletionStage &stage =
        modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());

    for (it = stNodes.begin(); (it != stNodes.end()) && !isAnisotropic; ++it) {
      const IMaterialRock &materialRock = *(*it)->Material(stage).LibraryMaterial();

      if (materialRock.IsParameter(IDT_VALUETYPE_YOUNGMODULUS_NORM) &&
          materialRock.IsParameter(IDT_VALUETYPE_YOUNGMODULUS_TRANS) &&
          materialRock.IsParameter(IDT_VALUETYPE_POISSONRATIO_NORM) &&
          materialRock.IsParameter(IDT_VALUETYPE_POISSONRATIO_TRANS)) {
        isAnisotropic = true;
      }
    }

    anisotropicIsSet = true;
  }
}

void CLoadPropertyBase::unlinkValueTypes(unsigned int uValueType, const CDepletionStage &stage) const {
  TFormationBaseEntry *pFormationEntry = dynamic_cast<TFormationBaseEntry *>(m_ModelBase.GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stNodes = pFormationEntry->EntryNodes();

  for (TFormationBaseEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
    unlinkValueTypes(*(*it), uValueType, stage);
  }
}

void CLoadPropertyBase::unlinkValueTypes(CFormationBase &formation, unsigned int uValueType,
                                         const CDepletionStage &stage) const {
  CMaterialServer *matServer = formation.ConnectedMaterial(stage);

  if (matServer) {
    std::set<CValueType *> links = matServer->Links<CValueType>();

    for (std::set<CValueType *>::iterator it = links.begin(); it != links.end(); ++it) {
      if ((*it)->TypeId() == uValueType)
        matServer->UnLink(**it);
    }
  }
}

} // namespace GeomecRGI
