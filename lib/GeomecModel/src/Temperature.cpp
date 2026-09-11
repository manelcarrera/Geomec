
#include "Temperature.h"
#include "DepletionStage.h"
#include "FormationBase.h"
#include "GeomecStringTable.h"
#include "GlobalTemperature.h"
#include "ValueTypeFactory.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

///// CTemperatureGradientComponent

CTemperatureGradientComponent::CTemperatureGradientComponent(CFormationLoadBase<TTemperature> &temperature, bool bEmpty)
    : CFormationLoadGradientComponent<TTemperature, CTemperatureQuantity, CTemperatureGradientQuantity>(temperature,
                                                                                                        bEmpty) {}

CTemperatureGradientComponent::CTemperatureGradientComponent(const CTemperatureGradientComponent &rhs)
    : CFormationLoadGradientComponent<TTemperature, CTemperatureQuantity, CTemperatureGradientQuantity>(rhs) {}

unsigned int CTemperatureGradientComponent::TypeId() const { return IDT_COMPONENT_TEMPERATURE_GRADIENT; }

unsigned int CTemperatureGradientComponent::IconId() const { return IDI_COMPONENT_TEMPERATURE_GRADIENT; }

QString CTemperatureGradientComponent::TypeName() const {
  return getStringTableEntry(IDS_COMPONENT_TEMPERATURE_GRADIENT);
}

QString CTemperatureGradientComponent::UnitName(const CQuantity::UNIT /*unit*/) const {
  return QString::fromLocal8Bit(getStringTableEntry(IDS_UNIT_TEMPERATURE));
}

QString CTemperatureGradientComponent::ExportLabel() const { return getStringTableEntry(IDS_ET_TEMPERATURE); }

///// CTemperatureGWCDummyComponent

CTemperatureGWCDummyComponent::CTemperatureGWCDummyComponent() : IValueComponentBase(*this) { assert(false); }

CTemperatureGWCDummyComponent::CTemperatureGWCDummyComponent(CTemperatureGWCDummyComponent &rhs)
    : IValueComponentBase(rhs) {
  assert(false);
}

const CTemperatureQuantity &CTemperatureGWCDummyComponent::ContactPressure() const {
  assert(false);
  CTemperatureQuantity *p = 0;
  return *p;
}

const CTemperatureQuantity &CTemperatureGWCDummyComponent::TransitionPressure() const {
  assert(false);
  CTemperatureQuantity *p = 0;
  return *p;
}

const CLengthQuantity &CTemperatureGWCDummyComponent::ContactDepth() const {
  assert(false);
  CLengthQuantity *p = 0;
  return *p;
}

const CLengthQuantity &CTemperatureGWCDummyComponent::TransitionDepth() const {
  assert(false);
  CLengthQuantity *p = 0;
  return *p;
}

const CTemperatureGradientQuantity &CTemperatureGWCDummyComponent::UpperGradient() const {
  assert(false);
  CTemperatureGradientQuantity *p = 0;
  return *p;
}

const CTemperatureGradientQuantity &CTemperatureGWCDummyComponent::LowerGradient() const {
  assert(false);
  CTemperatureGradientQuantity *p = 0;
  return *p;
}

bool CTemperatureGWCDummyComponent::TransitionFromPrevious() const {
  assert(false);
  return false;
}

void CTemperatureGWCDummyComponent::Set(const double & /*dContactPressure*/, const double & /*dTransitionPressure*/,
                                        const double & /*dContactDepth*/, const double & /*dTransitionDepth*/,
                                        const double & /*dUpperGradient*/, const double & /*dLowerGradient*/,
                                        bool /*bTransitionFromPrevious*/, const CQuantity::UNIT /*unit*/) {
  assert(false);
}

///// CTemperatureRepeaterComponent

CTemperatureRepeaterComponent::CTemperatureRepeaterComponent(CTemperature &temperature)
    : CFormationLoadRepeaterComponent<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent,
                                      CTemperatureGWCDummyComponent>(temperature) {}

CTemperatureRepeaterComponent::CTemperatureRepeaterComponent(const CTemperatureRepeaterComponent &rhs)
    : CFormationLoadRepeaterComponent<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent,
                                      CTemperatureGWCDummyComponent>(rhs) {}

unsigned int CTemperatureRepeaterComponent::TypeId() const {
  const CTemperature *pTemperature = dynamic_cast<const CTemperature *>(&FormationLoadBase());
  assert(pTemperature);
  switch (pTemperature->Type()) {
  case CTemperature::CONSTANT_MODE:
    return pTemperature->Constant().TypeId();
  case CTemperature::REPEAT_MODE:
    return IDT_COMPONENT_TEMPERATURE_REPEATER;
  default:
    assert(false); // Bogus
  };

  return 0;
}

