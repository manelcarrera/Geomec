// TimeHQuantity.cpp: implementation of the CTimeHQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "TimeHQuantity.h"

CTimeHQuantity::CTimeHQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CTimeHQuantity::CTimeHQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CTimeHQuantity::CTimeHQuantity(const double &dTimeH, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dTimeH, unit);
}

CTimeHQuantity::CTimeHQuantity(const double &value, const double &min, const double &max, bool bIncludeMin,
                               bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CTimeHQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp / 2.7778e-4;
    break;
  case FIELD_UNIT:
    tmp = tmp / 2.7778e-4;
    break;
  case US_UNIT:
    tmp = tmp / 2.7778e-4;
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp * 2.7778e-4;
    break;
  case FIELD_UNIT:
    tmp = tmp * 2.7778e-4;
    break;
  case US_UNIT:
    tmp = tmp * 2.7778e-4;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CTimeHQuantity::QuantityName() const { return "TimeInHours"; }

std::string CTimeHQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "s";
    break;
  case SI_USER_UNIT:
    return "hours";
    break;
  case FIELD_UNIT:
    return "hours";
    break;
  case US_UNIT:
    return "hours";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CTimeHQuantity::CTimeHQuantity(const CTimeHQuantity &rhs) : CDoubleQuantity(rhs) {}
