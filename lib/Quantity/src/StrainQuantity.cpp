// StrainQuantity.cpp: implementation of the CStrainQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "StrainQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrainQuantity::CStrainQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CStrainQuantity::CStrainQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CStrainQuantity::CStrainQuantity(const double &value, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CStrainQuantity::CStrainQuantity(const double &value, const double &min, const double &max, bool bIncludeMin,
                                 bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CStrainQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

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
    tmp = tmp / 1e3;
    break;
  case US_UNIT:
    tmp = tmp / 1e2;
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
    tmp = tmp * 1e3;
    break;
  case US_UNIT:
    tmp = tmp * 1e2;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CStrainQuantity::QuantityName() const { return "Strain"; }

std::string CStrainQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "-";
    break;
  case SI_USER_UNIT:
    return "microstrain";
    break;
  case FIELD_UNIT:
    return "per mil";
    break;
  case US_UNIT:
    return "%";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CStrainQuantity::CStrainQuantity(const CStrainQuantity &rhs) : CDoubleQuantity(rhs) {}
