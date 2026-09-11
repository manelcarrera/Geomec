// DensityQuantity.cpp: implementation of the CDensityQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "DensityQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDensityQuantity::CDensityQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CDensityQuantity::CDensityQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CDensityQuantity::CDensityQuantity(const double &value, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CDensityQuantity::CDensityQuantity(const double &value, const double &min, const double &max, bool bIncludeMin,
                                   bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CDensityQuantity::CDensityQuantity(const CDensityQuantity &rhs) : CDoubleQuantity(rhs) {}

double CDensityQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp / 1e6;
    break;
  case FIELD_UNIT:
    tmp = tmp / 6.2428e4;
    break;
  case US_UNIT:
    tmp = tmp / 8.3454e3;
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp * 1e6;
    break;
  case FIELD_UNIT:
    tmp = tmp * 6.2428e4;
    break;
  case US_UNIT:
    tmp = tmp * 8.3454e3;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CDensityQuantity::QuantityName() const { return "Density"; }

std::string CDensityQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "Mkg/m3";
    break;
  case SI_USER_UNIT:
    return "kg/m3";
    break;
  case FIELD_UNIT:
    return "lb/ft3";
    break;
  case US_UNIT:
    return "lb/gal";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}
