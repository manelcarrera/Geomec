// ThermalExpansion.cpp: implementation of the CThermalExpansionQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "ThermalExpansion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThermalExpansionQuantity::~CThermalExpansionQuantity() {}

CThermalExpansionQuantity::CThermalExpansionQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CThermalExpansionQuantity::CThermalExpansionQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CThermalExpansionQuantity::CThermalExpansionQuantity(const double &dTimeH, const double &min, const double &max,
                                                     const UNIT unit)
    : CDoubleQuantity() {

  CDoubleQuantity::MaxValue(max, unit);
  CDoubleQuantity::MinValue(min, unit);
  Value(dTimeH, unit);
}

CThermalExpansionQuantity::CThermalExpansionQuantity(const double &value, const double &min, const double &max,
                                                     bool bIncludeMin, bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

double CThermalExpansionQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    break;
  case US_UNIT:
    tmp = tmp / 0.5556;
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
    break;
  case US_UNIT:
    tmp = tmp * 0.5556;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string CThermalExpansionQuantity::QuantityName() const { return "Thermal expansion"; }

std::string CThermalExpansionQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "1/oC";
    break;
  case SI_USER_UNIT:
    return "1/oC";
    break;
  case FIELD_UNIT:
    return "1/oC";
    break;
  case US_UNIT:
    return "1/oF";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}

CThermalExpansionQuantity::CThermalExpansionQuantity(const CThermalExpansionQuantity &rhs) : CDoubleQuantity(rhs) {}
