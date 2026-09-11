// StressGradientQuantity.cpp: implementation of the CStressGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "StressGradientQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStressGradientQuantity::CStressGradientQuantity()
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CStressGradientQuantity::CStressGradientQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(value, unit);
}

CStressGradientQuantity::CStressGradientQuantity(const double& dStressGradient,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

	CDoubleQuantity::MaxValue(max,  unit);
	CDoubleQuantity::MinValue(min,  unit);
	Value(dStressGradient, unit);
}

CStressGradientQuantity::CStressGradientQuantity(const double& value,
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


double CStressGradientQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
{

	if(in == out)
		return value;


	double tmp=value;

	switch(in)
	{
	case SI_UNIT:
		break;
	case SI_USER_UNIT:
		tmp=tmp / 1e3;
		break;
	case FIELD_UNIT:
		tmp=tmp / 8.4970e2;
		break;
	case US_UNIT:
		tmp=tmp / 4.4210e1;
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
		tmp=tmp * 1e3;
		break;
	case FIELD_UNIT:
		tmp=tmp * 8.4970e2;
		break;
	case US_UNIT:
		tmp=tmp * 4.4210e1;
		break;
	default:
		assert(false);
		break;
	}

	return tmp;
}

std::string	CStressGradientQuantity::QuantityName() const
{
	return "StressGradient";
}

std::string CStressGradientQuantity::UnitName(const UNIT unit) const 
{
	switch(unit)
	{
	case SI_UNIT:
		return "MPa/m";
		break;
	case SI_USER_UNIT:
		return "kPa/m";
		break;
	case FIELD_UNIT:
		return "psi/ft";
		break;
	case US_UNIT:
		return "psi/ft";
		break;
	default:
		assert(false);
		break;
	}

	return "";
}

CStressGradientQuantity::CStressGradientQuantity(const CStressGradientQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
