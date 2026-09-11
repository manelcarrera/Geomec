// LengthQuantity.cpp: implementation of the CLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "LengthQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLengthQuantity::CLengthQuantity()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
}

CLengthQuantity::CLengthQuantity(const double& dLength, CDoubleQuantity::UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(dLength, unit);
}


CLengthQuantity::CLengthQuantity(const double& value,const double &min ,const double &max, const CDoubleQuantity::UNIT unit)
: CDoubleQuantity()
{
  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}



CLengthQuantity::CLengthQuantity(  const double& value,
                             const double &min ,
                             const double &max,
                             bool bIncludeMin,
                             bool bIncludeMax,
                     const CDoubleQuantity::UNIT unit)
:CDoubleQuantity()
{
  m_bIncludeMin=bIncludeMin;
  m_bIncludeMax=bIncludeMax;
  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}



double CLengthQuantity::Convert(const double &value, const CDoubleQuantity::UNIT out, const CDoubleQuantity::UNIT in) const
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
    tmp=tmp * 0.3048;
    break;
  case US_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp * 0.3048;
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
    tmp=tmp / 0.3048;
    break;
  case US_UNIT:
    // Using 0.3048, see Mantis #2322: official factor (imperial ft)
    tmp=tmp / 0.3048;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;

}

std::string	CLengthQuantity::QuantityName() const
{
  return "Length";
}

std::string CLengthQuantity::UnitName(const CDoubleQuantity::UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "m";
    break;
  case SI_USER_UNIT:
    return "m";
    break;
  case FIELD_UNIT:
    return "ft";
    break;
  case US_UNIT:
    return "ft";
    break;	
  default:
    assert(false);
    break;
  }

  return "";
}

CLengthQuantity::CLengthQuantity(const CDoubleQuantity &rhs)
: CDoubleQuantity(rhs)
{
  // Construct
}

