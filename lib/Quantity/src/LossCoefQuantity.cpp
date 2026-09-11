// AngleQuantity.cpp: implementation of the CLossCoefQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "LossCoefQuantity.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLossCoefQuantity::CLossCoefQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
}

CLossCoefQuantity::CLossCoefQuantity(const double &value, const UNIT unit)
{
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CLossCoefQuantity::CLossCoefQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
{

  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}

CLossCoefQuantity::CLossCoefQuantity(const CLossCoefQuantity& rhs)
: CDoubleQuantity(rhs)
{
}

CLossCoefQuantity::CLossCoefQuantity(const double& value,
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

double CLossCoefQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{

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
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp * pow(0.3048,3);
    break;
  case US_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp * pow(0.3048,3);
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
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp / pow(0.3048,3);
    break;
  case US_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp / pow(0.3048,3);
    break;
  default:
    assert(false);
    break;
  }

  return tmp;


}

std::string	CLossCoefQuantity::QuantityName() const
{
  return "Angle";
}

std::string CLossCoefQuantity::UnitName(const UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "m3*S-0.5";
    break;
  case SI_USER_UNIT:
    return "m3*S-0.5";
    break;
  case FIELD_UNIT:
    return "ft3*S-0.5";
    break;
  case US_UNIT:
    return "ft3*S-0.5";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}
