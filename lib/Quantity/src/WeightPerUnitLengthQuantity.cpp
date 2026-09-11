// WeightPerUnitLengthQuantity.cpp: implementation of the CWeightPerUnitLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "WeightPerUnitLengthQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeightPerUnitLengthQuantity::~CWeightPerUnitLengthQuantity() {}

CWeightPerUnitLengthQuantity::CWeightPerUnitLengthQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CWeightPerUnitLengthQuantity::CWeightPerUnitLengthQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CWeightPerUnitLengthQuantity::CWeightPerUnitLengthQuantity(const double &dWPUL, const double &min, const double &max,
                                                           const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dWPUL, unit);
}

CWeightPerUnitLengthQuantity::CWeightPerUnitLengthQuantity(const double &value, const double &min, const double &max,
                                                           bool bIncludeMin, bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CWeightPerUnitLengthQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

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
    tmp = tmp / (0.67197 * 1e6);
    break;
  case US_UNIT:
    tmp = tmp / (0.67197 * 1e6);
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
    tmp = tmp * 0.67197 * 1e6;
    break;
  case US_UNIT:
    tmp = tmp * 0.67197 * 1e6;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CWeightPerUnitLengthQuantity::QuantityName() const { return "Weight per unit Length"; }

std::string CWeightPerUnitLengthQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "Mkg/m";
    break;
  case SI_USER_UNIT:
    return "kg/m";
    break;
  case FIELD_UNIT:
    return "lbm/ft";
    break;
  case US_UNIT:
    return "lbm/ft";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CWeightPerUnitLengthQuantity::CWeightPerUnitLengthQuantity(const CWeightPerUnitLengthQuantity &rhs)
    : CDoubleQuantity(rhs) {}
