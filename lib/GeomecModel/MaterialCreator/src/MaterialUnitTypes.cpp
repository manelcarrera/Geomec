#include "MaterialUnitTypes.h"
#include "Materials.h"

#include <cassert>

CUnitType::CUnitType()
{
}

CUnitType::~CUnitType()
{
}

const char *CUnitType::UnitName(int nUnitDef) const
{
  switch(nUnitDef)
  {
  case MLUD_SI:
    return UnitNameSI();
  case MLUD_FIELD:
    return UnitNameField();
  }

  assert(false);
  return 0;
}

double CUnitType::ToUserUnit(double dValue, int nUnitDef) const
{
  if(nUnitDef == MLUD_FIELD)
    return ToFieldUnit(dValue);

  assert(nUnitDef == MLUD_SI);
  return dValue;
}

double CUnitType::FromUserUnit(double dValue, int nUnitDef) const
{
  if(nUnitDef == MLUD_FIELD)
    return FromFieldUnit(dValue);

  assert(nUnitDef == MLUD_SI);
  return dValue;
}

// CUnitTypeNoConv
CUnitTypeNoConv::CUnitTypeNoConv()
{
}

double CUnitTypeNoConv::ToFieldUnit(double dValue) const
{
  return dValue;
}

double CUnitTypeNoConv::FromFieldUnit(double dValue) const
{
  return dValue;
}

const char *CUnitTypeNoConv::UnitNameSI() const
{
  return UnitName();
}

const char *CUnitTypeNoConv::UnitNameField() const
{
  return UnitName();
}


// CUnitTypeNone
CUnitTypeNone::CUnitTypeNone()
{
}

/*
CUnitTypeNone* CUnitTypeNone::Clone() const
{
  return new CUnitTypeNone;
}
*/

const char *CUnitTypeNone::UnitName() const
{
  return "-";
}


// CUnitTypeStress
CUnitTypeStress::CUnitTypeStress()
{
}

/*
CUnitTypeStress* CUnitTypeStress::Clone() const
{
  return new CUnitTypeStress;
}
*/

const char *CUnitTypeStress::UnitNameSI() const
{
    return "MPa";
}

const char *CUnitTypeStress::UnitNameField() const
{
    return "psi";
}

double CUnitTypeStress::ToFieldUnit(double dValue) const
{
	return dValue / 0.006894757;
}

double CUnitTypeStress::FromFieldUnit(double dValue) const
{
	return dValue * 0.006894757;
}


// CUnitTypeStressGradient
CUnitTypeStressGradient::CUnitTypeStressGradient()
{
}

/*
CUnitTypeStressGradient* CUnitTypeStressGradient::Clone() const
{
  return new CUnitTypeStressGradient;
}
*/

const char *CUnitTypeStressGradient::UnitNameSI() const
{
  return "MPa/m";
}

const char *CUnitTypeStressGradient::UnitNameField() const
{
  return "psi/ft";
}

double CUnitTypeStressGradient::ToFieldUnit(double dValue) const
{
  return 0.3048 / 0.006894757 * dValue;
}

double CUnitTypeStressGradient::FromFieldUnit(double dValue) const
{
  return 0.006894757 / 0.3048  * dValue;
}


// CUnitTypeDensity
CUnitTypeDensity::CUnitTypeDensity()
{
}

/*
CUnitTypeDensity* CUnitTypeDensity::Clone() const
{
  return new CUnitTypeDensity;
}
*/

const char *CUnitTypeDensity::UnitNameSI() const
{
  return "kg/m3";
}

const char *CUnitTypeDensity::UnitNameField() const
{
  return "lb/gal";
}

double CUnitTypeDensity::ToFieldUnit(double dValue) const
{
  return dValue / 119.82648378907;
}

double CUnitTypeDensity::FromFieldUnit(double dValue) const
{
  return dValue * 119.82648378907;
}


// CUnitTypeDepth
CUnitTypeDepth::CUnitTypeDepth()
{
}

/*
CUnitTypeDepth* CUnitTypeDepth::Clone() const
{
  return new CUnitTypeDepth;
}
*/

const char *CUnitTypeDepth::UnitNameSI() const
{
  return "m";
}

const char *CUnitTypeDepth::UnitNameField() const
{
  return "ft";
}

double CUnitTypeDepth::ToFieldUnit(double dValue) const
{
  return dValue / 0.3048;
}

double CUnitTypeDepth::FromFieldUnit(double dValue) const
{
  return dValue * 0.3048;
}


// CUnitTypeForce
CUnitTypeForce::CUnitTypeForce()
{
}

/*
CUnitTypeForce* CUnitTypeForce::Clone() const
{
  return new CUnitTypeForce;
}
*/

const char *CUnitTypeForce::UnitName() const
{
  return "MN";
}


// CUnitTypeVelocity
CUnitTypeVelocity::CUnitTypeVelocity()
{
}

/*
CUnitTypeVelocity* CUnitTypeVelocity::Clone() const
{
  return new CUnitTypeVelocity;
}
*/

const char *CUnitTypeVelocity::UnitNameSI() const
{
  return "m/s";
}

const char *CUnitTypeVelocity::UnitNameField() const
{
  return "ft/s";
}

double CUnitTypeVelocity::ToFieldUnit(double dValue) const
{
  return dValue / 0.3048;
}

