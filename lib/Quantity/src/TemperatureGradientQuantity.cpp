// TemperatureGradientQuantity.cpp: implementation of the CTemperatureGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "TemperatureGradientQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTemperatureGradientQuantity::CTemperatureGradientQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CTemperatureGradientQuantity::CTemperatureGradientQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CTemperatureGradientQuantity::CTemperatureGradientQuantity(const double &dTemperatureGradient, const double &min,
                                                           const double &max, const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dTemperatureGradient, unit);
}

CTemperatureGradientQuantity::CTemperatureGradientQuantity(const double &value, const double &min, const double &max,
                                                           bool bIncludeMin, bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CTemperatureGradientQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    tmp = tmp / 0.3048; // wjrx mantis 3461
    break;
  case US_UNIT:
    tmp = tmp / 0.5486; // wjrx mantis 3461
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    tmp = tmp * 0.3048; // wjrx mantis 3461
    break;
  case US_UNIT:
    tmp = tmp * 0.5486; // wjrx mantis 3461
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CTemperatureGradientQuantity::QuantityName() const { return "TemperatureGradient"; }

std::string CTemperatureGradientQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "C/m";
    break;
  case SI_USER_UNIT:
    return "C/m";
    break;
  case FIELD_UNIT:
    return "C/ft"; // wjrx mantis 3461
    break;
  case US_UNIT:
    return "F/ft"; // wjrx mantis 3461
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CTemperatureGradientQuantity::CTemperatureGradientQuantity(const CTemperatureGradientQuantity &rhs)
    : CDoubleQuantity(rhs) {}
