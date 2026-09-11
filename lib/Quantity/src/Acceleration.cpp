// Acceleration.cpp: implementation of the CAcceleration class.
//
//////////////////////////////////////////////////////////////////////

#include "Acceleration.h"
#include "LengthQuantity.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ##ModelId=3C03636500A2
CAcceleration::CAcceleration(const double &value, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {
  assert((unit == SI_UNIT) || (unit == FIELD_UNIT));
  MaxValue(max, SI_UNIT);
  MinValue(min, SI_UNIT);
  Value(value, unit);
}

// ##ModelId=3C036365009E
CAcceleration::CAcceleration(const double &min, const double &max, const UNIT unit) : CDoubleQuantity() {
  assert((unit == SI_UNIT) || (unit == FIELD_UNIT));
  MaxValue(max, SI_UNIT);
  MinValue(min, SI_UNIT);
}

CAcceleration::CAcceleration(const CAcceleration &rhs) : CDoubleQuantity(rhs) {}

// ##ModelId=3C03636500B0
double CAcceleration::Convert(const double &value, const UNIT out, const UNIT in) const {
  CLengthQuantity accel;
  return accel.Convert(value, out, in);
}

// ##ModelId=3C03636500C0
std::string CAcceleration::QuantityName() const { return "Acceleration"; }

// ##ModelId=3C03636500C2
std::string CAcceleration::UnitName(const UNIT unit) const {
  if (unit == FIELD_UNIT)
    return "ft/s2";

  return "m/s2";
}
