// SmallLengthQuantity.cpp: implementation of the CSmallLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "SmallLengthQuantity.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmallLengthQuantity::CSmallLengthQuantity()
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
}

CSmallLengthQuantity::CSmallLengthQuantity(const double& dSmallLength, const UNIT unit)
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(dSmallLength, unit);
}

CSmallLengthQuantity::CSmallLengthQuantity(  const double& value,
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


double CSmallLengthQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{
	if(in == out)
		return value;


	double tmp=value;

	switch(in)
	{
	case SI_UNIT:
		break;
	case SI_USER_UNIT:
		tmp=tmp / 1000.0;
		break;
	case FIELD_UNIT:
		tmp=tmp / 39.3701;
		break;
	case US_UNIT:
		tmp=tmp / 39.3701;
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
		tmp=tmp * 1000.0;
		break;
	case FIELD_UNIT:
		tmp=tmp * 39.3701;
		break;
	case US_UNIT:
		tmp=tmp * 39.3701;
		break;
	default:
		assert(false);
		break;
	}

	return tmp;

}

std::string	CSmallLengthQuantity::QuantityName() const
{
	return "Length";
}

std::string CSmallLengthQuantity::UnitName(const UNIT unit) const 
{
	switch(unit)
	{
	case SI_UNIT:
		return "m";
		break;
	case SI_USER_UNIT:
		return "mm";
		break;
	case FIELD_UNIT:
		return "in";
		break;
	case US_UNIT:
		return "in";
		break;	
	default:
		assert(false);
		break;
	}

	return "";
}

CSmallLengthQuantity::CSmallLengthQuantity(const CSmallLengthQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
