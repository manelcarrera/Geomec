// CurvatureQuantity.cpp: implementation of the CCurvatureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "CurvatureQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurvatureQuantity::CCurvatureQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CCurvatureQuantity::CCurvatureQuantity(const double value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CCurvatureQuantity::~CCurvatureQuantity() {}

std::string CCurvatureQuantity::QuantityName() const { return "Curvature"; }

std::string CCurvatureQuantity::UnitName(const UNIT unit /*=SI_UNIT*/) const {
  switch (unit) {
  case SI_UNIT:
    return "rad/m";
  case FIELD_UNIT:
    return "deg/100ft";
  case SI_USER_UNIT:
    return "rad/m";
  case US_UNIT:
    return "deg/100ft";
  default:
    assert(false);
  }

  return "";
}

double CCurvatureQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {
  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    tmp = tmp / 1746.3966;
    break;
  case US_UNIT:
    tmp = tmp / 1746.3966;
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
    tmp = tmp * 1746.3966;
    break;
  case US_UNIT:
    tmp = tmp * 1746.3966;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}
