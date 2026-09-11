// GigaSinglePressureQuantity.cpp: implementation of the CGigaSinglePressureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "GigaSinglePressureQuantity.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGigaSinglePressureQuantity::CGigaSinglePressureQuantity()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CGigaSinglePressureQuantity::CGigaSinglePressureQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(value, unit);
}

CGigaSinglePressureQuantity::CGigaSinglePressureQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}

CGigaSinglePressureQuantity::CGigaSinglePressureQuantity(const double& value,
                           const double &min ,
                           const double &max,
                           bool bIncludeMin,
                           bool bIncludeMax,
                   const UNIT unit)
:CDoubleQuantity()
{
  m_bIncludeMin=bIncludeMin;
  m_bIncludeMax=bIncludeMax;
  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}


double CGigaSinglePressureQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{
  if(in == out)
    return value;


  double tmp=value;

  switch(in)
  {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp=tmp / 1e-3;
    break;
  case FIELD_UNIT:
    tmp=tmp / 1.451e-4;
    break;
  case US_UNIT:
    tmp=tmp / 1.451e-4;
    break;
  default:
    assert(false);
    break;
  }


  switch(out)
  {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp=tmp * 1e-3;
    break;
  case FIELD_UNIT:
    tmp=tmp * 1.451e-4;
    break;
  case US_UNIT:
    tmp=tmp * 1.451e-4;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;


}


std::string CGigaSinglePressureQuantity::QuantityName() const
{
  return "Giga Pressure";
}


std::string CGigaSinglePressureQuantity::UnitName(const UNIT unit) const
{
  switch(unit)
  {
  case SI_UNIT:
    return "MPa";
    break;
  case SI_USER_UNIT:
    return "GPa";
    break;
  case FIELD_UNIT:
    return "Mpsi";
    break;
  case US_UNIT:
    return "Mpsi";
    break;
  default:
    assert(false);	// Unit type not defined 
    break;
  };

  return "";
}

CGigaSinglePressureQuantity::CGigaSinglePressureQuantity(const CGigaSinglePressureQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
