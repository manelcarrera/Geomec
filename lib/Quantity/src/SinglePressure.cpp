// SinglePressure.cpp: implementation of the CSinglePressure class.
//
//////////////////////////////////////////////////////////////////////

#include "SinglePressure.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSinglePressure::CSinglePressure()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CSinglePressure::CSinglePressure(const double &value, const UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(value, unit);
}

CSinglePressure::CSinglePressure(const double& value,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}

CSinglePressure::CSinglePressure(const double& value,
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

//##ModelId=3BE7AEB802B6
double CSinglePressure::Convert(const double &value, const UNIT out, const UNIT in) const
{
  /*if((out == FIELD_UNIT) &&
     (in == SI_UNIT))
     return value / 0.006894757;

  if((out == SI_UNIT) &&
     (in == FIELD_UNIT))
     return value * 0.006894757;

  return value;*/

  if(in == out)
    return value;


  double tmp=value;

  switch(in)
  {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    break;
  case FIELD_UNIT:
    tmp=tmp / 145.0377;
    break;
  case US_UNIT:
    tmp=tmp / 145.0377;
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
    break;
  case FIELD_UNIT:
    tmp=tmp * 145.0377;
    break;
  case US_UNIT:
    tmp=tmp * 145.0377;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;


}


//##ModelId=3BE7AEB802BC
std::string CSinglePressure::QuantityName() const
{
  return "Pressure";
}

//##ModelId=3BE7AEB802C5
std::string CSinglePressure::UnitName(const UNIT unit) const
{
  switch(unit)
  {
  case SI_UNIT:
    return "MPa";
    break;
  case SI_USER_UNIT:
    return "MPa";
    break;
  case FIELD_UNIT:
    return "psi";
    break;
  case US_UNIT:
    return "psi";
    break;
  default:
    assert(false);	// Unit type not defined 
    break;
  };

  return "";
}

CSinglePressure::CSinglePressure(const CSinglePressure &rhs)
: CDoubleQuantity(rhs)
{

}