unsigned int CTemperatureRepeaterComponent::IconId() const {
  const CTemperature *pTemperature = dynamic_cast<const CTemperature *>(&FormationLoadBase());
  assert(pTemperature);

  const CTemperature *pParentTemperature = pTemperature->ParentTemperature();

  bool bSimilarToParent = pParentTemperature && pTemperature->IsSimilar(*pParentTemperature, false);

  switch (pTemperature->Type()) {
  case CTemperature::CONSTANT_MODE:
    if (bSimilarToParent && pTemperature->Constant() == pParentTemperature->Constant())
      return IDI_COMPONENT_TEMPERATURE_PARENT;
    else
      return pTemperature->Constant().IconId();
  case CTemperature::REPEAT_MODE:
    if (pTemperature->DepletionStage().Initial()) {
      if (bSimilarToParent)
        return IDI_COMPONENT_TEMPERATURE_PARENT;
      else
        return IDI_INITIAL_TEMPERATURE;
    } else {
      const CTemperature *pPrevTemperature =
          &pTemperature->Formation()->UserTemperature(pTemperature->DepletionStage().Previous());
      assert(pPrevTemperature);

      if (pTemperature->IsSimilar(*pPrevTemperature))
        return IDI_COMPONENT_TEMPERATURE_REPEATER;

      if (bSimilarToParent)
        return IDI_COMPONENT_TEMPERATURE_PARENT;

      return IDI_COMPONENT_TEMPERATURE_REPEATER_CHANGE;
    }
  default:
    assert(false);
  };

  return 0;
}

QString CTemperatureRepeaterComponent::TypeName() const {
  const CTemperature *pTemperature = dynamic_cast<const CTemperature *>(&FormationLoadBase());
  assert(pTemperature);
  switch (pTemperature->Type()) {
  case CTemperature::CONSTANT_MODE:
    return pTemperature->Constant().TypeName();
  case CTemperature::REPEAT_MODE: {
    return getStringTableEntry(IDS_COMPONENT_TEMPERATURE_REPEATER);
  }
  default:
    assert(false); // Bogus
  };

  return QString();
}

QString CTemperatureRepeaterComponent::UnitName(const CQuantity::UNIT /*unit*/) const {
  return QString::fromLocal8Bit(getStringTableEntry(IDS_UNIT_TEMPERATURE));
}

QString CTemperatureRepeaterComponent::ExportLabel() const { return getStringTableEntry(IDS_ET_TEMPERATURE); }

const CTemperatureRepeaterComponent::TFormationLoad &CTemperatureRepeaterComponent::PreviousFormationLoad() const {
  const CTemperature *pTemperature = dynamic_cast<const CTemperature *>(&FormationLoadBase());
  assert(pTemperature);
  assert(!pTemperature->DepletionStage().Initial());
  return pTemperature->Formation()->UserTemperature(pTemperature->DepletionStage().Previous());
}

const IValueComponentBase &CTemperatureRepeaterComponent::GlobalValues() const {
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  return model.InitialTemperature().Component();
}

geo::CValue CTemperatureRepeaterComponent::ValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit,
                                                      geo::IParallelInitializationCallback *cb) const {
  geo::CValue val =
      CFormationLoadRepeaterComponent<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent,
                                      CTemperatureGWCDummyComponent>::ValuePoint(pt, unit, cb);

  if (val.Valid() && val.Value() < -272.15) // prevent values smaller than 1K (-272.15 C)
    val = -272.15;

  return val;
}

IValueDomainScalar::TValueVec
CTemperatureRepeaterComponent::ValueElement(const geo::IElement &elm, const CQuantity::UNIT unit,
                                            geo::IParallelInitializationCallback *cb) const {
  IValueDomainScalar::TValueVec vcValues =
      CFormationLoadRepeaterComponent<TTemperature, CTemperatureGradientComponent, CTemperatureRepeaterComponent,
                                      CTemperatureGWCDummyComponent>::ValueElement(elm, unit, cb);

  for (size_t i = 0; i < vcValues.size(); ++i) {
    if (vcValues[i].Valid() && vcValues[i].Value() < -272.15) // prevent values smaller than 1K (-272.15 C)
      vcValues[i] = -272.15;
  }

  return vcValues;
}

///// CTemperature

