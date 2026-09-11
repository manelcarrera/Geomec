// ValueComponent.h: interface for the CValueComponent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUECOMPONENT_H__6EB7525B_6874_4F83_8D60_59CD829C568D__INCLUDED_)
#define AFX_VALUECOMPONENT_H__6EB7525B_6874_4F83_8D60_59CD829C568D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValueSet.h"

namespace geo {
class IValueSet;
}

class IValueSet;

#include "GeomecIValueSet.h"
#include "GeomecStringTable.h"
#include "ivaluecomponent.h"

#include "GeomecModelVisitor.h"
#include "IValueDataInterface.h"
#include "ValueType.h"
#include "ivaluecomponent.h"

class CValueComponent : public IValueComponentBase {
  IValueSet *m_pValueSet;
  mutable geo::IValueSet *m_pSIValueSet;
  mutable geo::IValueSet *m_pFieldValueSet;
  IValueDomainScalar::TValue Convert(const IValueDomainScalar::TValue &value, CQuantity::UNIT unit) const;

private:
  IValueDomainScalar::TValue InterpolateValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit,
                                                   geo::IParallelInitializationCallback *cb) const;

public:
  class CValueSet : public geo::IValueSet {
    const CValueComponent &m_component;

  public:
    CValueSet(const CValueComponent &component);
    virtual const geo::IElementSet &ElementSet() const;
    virtual void ElementValues(TValueVec &values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
    CQuantity::UNIT Unit() const;
    virtual void Empty();
    virtual bool IsEmpty() const;

  protected:
    virtual int PushBack(const TValueVec &values);
  };
  friend class CValueSet;

  typedef IValueDataInterfaceScalarTempl<CValueComponent> IValueDataInterfaceValueComponentScalar;

  // Construction ...
  CValueComponent(unsigned int uName, CValueType &value_type, IValueSet &value_set, unsigned int uComponent,
                  unsigned int uMode);
  CValueComponent(const QString &sName, CValueType &value_type, IValueSet &value_set, unsigned int uComponent,
                  unsigned int uMode);
  CValueComponent(unsigned int uName, CValueType &value_type, unsigned int uComponent, unsigned int uMode);
  CValueComponent(const QString &sName, CValueType &value_type, unsigned int uComponent, unsigned int uMode);
  CValueComponent(CValueType &value_type, IValueSet &value_set, unsigned int uComponent, unsigned int uMode);
  CValueComponent(CValueType &value_type, unsigned int uComponent, unsigned int uMode);
  CValueComponent(const CValueComponent &rhs);
  virtual ~CValueComponent();

  virtual TValueComponentType Type() const;

  virtual const IValueDataInterfaceScalar &ScalarData() const;
  virtual IValueDataInterfaceScalar &ScalarData();

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  // ValueSet access
  const IValueSet &ValueSet() const;
  IValueSet &ValueSet();

  const geo::IValueSet &MapValueSet(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  // Notification
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // Assignment and equal
  bool operator==(const CValueComponent &rhs) const;
  CValueComponent &operator=(const CValueComponent &rhs);

  // Copy over rides ....
  virtual const QString &Name() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual unsigned int IconId() const;
  virtual double FieldFactor() const;
  virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  // Value discription ...
  virtual bool Defined() const;
  virtual IValueDomainScalar::TMinMax MinMax(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm,
                                                     const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                     geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValue Average(IProgressBase &progressBase,
                                             const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  // Save and load
  virtual bool Empty() const;
  virtual long SavedItems() const;

  // Testing the range ...
  virtual bool IsInRange(const IValueSet &value_set) const;
  virtual geo::CValue RangeMin(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual geo::CValue RangeMax(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  ACCEPT_GEOMECMODELVISITORS(VisitValueComponent);

protected:
  IValueDataInterfaceValueComponentScalar m_Data;
};

class Ranges {
public:
  class CRangeType {
  public:
    CRangeType(bool bMin, double dMin, bool bMinInclusive, bool bMax, double dMax, bool bMaxInclusive)
        : m_bMin(bMin), m_dMin(dMin), m_bMinInclusive(bMinInclusive), m_bMax(bMax), m_dMax(dMax),
          m_bMaxInclusive(bMaxInclusive) {}

    CRangeType(const CRangeType &rhs)
        : m_bMin(rhs.m_bMin), m_dMin(rhs.m_dMin), m_bMinInclusive(rhs.m_bMinInclusive), m_bMax(rhs.m_bMax),
          m_dMax(rhs.m_dMax), m_bMaxInclusive(rhs.m_bMaxInclusive) {}

    bool hasMin() const { return m_bMin; }
    double MinVal() const { return m_dMin; }
    bool MinInc() const { return m_bMinInclusive; }
    bool hasMax() const { return m_bMax; }
    double MaxVal() const { return m_dMax; }
    bool MaxInc() const { return m_bMaxInclusive; }

  private:
    bool m_bMin;
    double m_dMin;
    bool m_bMinInclusive;
    bool m_bMax;
    double m_dMax;
    bool m_bMaxInclusive;
  };

  static const CRangeType NoRange;
};

class Units {
public:
  class CUnitType {
  public:
    CUnitType(unsigned int uSIUnitName, unsigned int uFieldUnitName, double dFieldFactor)
        : m_uSIUnitName(uSIUnitName), m_uFieldUnitName(uFieldUnitName), m_dFieldFactor(dFieldFactor) {}

    CUnitType(const CUnitType &rhs)
        : m_uSIUnitName(rhs.m_uSIUnitName), m_uFieldUnitName(rhs.m_uFieldUnitName), m_dFieldFactor(rhs.m_dFieldFactor) {
    }

    unsigned int si_id() const { return m_uSIUnitName; }
    unsigned int field_id() const { return m_uFieldUnitName; }
    double field_factor() const { return m_dFieldFactor; }

  private:
    unsigned int m_uSIUnitName;
    unsigned int m_uFieldUnitName;
    double m_dFieldFactor;
  };

  static const CUnitType LengthUnit;
  static const CUnitType PressureUnit;
  static const CUnitType TimeUnit;
  static const CUnitType PerTimeUnit;
  static const CUnitType StressUnit;
  static const CUnitType AngleUnit;
  static const CUnitType DimensionLessUnit;
  static const CUnitType DensityUnit;
  static const CUnitType PerTemperUnit;
  static const CUnitType TemperatureUnit;
  static const CUnitType VelocityUnit;
  static const CUnitType PerStressUnit;
  static const CUnitType PerLengthUnit;
  static const CUnitType StressGradientUnit;
  static const CUnitType ThermalConductUnit;
  static const CUnitType ThermalCapaciUnit;
  static const CUnitType PermeaUnit;
  static const CUnitType ViscosityUnit;
};

template <unsigned int uComponentName,  // Resource string ID of the component name
          unsigned int uComponentIndex> // Index of the component in the value set
class CComponentTemp : public CValueComponent {
private:
  Units::CUnitType m_UnitType;
  Ranges::CRangeType m_RangeType;

public:
  CComponentTemp(CValueType &value_type, IValueSet &value_set, const Units::CUnitType &unittype,
                 const Ranges::CRangeType &rangetype)
      : CValueComponent(uComponentName, value_type, uComponentIndex, 0), m_UnitType(unittype), m_RangeType(rangetype) {
    assert(!IsLinkedTo(value_set));
    LinkTo(value_set);
    assert(&ValueSet() == &value_set);
  }

  CComponentTemp(CValueType &value_type, const Units::CUnitType &unittype, const Ranges::CRangeType &rangetype)
      : CValueComponent(uComponentName, value_type, uComponentIndex, 0), m_UnitType(unittype), m_RangeType(rangetype) {}

  CComponentTemp(const CComponentTemp &rhs)
      : CValueComponent(rhs), m_UnitType(rhs.m_UnitType), m_RangeType(rhs.m_RangeType) {}

  virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const {
    QString sRet;

    if (unit == CQuantity::SI_UNIT)
      sRet = getStringTableEntry(m_UnitType.si_id());
    else
      sRet = getStringTableEntry(m_UnitType.field_id());

    return sRet;
  }

  virtual double FieldFactor() const { return m_UnitType.field_factor(); }

  virtual QString ExportLabel() const {
    const CValueType &value_type = dynamic_cast<const CValueType &>(Parent());
    return value_type.ExportType();
  }

  virtual bool IsInRange(const IValueSet &value_set) const {
    // Check max range
    if (m_RangeType.hasMax()) {
      if (!value_set.Max().Valid())
        return false;
      if (m_RangeType.MaxInc()) {
        if (value_set.Max().Value() > m_RangeType.MaxVal())
          return false;
      } else {
        if (value_set.Max().Value() >= m_RangeType.MaxVal())
          return false;
      }
    }

    if (m_RangeType.hasMin()) {
      if (!value_set.Min().Valid())
        return false;
      if (m_RangeType.MinInc()) {
        if (value_set.Min().Value() < m_RangeType.MinVal())
          return false;
      } else {
        if (value_set.Min().Value() <= m_RangeType.MinVal())
          return false;
      }
    }

    return true;
  }

  virtual geo::CValue RangeMin(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const {
    if (m_RangeType.hasMin()) {
      if (unit == CQuantity::SI_UNIT)
        return geo::CValue(m_RangeType.MinVal() * 1.0);
      return geo::CValue(m_RangeType.MinVal() * m_UnitType.field_factor());
    }
    return geo::CValue();
  }

  virtual geo::CValue RangeMax(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const {
    if (m_RangeType.hasMax()) {
      if (unit == CQuantity::SI_UNIT)
        return geo::CValue(m_RangeType.MaxVal() * 1.0);
      return geo::CValue(m_RangeType.MaxVal() * m_UnitType.field_factor());
    }
    return geo::CValue();
  }
};

#endif // !defined(AFX_VALUECOMPONENT_H__6EB7525B_6874_4F83_8D60_59CD829C568D__INCLUDED_)
