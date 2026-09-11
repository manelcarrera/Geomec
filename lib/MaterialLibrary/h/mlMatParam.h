#ifndef _MLMATPARAM_H_
#define _MLMATPARAM_H_

#include <QtCore/QString>

#ifdef TEST_MATERIALS
namespace test_materials
{
  class AccessParam;
}
#endif


namespace ml {

class CMaterial;

class CMatParam
{
public:
  // base class for unit conversions
  // nUnitDef = 0 is used for the internally used unit (no conversion needed)
  class CUnitType
  {
  public:
    CUnitType();
    virtual ~CUnitType();
    //virtual CUnitType* Clone() const = 0;
    virtual const char *UnitName(int nUnitDef) const = 0;
    virtual double ToUserUnit(double dValue, int nUnitDef) const = 0;
    virtual double FromUserUnit(double dValue, int nUnitDef) const = 0;
  };

  class CCheckStrategy
  {
  public:
    CCheckStrategy() {}
    virtual ~CCheckStrategy() {}
    //virtual CCheckStrategy* Clone() const = 0;
    virtual bool operator()(double dValue, const CMatParam& param, QString& strErrorMsg, int nUnitDef = 0) const = 0;
    virtual bool IsDomainCheck() const { return false; }
  };

  class CSetStrategy
  {
  public:
    CSetStrategy() {}
    virtual ~CSetStrategy() {}
    //virtual CSetStrategy* Clone() const = 0;
    virtual void operator()(double dValue, CMatParam& param) const = 0;

  protected:
    void SetParamValue(CMatParam& param, double dValue) const { param.InternalSetValue(dValue); }
  };

  friend class CSetStrategy;

public:
  /*!
   * ownership of the check and set strategies (if any) and the unit type
   * is transferred to the CMatParam object
   */
  CMatParam(const QString& strName,
            double dValue,
            CMaterial& parentMat,
            const CUnitType& unitConversion,
            const CCheckStrategy* pCheckStrategy = 0,
            const CSetStrategy* pSetStrategy = 0);

  CMatParam(CMaterial& parentMat);
  virtual ~CMatParam();

  virtual CMatParam* Clone(CMaterial& parentMat) const;

  CMatParam& operator=(const CMatParam& rhs);
  bool operator==(const CMatParam& rhs) const;
  bool operator!=(const CMatParam& rhs) const;

  virtual const QString& Name() const;
  const char *UnitName(int nUnitDef) const;

  virtual double Value() const;
  virtual double ValueToUserUnit(int nUnitDef) const;
  virtual bool Value(double dValue);
  virtual bool Value(double dValue, QString& strErrorMsg);
  virtual bool ValueFromUserUnit(double dValue, int nUnitDef);
  virtual bool ValueFromUserUnit(double dValue, int nUnitDef, QString& strErrorMsg);

  // ForceValue always sets the value and triggers dependencies, even if the value remains the same
  virtual bool ForceValue(double dValue, QString& strErrorMsg);

  virtual double PreviousValue() const;

  virtual bool CheckDomainValue(double dValue, QString& strErrorMsg) const;
  virtual bool CheckValue(double dValue) const;
  virtual bool CheckValue(double dValue, QString& strErrorMsg) const;
  virtual bool CheckValueFromUserUnit(double dValue, int nUnitDef) const;
  virtual bool CheckValueFromUserUnit(double dValue, int nUnitDef, QString& strErrorMsg) const;

  const CMaterial& ParentMaterial() const;
  CMaterial& ParentMaterial();

  const CUnitType& UnitConversion() const;

  // for loading interface only (no checking and no dependencies)
  void LoadValue(double dValue);

protected:
  void InternalSetValue(double dValue);
  CMatParam(const CMatParam& rhs, CMaterial& parentMat);

private:
  QString m_strName;
  double m_dValue;
  double m_dPreviousValue;
  CMaterial& m_parentMat;
  const CUnitType* m_pUnitType;
#ifdef TEST_MATERIALS
protected:
  friend class test_materials::AccessParam;
#endif
  const CCheckStrategy* m_pCheckStrategy;
  const CSetStrategy* m_pSetStrategy;
};

} // namespace ml

#endif // _MLMATPARAM_H_
