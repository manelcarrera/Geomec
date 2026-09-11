
#include "WellCasingInternalTemperature.h"
#include "BodyGroup.h"
#include "DepletionStage.h"
#include "FieldFactor.h"
#include "WellCasingModel.h"
#include "WellCasingNode.h"

CWellCasingInternalTemperature::CWellCasingInternalTemperature(CDepletionStage &stage)
    : IWellCasingInternalLoad<TTemperature>(stage) {
  m_pRepeater = new CWellCasingInternalTemperatureComponentRepeater(*this);
  m_pGradient = new CWellCasingInternalTemperatureComponentGradient(*this);

  if (stage.Initial())
    LoadingMode(LM_GRADIENT);
}

CWellCasingInternalTemperature::CWellCasingInternalTemperature(const CWellCasingInternalTemperature &rhs)
    : IWellCasingInternalLoad<TTemperature>::IWellCasingInternalLoad(rhs),
      m_pRepeater(new CWellCasingInternalTemperatureComponentRepeater(*rhs.m_pRepeater)),
      m_pGradient(new CWellCasingInternalTemperatureComponentGradient(*rhs.m_pGradient)) {}

CWellCasingInternalTemperature &CWellCasingInternalTemperature::operator=(const CWellCasingInternalTemperature &rhs) {
  if (!operator==(rhs)) {
    IWellCasingInternalLoad<TTemperature>::operator=(rhs);

    *m_pRepeater = *rhs.m_pRepeater;
    *m_pGradient = *rhs.m_pGradient;
  }

  return *this;
}

bool CWellCasingInternalTemperature::operator==(const CWellCasingInternalTemperature &rhs) const {
  if (!IWellCasingInternalLoad<TTemperature>::operator==(rhs))
    return false;

  return (*m_pRepeater == *rhs.m_pRepeater && *m_pGradient == *rhs.m_pGradient);
}

unsigned int CWellCasingInternalTemperature::IconId() const { return Component().IconId(); }

unsigned int CWellCasingInternalTemperature::TypeId() const { return IDT_VALUETYPE_TEMPERATURE; }

bool CWellCasingInternalTemperature::Less(const CGraphNode &node) const {
  const CWellCasingInternalTemperature *pTemperature = dynamic_cast<const CWellCasingInternalTemperature *>(&node);
  if (pTemperature)
    return Stage().Less(pTemperature->Stage());

  return false;
}

QString CWellCasingInternalTemperature::LoadName() const { return "Temperature"; }

const CWellCasingInternalTemperature &CWellCasingInternalTemperature::PreviousStageLoad() const {
  assert(!Stage().Initial());
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  return model.CasingNode().InternalTemperature(Stage().Previous());
}

const CWellCasingInternalTemperature &CWellCasingInternalTemperature::NextStageLoad() const {
  assert(!Stage().Last());
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  return model.CasingNode().InternalTemperature(Stage().Next());
}

geo::CPoint CWellCasingInternalTemperature::Min() const {
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  return model.Mesh().SteelElements().Min();
}

geo::CPoint CWellCasingInternalTemperature::Max() const {
  const CWellCasingModel &model = static_cast<const CWellCasingModel &>(Model());
  return model.Mesh().SteelElements().Max();
}

QString CWellCasingInternalTemperature::TypeName() const {
  // TODO MAGIC
  return "Internal Temperature";
}

bool CWellCasingInternalTemperature::Empty() const { return false; }

long CWellCasingInternalTemperature::SavedItems() const { return 0; }

const CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater &
CWellCasingInternalTemperature::Repeater() const {
  return *m_pRepeater;
}

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater &
CWellCasingInternalTemperature::Repeater() {
  return *m_pRepeater;
}

const CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient &
CWellCasingInternalTemperature::Gradient() const {
  return *m_pGradient;
}

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient &
CWellCasingInternalTemperature::Gradient() {
  return *m_pGradient;
}

void CWellCasingInternalTemperature::LoadComponents(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  m_pRepeater->LoadStream(stream, version, progress);
  m_pGradient->LoadStream(stream, version, progress);
}

void CWellCasingInternalTemperature::SaveComponents(TSTREAM &stream, TPROGRESS &progress) {
  m_pRepeater->SaveStream(stream, progress);
  m_pGradient->SaveStream(stream, progress);
}

long CWellCasingInternalTemperature::SavedItemsComponents() const {
  return m_pRepeater->SavedItems() + m_pGradient->SavedItems();
}

///// CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::
    CWellCasingInternalTemperatureComponentRepeater(CWellCasingInternalTemperature &parent)
    : IWellCasingInternalLoadComponentRepeater<TTemperature>(parent) {}

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::
    CWellCasingInternalTemperatureComponentRepeater(const CWellCasingInternalTemperatureComponentRepeater &rhs)
    : IWellCasingInternalLoadComponentRepeater<TTemperature>(rhs) {}

unsigned int CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::IconId() const {
  if (ParentLoad().DistributedSize() > 0)
    return IDI_COMPONENT_TEMPERATURE_REPEATER_CHANGE;

  return IDI_COMPONENT_TEMPERATURE_REPEATER;
}

double CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::UnitFactor(
    CQuantity::UNIT unit) const {
  if (unit == CDoubleQuantity::FIELD_UNIT)
    return FF_FACTOR_TEMPERATURE;

  return 1;
}

QString CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::UnitName(
    const CQuantity::UNIT /*unit*/) const {
  return getStringTableEntry(IDS_UNIT_TEMPERATURE);
}

QString CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentRepeater::ExportLabel() const {
  return getStringTableEntry(IDS_ET_CASINGINTERNALTEMPERATURE);
}

///// CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::
    CWellCasingInternalTemperatureComponentGradient(CWellCasingInternalTemperature &parent)
    : IWellCasingInternalLoadComponentGradient<TTemperature>(parent) {}

CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::
    CWellCasingInternalTemperatureComponentGradient(const CWellCasingInternalTemperatureComponentGradient &rhs)
    : IWellCasingInternalLoadComponentGradient<TTemperature>(rhs) {}

unsigned int CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::IconId() const {
  return IDI_COMPONENT_TEMPERATURE_GRADIENT;
}

double CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::UnitFactor(
    CQuantity::UNIT unit) const {
  if (unit == CDoubleQuantity::FIELD_UNIT)
    return FF_FACTOR_TEMPERATURE;

  return 1;
}

QString CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::UnitName(
    const CQuantity::UNIT /*unit*/) const {
  return getStringTableEntry(IDS_UNIT_TEMPERATURE);
}

QString CWellCasingInternalTemperature::CWellCasingInternalTemperatureComponentGradient::ExportLabel() const {
  return getStringTableEntry(IDS_ET_CASINGINTERNALTEMPERATURE);
}
