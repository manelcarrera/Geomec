// SingleQuantityTemp.h: interface for the CSingleQuantityTemp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEQUANTITYTEMP_H__214FDA08_D8BB_429A_8B4B_F8D14369C45A__INCLUDED_)
#define AFX_SINGLEQUANTITYTEMP_H__214FDA08_D8BB_429A_8B4B_F8D14369C45A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <std::string NAME,       // Name of quantity
          double FIELD_FACTOR,    // Field unit factor
          double US_FACTOR,       // US unit factor
          std::string USER_NAME,  // Name of user unit
          std::string SI_NAME,    // Name of SI unit
          std::string FIELD_NAME, // Name of field unit
          std::string US_NAME>    // Name of US unit
class CSingleQuantityTemp : CDoubleQuantity {
public:
  CSingleQuantityTemp() : CDoubleQuantity() {
    MaxValue(DOUBLE_MAX_, SI_UNIT);
    MinValue(DOUBLE_MIN_, SI_UNIT);
  }

  CSingleQuantityTemp(const double &value, const UNIT unit) : CDoubleQuantity() {
    MaxValue(DOUBLE_MAX_, SI_UNIT);
    MinValue(DOUBLE_MIN_, SI_UNIT);
    Value(value, unit);
  }

  CSingleQuantityTemp(const double &value, const double &min, const double &max, const UNIT unit) : CDoubleQuantity() {
    MaxValue(max, unit);
    MinValue(min, unit);
    Value(value, unit);
  }

  CSingleQuantityTemp(const CCohesionQuantity &rhs) : CDoubleQuantity(rhs) {}

  CSingleQuantityTemp(const double &value, const double &min, const double &max, bool bIncludeMin, bool bIncludeMax,
                      const UNIT unit)
      : CDoubleQuantity() {
    m_bIncludeMin = bIncludeMin;
    m_bIncludeMax = bIncludeMax;
    MaxValue(max, unit);
    MinValue(min, unit);
    Value(value, unit);
  }

  double Convert(const double &value, const UNIT out, const UNIT in) const {

    if (in == out)
      return value;

    double tmp = value;

    switch (in) {
    case SI_UNIT:
      break;
    case SI_USER_UNIT:
      break;
    case FIELD_UNIT:
      tmp = tmp / FIELD_FACTOR;
      break;
    case US_UNIT:
      tmp = tmp / US_FACTOR;
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
      tmp = tmp * FIELD_FACTOR;
      break;
    case US_UNIT:
      tmp = tmp * US_FACTOR;
      break;
    default:
      assert(false);
      break;
    }

    return tmp;
  }

  std::string QuantityName() const { return NAME; }

  std::string UnitName(const UNIT unit) const {
    switch (unit) {
    case SI_UNIT:
      return SI_NAME;
      break;
    case SI_USER_UNIT:
      return USER_NAME;
      break;
    case FIELD_UNIT:
      return FIELD_NAME;
      break;
    case US_UNIT:
      return US_NAME;
      break;
    default:
      assert(false); // Unit type not defined
      break;
    };

    return "";
  }
};

#endif // !defined(AFX_SINGLEQUANTITYTEMP_H__214FDA08_D8BB_429A_8B4B_F8D14369C45A__INCLUDED_)
