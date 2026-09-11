#ifndef _MATERIALUNITTYPES_H_
#define _MATERIALUNITTYPES_H_

#include "mlMatParam.h"

class CUnitType : public ml::CMatParam::CUnitType
{
public:
  CUnitType();
  virtual ~CUnitType();
  virtual const char *UnitName(int nUnitDef) const;
  virtual const char *UnitNameSI() const = 0;
  virtual const char *UnitNameField() const = 0;
  virtual double ToUserUnit(double dValue, int nUnitDef) const;
  virtual double FromUserUnit(double dValue, int nUnitDef) const;
  virtual double ToFieldUnit(double dValue) const = 0;
  virtual double FromFieldUnit(double dValue) const = 0;
};

class CUnitTypeNoConv : public CUnitType
{
public:
  CUnitTypeNoConv();
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual const char *UnitName() const = 0;
};

class CUnitTypeNone : public CUnitTypeNoConv
{
public:
  CUnitTypeNone();
  //virtual CUnitTypeNone* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypeStress : public CUnitType
{
public:
  CUnitTypeStress();
  //virtual CUnitTypeStress* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeStressGradient : public CUnitType
{
public:
  CUnitTypeStressGradient();
  //virtual CUnitTypeStressGradient* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeDensity : public CUnitType
{
public:
  CUnitTypeDensity();
  //virtual CUnitTypeDensity* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeDepth : public CUnitType
{
public:
  CUnitTypeDepth();
  //virtual CUnitTypeDepth* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeForce : public CUnitTypeNoConv
{
public:
  CUnitTypeForce();
  //virtual CUnitTypeForce* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypeVelocity : public CUnitType
{
public:
  CUnitTypeVelocity();
  //virtual CUnitTypeVelocity* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeAngle : public CUnitTypeNoConv
{
public:
  CUnitTypeAngle();
  //virtual CUnitTypeAngle* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypePerStress : public CUnitType
{
public:
  CUnitTypePerStress();
  //virtual CUnitTypePerStress* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypePerTime : public CUnitTypeNoConv
{
public:
  CUnitTypePerTime();
  //virtual CUnitTypePerTime* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypeTemper : public CUnitTypeNoConv
{
public:
  CUnitTypeTemper();
  //virtual CUnitTypeTemper* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypeKelvinTemper : public CUnitTypeNoConv
{
public:
  CUnitTypeKelvinTemper();
  //virtual CUnitTypeKelvinTemper* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypePerTemper : public CUnitTypeNoConv
{
public:
  CUnitTypePerTemper();
  //virtual CUnitTypePerTemper* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypePerDistance : public CUnitType
{
public:
  CUnitTypePerDistance();
  //virtual CUnitTypePerDistance* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeThermConduct : public CUnitType
{
public:
  CUnitTypeThermConduct();
  //virtual CUnitTypeThermConduct* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypeThermCapacity : public CUnitType
{
public:
  CUnitTypeThermCapacity();
  //virtual CUnitTypeThermCapacity* Clone() const;
  virtual const char *UnitNameSI() const;
  virtual const char *UnitNameField() const;
  virtual double ToFieldUnit(double dValue) const;
  virtual double FromFieldUnit(double dValue) const;
};

class CUnitTypePermea : public CUnitTypeNoConv
{
public:
  CUnitTypePermea();
  //virtual CUnitTypePermea* Clone() const;
  virtual const char *UnitName() const;
};

class CUnitTypeViscosity : public CUnitTypeNoConv
{
public:
  CUnitTypeViscosity();
  //virtual CUnitTypeViscosity* Clone() const;
  virtual const char *UnitName() const;
};

#endif // _MATERIALUNITTYPES_H_