double CUnitTypeVelocity::FromFieldUnit(double dValue) const
{
  return dValue * 0.3048;
}


// CUnitTypeAngle
CUnitTypeAngle::CUnitTypeAngle()
{
}

/*
CUnitTypeAngle* CUnitTypeAngle::Clone() const
{
  return new CUnitTypeAngle;
}
*/

const char *CUnitTypeAngle::UnitName() const
{
  return "deg";
}


// CUnitTypePerStress
CUnitTypePerStress::CUnitTypePerStress()
{
}

/*
CUnitTypePerStress* CUnitTypePerStress::Clone() const
{
  return new CUnitTypePerStress;
}
*/

const char *CUnitTypePerStress::UnitNameSI() const
{
  return "1/MPa";
}

const char *CUnitTypePerStress::UnitNameField() const
{
  return "1/psi";
}

double CUnitTypePerStress::ToFieldUnit(double dValue) const
{
  return dValue * 0.006894757;
}

double CUnitTypePerStress::FromFieldUnit(double dValue) const
{
  return dValue / 0.006894757;
}


// CUnitTypePerTime
CUnitTypePerTime::CUnitTypePerTime()
{
}

/*
CUnitTypePerTime* CUnitTypePerTime::Clone() const
{
  return new CUnitTypePerTime;
}
*/

const char *CUnitTypePerTime::UnitName() const
{
  return "1/s";
}


// CUnitTypeTemper
CUnitTypeTemper::CUnitTypeTemper()
{
}

/*
CUnitTypeTemper* CUnitTypeTemper::Clone() const
{
  return new CUnitTypeTemper;
}
*/

const char *CUnitTypeTemper::UnitName() const
{
  return "C";
}


// CUnitTypeKelvinTemper
CUnitTypeKelvinTemper::CUnitTypeKelvinTemper()
{
}

/*
CUnitTypeKelvinTemper* CUnitTypeKelvinTemper::Clone() const
{
  return new CUnitTypeKelvinTemper;
}
*/

const char *CUnitTypeKelvinTemper::UnitName() const
{
  return "K";
}


// CUnitTypePerTemper
CUnitTypePerTemper::CUnitTypePerTemper()
{
}

/*
CUnitTypePerTemper* CUnitTypePerTemper::Clone() const
{
  return new CUnitTypePerTemper;
}
*/

const char *CUnitTypePerTemper::UnitName() const
{
  return "1/C";
}


// CUnitTypePerDistance
CUnitTypePerDistance::CUnitTypePerDistance()
{
}

/*
CUnitTypePerDistance* CUnitTypePerDistance::Clone() const
{
  return new CUnitTypePerDistance;
}
*/

const char *CUnitTypePerDistance::UnitNameSI() const
{
  return "1/m";
}

const char *CUnitTypePerDistance::UnitNameField() const
{
  return "1/ft";
}

double CUnitTypePerDistance::ToFieldUnit(double dValue) const
{
  return dValue * 0.3048;
}

double CUnitTypePerDistance::FromFieldUnit(double dValue) const
{
  return dValue / 0.3048;
}


// CUnitTypeThermConduct
CUnitTypeThermConduct::CUnitTypeThermConduct()
{
}

/*
CUnitTypeThermConduct* CUnitTypeThermConduct::Clone() const
{
  return new CUnitTypeThermConduct;
}
*/

const char *CUnitTypeThermConduct::UnitNameSI() const
{
  return "W/(m.K)";
}

const char *CUnitTypeThermConduct::UnitNameField() const
{
  return "W/(ft.K)";
}

double CUnitTypeThermConduct::ToFieldUnit(double dValue) const
{
  return dValue * 0.3048;
}

double CUnitTypeThermConduct::FromFieldUnit(double dValue) const
{
  return dValue / 0.3048;
}


// CUnitTypeThermCapacity
CUnitTypeThermCapacity::CUnitTypeThermCapacity()
{
}

/*
CUnitTypeThermCapacity* CUnitTypeThermCapacity::Clone() const
{
  return new CUnitTypeThermCapacity;
}
*/

const char *CUnitTypeThermCapacity::UnitNameSI() const
{
  return "J/(kg.K)";
}

const char *CUnitTypeThermCapacity::UnitNameField() const
{
  return "J/(lb.K)";
}

double CUnitTypeThermCapacity::ToFieldUnit(double dValue) const
{
  return dValue * 0.45359237;
}

double CUnitTypeThermCapacity::FromFieldUnit(double dValue) const
{
  return dValue / 0.45359237;
}


// CUnitTypePermea
CUnitTypePermea::CUnitTypePermea()
{
}

/*
CUnitTypePermea* CUnitTypePermea::Clone() const
{
  return new CUnitTypePermea;
}
*/

const char *CUnitTypePermea::UnitName() const
{
  return "mD"; // millidarcy = 1e-15 m2
}


// CUnitTypeViscosity
CUnitTypeViscosity::CUnitTypeViscosity()
{
}

/*
CUnitTypeViscosity* CUnitTypeViscosity::Clone() const
{
  return new CUnitTypeViscosity;
}
*/

const char *CUnitTypeViscosity::UnitName() const
{
  return "cP"; // centipoise = 1e-3 Pa.s
}
