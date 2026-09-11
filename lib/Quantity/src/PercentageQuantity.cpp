// PercentageQuantity.cpp: implementation of the CPercentageQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "PercentageQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPercentageQuantity::CPercentageQuantity()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CPercentageQuantity::CPercentageQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(value, unit);
}

CPercentageQuantity::CPercentageQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

  CDoubleQuantity::MaxValue(max,  unit);
  CDoubleQuantity::MinValue(min,  unit);
  Value(value, unit);
}

CPercentageQuantity::CPercentageQuantity(const double& value,
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



double CPercentageQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{

  if(in == out)
    return value;

  double tmp=value;

  switch(in)
  {
  case SI_UNIT:
    break;
  case SI_USER_UNIT:
    tmp=tmp / 100.0;
    break;
  case FIELD_UNIT:
    tmp=tmp / 100.0;
    break;
  case US_UNIT:
    tmp=tmp / 100.0;
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
    tmp=tmp * 100.0;
    break;
  case FIELD_UNIT:
    tmp=tmp * 100.0;
    break;
  case US_UNIT:
    tmp=tmp * 100.0;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string	CPercentageQuantity::QuantityName() const
{
  return "Percentage";
}

std::string CPercentageQuantity::UnitName(const UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "-";
    break;
  case SI_USER_UNIT:
    return "%";
    break;
  case FIELD_UNIT:
    return "%";
    break;
  case US_UNIT:
    return "%";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}


CPercentageQuantity::CPercentageQuantity(const CPercentageQuantity &rhs)
: CDoubleQuantity (rhs)
{

}
