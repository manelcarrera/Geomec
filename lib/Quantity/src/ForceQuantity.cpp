// ForceQuantity.cpp: implementation of the CForceQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "ForceQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CForceQuantity::CForceQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CForceQuantity::CForceQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CForceQuantity::CForceQuantity(const double &value, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CForceQuantity::CForceQuantity(const double &value, const double &min, const double &max, bool bIncludeMin,
                               bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CForceQuantity::CForceQuantity(const CForceQuantity &rhs) : CDoubleQuantity(rhs) {}

double CForceQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp / 10e3;
    break;
  case FIELD_UNIT:
    tmp = tmp / (10e6 / 4.448);
    break;
  case US_UNIT:
    tmp = tmp / (10e6 / 4.448);
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp * 10e3;
    break;
  case FIELD_UNIT:
    tmp = tmp * (10e6 / 4.448);
    break;
  case US_UNIT:
    tmp = tmp * (10e6 / 4.448);
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CForceQuantity::QuantityName() const { return "Force"; }

std::string CForceQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "MN";
    break;
  case SI_USER_UNIT:
    return "kN";
    break;
  case FIELD_UNIT:
    return "lbf";
    break;
  case US_UNIT:
    return "lbf";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}