CTemperature::CTemperature(CFormationBase &f, CDepletionStage &d, bool bEmpty)
    : TTemperatureBase(f, d), m_GlobalTemperatureObserver(new CGlobalValueObserver<CTemperature, CGlobalTemperature>(
                                  this, &(static_cast<CModelBase &>(f.Model()).InitialTemperature()))) {
  LinkTo(d);    // Link to depletion stage
  reParent(&f); // Link to formation

  if (!bEmpty) {
    new CTemperatureRepeaterComponent(*this);
    new CTemperatureGradientComponent(*this);
  }
}

CTemperature::CTemperature(CFormationBase &f, CDepletionStage &d, const double &dReferenceTemperature,
                           const double &dReferenceDepth, const double &dGradient, const CQuantity::UNIT unit)
    : TTemperatureBase(f, d), m_GlobalTemperatureObserver(new CGlobalValueObserver<CTemperature, CGlobalTemperature>(
                                  this, &(static_cast<CModelBase &>(f.Model()).InitialTemperature()))) {
  LinkTo(d);    // Link to depletion stage
  reParent(&f); // Link to formation

  new CTemperatureRepeaterComponent(*this); // Create repeater component
  new CTemperatureGradientComponent(*this); // Create constant component

  Constant().Set(dReferenceTemperature, dReferenceDepth, dGradient, unit);

  assert(Type() == CONSTANT_MODE);
}
// Constant initialisation
CTemperature::CTemperature(CFormationBase &f, CDepletionStage &d, TTemperature & /*p*/)
    : TTemperatureBase(f, d), m_GlobalTemperatureObserver(new CGlobalValueObserver<CTemperature, CGlobalTemperature>(
                                  this, &(static_cast<CModelBase &>(f.Model()).InitialTemperature()))) {
  LinkTo(d);    // Link to depletion stage
  reParent(&f); // Link to formation

  new CTemperatureRepeaterComponent(*this); // Create repeater component
  new CTemperatureGradientComponent(*this); // Create constant component
}

CTemperature::CTemperature(const CTemperature &rhs)
    : TTemperatureBase(rhs), m_GlobalTemperatureObserver(new CGlobalValueObserver<CTemperature, CGlobalTemperature>(
                                 *rhs.m_GlobalTemperatureObserver)) {}

CTemperature &CTemperature::operator=(const CTemperature &rhs) {
  TTemperatureBase::operator=(rhs);

  CModelBase &model = static_cast<CModelBase &>(Model());
  model.ResultRegister().ClearAll();

  return *this;
}

CTemperature::~CTemperature() { delete m_GlobalTemperatureObserver; }

unsigned int CTemperature::TypeId() const { return IDT_VALUETYPE_TEMPERATURE; }

QString CTemperature::TypeName() const { return getStringTableEntry(IDS_VALUENAME_TEMPERATURE); }

void CTemperature::OnNeighbourDeleted(const CGraphNode &node) {
  if (dynamic_cast<const TTemperature *>(&node)) {
    CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
    if (!pModel->getDeleting())
      pModel->ResultRegister().ClearAll();
  }

  TTemperatureBase::OnNeighbourDeleted(node);
}

void CTemperature::OnNewNeighbour(const CGraphNode &node) {
  if (dynamic_cast<const TTemperature *>(&node)) {
    CModelBase *pModel = static_cast<CModelBase *>(&Model());
    pModel->ResultRegister().ClearAll();
  }

  TTemperatureBase::OnNewNeighbour(node);
}

bool CTemperature::IsFirstValidDepletionStage() const { return (DepletionStage().Initial() != false); }

void CTemperature::LoadComponents(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  new CTemperatureRepeaterComponent(*this); // Create repeater component

  switch (Type()) {
  case REPEAT_MODE:
    new CTemperatureGradientComponent(*this);
    break;
  case CONSTANT_MODE: {
    CTemperatureGradientComponent *pConstant = new CTemperatureGradientComponent(*this, true);
    pConstant->LoadStream(stream, version, progress);
  } break;
  default:
    assert(false);
    break;
  };
}

void CTemperature::SaveComponents(TSTREAM &stream, TPROGRESS &progress) {
  if (Type() == CONSTANT_MODE)
    Constant().SaveStream(stream, progress);
}

TTemperatureBase &CTemperature::FormationStageLoad(CFormationBase &formation, CDepletionStage &stage) {
  return formation.UserTemperature(stage);
}

const CTemperature *CTemperature::ParentTemperature() const {
  const CFormationBase *pFormation = Formation();
  const CTemperature *pParentTemperature = 0;

  if (pFormation && pFormation->ParentFormation())
    pParentTemperature = &pFormation->ParentFormation()->UserTemperature(
        static_cast<const CModelBase &>(pFormation->Model()).ParentEquivalentDepletionStage(DepletionStage()));

  return pParentTemperature;
}
