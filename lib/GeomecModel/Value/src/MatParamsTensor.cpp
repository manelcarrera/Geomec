#include "MatParamsTensor.h"

CMatParamsTensor::CMatTensorComponent::CMatTensorComponent(CValueType &value_type, const Units::CUnitType& unittype, 
               const Ranges::CRangeType& rangetype, unsigned int uComponentName, unsigned int uComponentIndex, unsigned int uExportLabel, unsigned int typeID/*, unsigned int iconID*/)
: CValueComponent(uComponentName, value_type, uComponentIndex, 0),
  m_UnitType(unittype),
  m_RangeType(rangetype),
  m_ExportLabel(uExportLabel), 
  m_TypeID(typeID)
{
}

CMatParamsTensor::CMatTensorComponent::CMatTensorComponent(const CMatTensorComponent& rhs)
: CValueComponent(rhs),
  m_UnitType(rhs.m_UnitType),
  m_RangeType(rhs.m_RangeType),
  m_ExportLabel(rhs.m_ExportLabel),
  m_TypeID(rhs.m_TypeID)
{
}

QString CMatParamsTensor::CMatTensorComponent::UnitName(const CQuantity::UNIT unit) const
{
  QString sRet;

  if(unit == CQuantity::SI_UNIT)
	  sRet = getStringTableEntry(m_UnitType.si_id());
  else
	  sRet = getStringTableEntry(m_UnitType.field_id());

  return sRet;
}

unsigned int CMatParamsTensor::CMatTensorComponent::TypeId() const
{
  return m_TypeID;
}

QString CMatParamsTensor::CMatTensorComponent::ExportLabel() const
{
  return getStringTableEntry(m_ExportLabel);
}

double CMatParamsTensor::CMatTensorComponent::FieldFactor() const
{
  return m_UnitType.field_factor();
}

bool CMatParamsTensor::CMatTensorComponent::IsInRange(const IValueSet& value_set) const
{
  // Check max range
  if(m_RangeType.hasMax())
  {
	  if(!value_set.Max().Valid())
		  return false;
	  if(m_RangeType.MaxInc())
	  {
		  if(value_set.Max().Value() > m_RangeType.MaxVal())
			  return false;
	  }
	  else
	  {
		  if(value_set.Max().Value() >= m_RangeType.MaxVal())
			  return false;
	  }
  }

  if(m_RangeType.hasMin())
  {
	  if(!value_set.Min().Valid())
		  return false;
	  if(m_RangeType.MinInc())
	  {
		  if(value_set.Min().Value() < m_RangeType.MinVal())
			  return false;
	  }
	  else
	  {
		  if(value_set.Min().Value() <= m_RangeType.MinVal())
			  return false;
	  }
  }

  return true;
}

geo::CValue CMatParamsTensor::CMatTensorComponent::RangeMin(const CQuantity::UNIT unit) const
{
  if(m_RangeType.hasMin())
  {
	  if(unit == CQuantity::SI_UNIT)
		  return geo::CValue(m_RangeType.MinVal() * 1.0);
	  return geo::CValue(m_RangeType.MinVal() * m_UnitType.field_factor());
  }
  return geo::CValue();
}

geo::CValue CMatParamsTensor::CMatTensorComponent::RangeMax(const CQuantity::UNIT unit) const
{
  if(m_RangeType.hasMax())
  {
	  if(unit == CQuantity::SI_UNIT)
		  return geo::CValue(m_RangeType.MaxVal() * 1.0);
	  return geo::CValue(m_RangeType.MaxVal() * m_UnitType.field_factor());
  }
  return geo::CValue();
}
  
CMatParamsTensor::CMatParamsTensor(unsigned int uExportID, unsigned int uTypeID, unsigned int uIconID,
                                   IPointSet& point_set, const QString &strName)
:CValueType(point_set, strName), m_CurrentIndex(0), m_ExportID(uExportID), m_TypeID(uTypeID), m_IconID(uIconID), m_TypeName(strName)
{
}

QString CMatParamsTensor::ExportType() const 
{
  return getStringTableEntry(m_ExportID);
}

unsigned int CMatParamsTensor::TypeId() const
{
  return m_TypeID;
}

unsigned int CMatParamsTensor::IconId() const
{
  return m_IconID;
}

void CMatParamsTensor::AddComponent(unsigned int uComponentName, unsigned int uExportLabel, unsigned int typeID,
    const Units::CUnitType &unittype, const Ranges::CRangeType &rangetype)
{
  new CMatTensorComponent(*this, unittype, rangetype, uComponentName, m_CurrentIndex++, uExportLabel, typeID);
}

IValueComponentBase& CMatParamsTensor::Component(unsigned int uComponent, unsigned int uMode)
{
  if(ModeSize() == 0)
  {
    AddComponents();
  }
  return CValueType::Component(uComponent, uMode);
}

const IValueComponentBase& CMatParamsTensor::Component(unsigned int uComponent, unsigned int uMode) const
{
  if(ModeSize() == 0)
  {
    CMatParamsTensor *self = const_cast<CMatParamsTensor*>(this);
    self->AddComponents();
  }

  return CValueType::Component(uComponent, uMode);
}

unsigned int CMatParamsTensor::ComponentTypeID(unsigned int uComponent) 
{
  CMatTensorComponent &comp = (CMatTensorComponent&)(Component(uComponent));
  return comp.TypeId();
}

unsigned int CMatParamsTensor::ComponentTypeID(unsigned int uComponent) const
{
  CMatTensorComponent &comp = (CMatTensorComponent&)(Component(uComponent));
  return comp.TypeId();
}
QString CMatParamsTensor::TypeName() const
{
  return m_TypeName;
}
