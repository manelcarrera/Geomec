// AngleQuantity.cpp: implementation of the CAngleQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "AngleQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAngleQuantity::CAngleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
}

CAngleQuantity::CAngleQuantity(const double &value, const UNIT unit)
{
  MaxValue(DOUBLE_MAX_, SI_UNIT);
  MinValue(DOUBLE_MIN_, SI_UNIT);
  Value(value, unit);
}

CAngleQuantity::CAngleQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
{

  MaxValue(max,  unit);
  MinValue(min,  unit);
  Value(value, unit);
}

CAngleQuantity::CAngleQuantity(const CAngleQuantity& rhs)
: CDoubleQuantity(rhs)
{
}

CAngleQuantity::CAngleQuantity(const double& value,
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

double CAngleQuantity::Convert(const double &value, const UNIT /*out*/, const UNIT /*in*/) const
{

  //if(in == out)
    return value;

/*
  double tmp=value;

  switch(in)
  {
  case SI_UNIT:
    break;
  case FIELD_UNIT:
    break;
  case US_UNIT:
    tmp=(tmp-32)/1.8;
    break;
  default:
    assert(false);
    break;
  }


  switch(out)
  {
  case SI_UNIT:
    break;
  case FIELD_UNIT:
    break;
  case US_UNIT:
    tmp=(tmp*1.8)+32;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;*/
}

std::string	CAngleQuantity::QuantityName() const
{
  return "Angle";
}

std::string CAngleQuantity::UnitName(const UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "degrees";
    break;
  case SI_USER_UNIT:
    return "degrees";
    break;
  case FIELD_UNIT:
    return "degrees";
    break;
  case US_UNIT:
    return "degrees";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}
