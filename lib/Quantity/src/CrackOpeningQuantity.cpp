// CrackOpeningQuantity.cpp: implementation of the CCrackOpeningQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "CrackOpeningQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrackOpeningQuantity::CCrackOpeningQuantity()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CCrackOpeningQuantity::CCrackOpeningQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(value, unit);
}

CCrackOpeningQuantity::CCrackOpeningQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{
  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}

CCrackOpeningQuantity::CCrackOpeningQuantity(const CCrackOpeningQuantity& rhs)
: CDoubleQuantity(rhs)
{
}

CCrackOpeningQuantity::CCrackOpeningQuantity(const double& value,
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


double CCrackOpeningQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{

  if(in == out)
    return value;


  double tmp=value;

  switch(in)
  {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp=tmp / 1e6;
    break;
  case FIELD_UNIT:
    tmp=tmp / 1e6;
    break;
  case US_UNIT:
    tmp=tmp / 39370.07874;
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
    tmp=tmp * 1e6;
    break;
  case FIELD_UNIT:
    tmp=tmp * 1e6;
    break;
  case US_UNIT:
    tmp=tmp * 39370.07874;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string	CCrackOpeningQuantity::QuantityName() const
{
  return "Crack Opening";
}

std::string CCrackOpeningQuantity::UnitName(const UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "m";
    break;
  case SI_USER_UNIT:
    return "micron";
    break;
  case FIELD_UNIT:
    return "micron";
    break;
  case US_UNIT:
    return "mil";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}



  