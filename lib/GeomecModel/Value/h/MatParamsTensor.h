#if !defined(__MATPARAMSTENSOR_H__)
#define __MATPARAMSTENSOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueComponent.h"
#include "ValueType.h"


class CMatParamsTensor : public CValueType
{
public:
  class CMatTensorComponent : public CValueComponent
  {
  private:
  Units::CUnitType m_UnitType;
  Ranges::CRangeType m_RangeType;
  unsigned int m_ExportLabel;
  unsigned int m_TypeID;
 
  public:
  CMatTensorComponent(CValueType &value_type, const Units::CUnitType& unittype,
      const Ranges::CRangeType& rangetype, unsigned int uComponentName, 
      unsigned int uComponentIndex, unsigned int uExportLabel, unsigned int typeID);
    CMatTensorComponent(const CMatTensorComponent& rhs);

  virtual unsigned int TypeId() const;
    virtual QString ExportLabel() const;
  
    virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
    virtual double FieldFactor() const;
    virtual bool IsInRange(const IValueSet& value_set) const;
    virtual geo::CValue RangeMin(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual geo::CValue RangeMax(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
   };

private:
  int m_CurrentIndex;
  unsigned int m_ExportID;
  unsigned int m_TypeID;
  unsigned int m_IconID;
  QString m_TypeName;
public:
  CMatParamsTensor(unsigned int uExportID, unsigned int uTypeID, unsigned int uIconID, 
  IPointSet& point_set, const QString &strName);

  virtual IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0);
  virtual const IValueComponentBase& Component(unsigned int uComponent = 0, unsigned int uMode = 0) const;
  unsigned int ComponentTypeID(unsigned int uComponent);
  unsigned int ComponentTypeID(unsigned int uComponent) const;

  void AddComponent(unsigned int uComponentName, unsigned int uExportLabel, unsigned int uTypeID,
  const Units::CUnitType &unittype, const Ranges::CRangeType &rangetype);
  virtual QString ExportType() const;
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual void AddComponents() = 0;

  static unsigned int icon_id() { assert(false); return 0; }

  ACCEPT_GEOMECMODELVISITORS(VisitMatParamsTensor);
};

#endif