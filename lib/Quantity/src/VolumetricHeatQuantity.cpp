// VolumetricHeatQuantity.cpp: implementation of the CVolumetricHeatQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "VolumetricHeatQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVolumetricHeatQuantity::~CVolumetricHeatQuantity()
{

}




CVolumetricHeatQuantity::CVolumetricHeatQuantity()
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  
}

CVolumetricHeatQuantity::CVolumetricHeatQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
  MaxValue(DOUBLE_MAX_,  SI_UNIT);
  MinValue(DOUBLE_MIN_,  SI_UNIT);
  Value(value, unit);
}

CVolumetricHeatQuantity::CVolumetricHeatQuantity(const double& dVolHeat,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

  CDoubleQuantity::MaxValue(max,  unit);
  CDoubleQuantity::MinValue(min,  unit);
  Value(dVolHeat, unit);
}

CVolumetricHeatQuantity::CVolumetricHeatQuantity(	const double& value,
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

double CVolumetricHeatQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
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
    tmp=tmp / 26.568;
    break;
  case US_UNIT:
    tmp=tmp / 26.568;
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
    tmp=tmp * 26.568;
    break;
  case US_UNIT:
    tmp=tmp * 26.568;
    break;
  default:
    assert(false);
    break;
  }

  return tmp;
}

std::string	CVolumetricHeatQuantity::QuantityName() const
{
  return "Volumetric heat";
}

std::string CVolumetricHeatQuantity::UnitName(const UNIT unit) const 
{
  switch(unit)
  {
  case SI_UNIT:
    return "MJ/m3";
    break;
  case SI_USER_UNIT:
    return "MJ/m3";
    break;
  case FIELD_UNIT:
    return "Btu/ft3";
    break;
  case US_UNIT:
    return "Btu/ft3";
    break;
  default:
    assert(false);
    break;
  }

  return "";
}



CVolumetricHeatQuantity::CVolumetricHeatQuantity(const CVolumetricHeatQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
