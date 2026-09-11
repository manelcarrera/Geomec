// NoneQuantity.cpp: implementation of the CNoneQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "NoneQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNoneQuantity::CNoneQuantity()
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CNoneQuantity::CNoneQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(value, unit);
}

CNoneQuantity::CNoneQuantity(const double& value,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

	MaxValue(max,  unit);
	MinValue(min,  unit);
	Value(value, unit);
}

CNoneQuantity::CNoneQuantity(      const double& value,
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


double CNoneQuantity::Convert(const double &value, const UNIT /*out*/, const UNIT /*in*/) const
{

	//if(in == out)
		return value;


	/*double tmp=value;

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
	case SI_USER_UNIT:
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

std::string	CNoneQuantity::QuantityName() const
{
	return "None";
}

std::string CNoneQuantity::UnitName(const UNIT unit) const 
{
	switch(unit)
	{
	case SI_UNIT:
		return "-";
		break;
	case SI_USER_UNIT:
		return "-";
		break;
	case FIELD_UNIT:
		return "-";
		break;
	case US_UNIT:
		return "-";
		break;
	default:
		assert(false);
		break;
	}

	return "";
}

CNoneQuantity::CNoneQuantity(const CNoneQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
