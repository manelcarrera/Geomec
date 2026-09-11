// TimeDQuantity.cpp: implementation of the CTimeDQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "TimeDQuantity.h"

CTimeDQuantity::CTimeDQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CTimeDQuantity::CTimeDQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CTimeDQuantity::CTimeDQuantity(const double &dTimeD, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dTimeD, unit);
}

CTimeDQuantity::CTimeDQuantity(const double &value, const double &min, const double &max, bool bIncludeMin,
                               bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CTimeDQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp / 1.1574e-5;
    break;
  case FIELD_UNIT:
    tmp = tmp / 1.1574e-5;
    break;
  case US_UNIT:
    tmp = tmp / 1.1574e-5;
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp * 1.1574e-5;
    break;
  case FIELD_UNIT:
    tmp = tmp * 1.1574e-5;
    break;
  case US_UNIT:
    tmp = tmp * 1.1574e-5;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CTimeDQuantity::QuantityName() const { return "TimeInDays"; }

std::string CTimeDQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "s";
    break;
  case SI_USER_UNIT:
    return "days";
    break;
  case FIELD_UNIT:
    return "days";
    break;
  case US_UNIT:
    return "days";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CTimeDQuantity::CTimeDQuantity(const CTimeDQuantity &rhs) : CDoubleQuantity(rhs) {}
