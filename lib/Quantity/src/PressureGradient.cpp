// PressureGradient.cpp: implementation of the CPressureGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "PressureGradient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ##ModelId=3BE7AEB80323

CPressureGradientQuantity::CPressureGradientQuantity() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CPressureGradientQuantity::CPressureGradientQuantity(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CPressureGradientQuantity::CPressureGradientQuantity(const double &value, const double &min, const double &max,
                                                     const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CPressureGradientQuantity::CPressureGradientQuantity(const double &value, const double &min, const double &max,
                                                     bool bIncludeMin, bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

// ##ModelId=3BE7AEB80334
std::string CPressureGradientQuantity::QuantityName() const { return "PressureGradient"; }

// ##ModelId=3BE7AEB80336
std::string CPressureGradientQuantity::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "MPa/m";
    break;
  case SI_USER_UNIT:
    return "kPa/m";
    break;
  case FIELD_UNIT:
    return "psi/ft";
    break;
  case US_UNIT:
    return "ppg";
    break;
  default:
    assert(false); // Unit type not defined
    break;
  };

  return "";
}

// ##ModelId=3BE7AEB80326
double CPressureGradientQuantity::Convert(const double &value, const UNIT out, const UNIT in) const {
  /*if((out == FIELD_UNIT) &&
     (in == SI_UNIT))
     return value / (0.006894757 / 0.3048);

  if((out == SI_UNIT) &&
     (in == FIELD_UNIT))
     return value * (0.006894757 / 0.3048);

  return value;*/

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp / 1e3;
    break;
  case FIELD_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp = tmp / (145.0377 * 0.3048);
    break;
  case US_UNIT:
    tmp = tmp / 8.4970e2;
    break;
  default:
    assert(false);
    break;
  }

  switch (out) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp = tmp * 1e3;
    break;
  case FIELD_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp = tmp * (145.0377 * 0.3048);
    break;
  case US_UNIT:
    tmp = tmp * 8.4970e2;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

CPressureGradientQuantity::CPressureGradientQuantity(const CPressureGradientQuantity &rhs) : CDoubleQuantity(rhs) {}

//////////////////////////////////////////////////////////////////

CPressureGradient2::CPressureGradient2() : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
}

CPressureGradient2::CPressureGradient2(const double &value, const UNIT unit) : CDoubleQuantity() {
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CPressureGradient2::CPressureGradient2(const double &value, const double &min, const double &max, const UNIT unit)
    : CDoubleQuantity() {
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

CPressureGradient2::CPressureGradient2(const double &value, const double &min, const double &max, bool bIncludeMin,
                                       bool bIncludeMax, const UNIT unit)
    : CDoubleQuantity() {
  m_bIncludeMin = bIncludeMin;
  m_bIncludeMax = bIncludeMax;
  MaxValue(max, unit);
  MinValue(min, unit);
  Value(value, unit);
}

std::string CPressureGradient2::QuantityName() const { return "PressureGradient"; }

std::string CPressureGradient2::UnitName(const UNIT unit) const {
  switch (unit) {
  case SI_UNIT:
    return "kPa/m";
    break;
  case SI_USER_UNIT:
    return "kPa/m";
    break;
  case FIELD_UNIT:
    return "psi/ft";
    break;
  case US_UNIT:
    return "ppg";
    break;
  default:
    assert(false); // Unit type not defined
    break;
  };

  return "";
}

double CPressureGradient2::Convert(const double &value, const UNIT out, const UNIT in) const {

  if (in == out)
    return value;

  double tmp = value;

  switch (in) {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp = tmp / (145.0377 * 0.3048);
    break;
  case US_UNIT:
    tmp = tmp / 8.4970e-1;
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
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp = tmp * (145.0377 * 0.3048);
    break;
  case US_UNIT:
    tmp = tmp * 8.4970e-1;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

CPressureGradient2::CPressureGradient2(const CPressureGradient2 &rhs) : CDoubleQuantity(rhs) {}
