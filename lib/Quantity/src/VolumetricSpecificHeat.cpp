// VolumetricSpecificHeat.cpp: implementation of the CVolumetricSpecificHeatQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "VolumetricSpecificHeat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVolumetricSpecificHeatQuantity::~CVolumetricSpecificHeatQuantity() {}

CVolumetricSpecificHeatQuantity::CVolumetricSpecificHeatQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CVolumetricSpecificHeatQuantity::CVolumetricSpecificHeatQuantity(const double &value, const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CVolumetricSpecificHeatQuantity::CVolumetricSpecificHeatQuantity(const double &dTimeH, const double &min,
                                                                 const double &max, const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dTimeH, unit);
}

CVolumetricSpecificHeatQuantity::CVolumetricSpecificHeatQuantity(const double &value, const double &min,
                                                                 const double &max, bool bIncludeMin, bool bIncludeMax,
                                                                 const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CVolumetricSpecificHeatQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    tmp = tmp / 26.568;
    break;
  case US_UNIT:
    tmp = tmp / 14.76;
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
    tmp = tmp * 26.568;
    break;
  case US_UNIT:
    tmp = tmp * 14.76;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CVolumetricSpecificHeatQuantity::QuantityName() const { return "Volumetric specific heat"; }

std::string CVolumetricSpecificHeatQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "MJ/(m3.oC)";
    break;
  case SI_USER_UNIT:
    return "MJ/(m3.oC)";
    break;
  case FIELD_UNIT:
    return "Btu/(ft3.oC)";
    break;
  case US_UNIT:
    return "Btu/(ft3.oF)";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CVolumetricSpecificHeatQuantity::CVolumetricSpecificHeatQuantity(const CVolumetricSpecificHeatQuantity &rhs)
    : CDoubleQuantity(rhs) {}
