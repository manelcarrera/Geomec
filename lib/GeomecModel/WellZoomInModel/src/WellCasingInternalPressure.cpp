
#include "WellCasingModel.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingNode.h"
#include "FieldFactor.h"
#include "DepletionStage.h"
#include "ValueTypes.h"
#include "BodyGroup.h"

CWellCasingInternalPressure::CWellCasingInternalPressure(CDepletionStage& stage)
: IWellCasingInternalLoad<TPressure>(stage)
{
  m_pRepeater = new CWellCasingInternalPressureComponentRepeater(*this);
  m_pGradient = new CWellCasingInternalPressureComponentGradient(*this);

  if(stage.Initial())
    LoadingMode(LM_GRADIENT);
}

CWellCasingInternalPressure::CWellCasingInternalPressure(const CWellCasingInternalPressure& rhs)
: IWellCasingInternalLoad<TPressure>::IWellCasingInternalLoad(rhs),
  m_pRepeater(new CWellCasingInternalPressureComponentRepeater(*rhs.m_pRepeater)),
  m_pGradient(new CWellCasingInternalPressureComponentGradient(*rhs.m_pGradient))
{
}

CWellCasingInternalPressure& CWellCasingInternalPressure::operator=(const CWellCasingInternalPressure& rhs)
{
  if(!operator==(rhs))
  {
    IWellCasingInternalLoad<TPressure>::operator=(rhs);

    *m_pRepeater = *rhs.m_pRepeater;
    *m_pGradient = *rhs.m_pGradient;
  }

  return *this;
}

bool CWellCasingInternalPressure::operator==(const CWellCasingInternalPressure& rhs) const
{
  if(!IWellCasingInternalLoad<TPressure>::operator==(rhs))
    return false;

  return (
    *m_pRepeater == *rhs.m_pRepeater &&
    *m_pGradient == *rhs.m_pGradient);
}

unsigned int CWellCasingInternalPressure::IconId() const
{
  return Component().IconId();
}

unsigned int CWellCasingInternalPressure::TypeId() const
{
  return IDT_VALUETYPE_PRESSURE;
}

bool CWellCasingInternalPressure::Less(const CGraphNode& node) const
{
  const CWellCasingInternalPressure* pPressure = dynamic_cast<const CWellCasingInternalPressure*>(&node);
  if(pPressure)
    return Stage().Less(pPressure->Stage());

  if(dynamic_cast<const CWellCasingInternalTemperature*>(&node))
    return true;

  return false;
}

QString CWellCasingInternalPressure::LoadName() const
{
  return "Pressure";
}

const CWellCasingInternalPressure& CWellCasingInternalPressure::PreviousStageLoad() const
{
  assert(!Stage().Initial());
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  return model.CasingNode().InternalPressure(Stage().Previous());
}

const CWellCasingInternalPressure& CWellCasingInternalPressure::NextStageLoad() const
{
  assert(!Stage().Last());
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  return model.CasingNode().InternalPressure(Stage().Next());
}

geo::CPoint CWellCasingInternalPressure::Min() const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  return model.Mesh().SteelElements().Min();
}

geo::CPoint CWellCasingInternalPressure::Max() const
{
  const CWellCasingModel& model = static_cast<const CWellCasingModel&>(Model());
  return model.Mesh().SteelElements().Max();
}

QString CWellCasingInternalPressure::TypeName() const
{
  // TODO MAGIC
  return "Internal Pressure";
}

bool CWellCasingInternalPressure::Empty() const
{
  return false;
}

const CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater& CWellCasingInternalPressure::Repeater() const
{
  return *m_pRepeater;
}

CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater& CWellCasingInternalPressure::Repeater()
{
  return *m_pRepeater;
}

const CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient& CWellCasingInternalPressure::Gradient() const
{
  return *m_pGradient;
}

CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient& CWellCasingInternalPressure::Gradient()
{
  return *m_pGradient;
}

void CWellCasingInternalPressure::LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  m_pRepeater->LoadStream(stream, version, progress);
  m_pGradient->LoadStream(stream, version, progress);
}

void CWellCasingInternalPressure::SaveComponents(TSTREAM& stream, TPROGRESS& progress)
{
  m_pRepeater->SaveStream(stream, progress);
  m_pGradient->SaveStream(stream, progress);
}

long CWellCasingInternalPressure::SavedItemsComponents() const
{
  return m_pRepeater->SavedItems() + m_pGradient->SavedItems();
}


///// CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater

CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::CWellCasingInternalPressureComponentRepeater(CWellCasingInternalPressure& parent)
: IWellCasingInternalLoadComponentRepeater<TPressure>(parent)
{
}

CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::CWellCasingInternalPressureComponentRepeater(const CWellCasingInternalPressureComponentRepeater& rhs)
: IWellCasingInternalLoadComponentRepeater<TPressure>(rhs)
{
}

unsigned int CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::IconId() const
{
  if(ParentLoad().DistributedSize() > 0)
    return IDI_COMPONENT_PRESSURE_REPEAT_CHANGE;

  return IDI_COMPONENT_PRESSURE_REPEAT;
}

double CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::UnitFactor(CQuantity::UNIT unit) const
{
  if(unit == CDoubleQuantity::FIELD_UNIT)
    return FF_FACTOR_PRESSURE;

  return 1;
}

QString CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::UnitName(const CQuantity::UNIT unit) const
{
	QString sRet;
	if(unit == CQuantity::SI_UNIT)
		sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
	else
		sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

	return sRet;
}

QString CWellCasingInternalPressure::CWellCasingInternalPressureComponentRepeater::ExportLabel() const
{
  return getStringTableEntry(IDS_ET_CASINGINTERNALPRESSURE);
}


///// CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient

CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::CWellCasingInternalPressureComponentGradient(CWellCasingInternalPressure& parent)
: IWellCasingInternalLoadComponentGradient<TPressure>(parent)
{
}

CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::CWellCasingInternalPressureComponentGradient(const CWellCasingInternalPressureComponentGradient& rhs)
: IWellCasingInternalLoadComponentGradient<TPressure>(rhs)
{
}

unsigned int CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::IconId() const
{
  return IDI_COMPONENT_PRESSURE_CONSTANT;
}

double CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::UnitFactor(CQuantity::UNIT unit) const
{
  if(unit == CDoubleQuantity::FIELD_UNIT)
    return FF_FACTOR_PRESSURE;

  return 1;
}

QString CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::UnitName(const CQuantity::UNIT unit) const
{
	QString sRet;
	if(unit == CQuantity::SI_UNIT)
		sRet = getStringTableEntry(IDS_UNIT_SI_PRESSURE);
	else
		sRet = getStringTableEntry(IDS_UNIT_FIELD_PRESSURE);

	return sRet;
}

QString CWellCasingInternalPressure::CWellCasingInternalPressureComponentGradient::ExportLabel() const
{
  return getStringTableEntry(IDS_ET_CASINGINTERNALPRESSURE);
}
