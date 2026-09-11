// TemperatureQuantity.cpp: implementation of the CTemperatureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "TemperatureQuantity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTemperatureQuantity::CTemperatureQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);

}

CTemperatureQuantity::CTemperatureQuantity(const double &value, const UNIT unit)
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(value, unit);
}

CTemperatureQuantity::CTemperatureQuantity(const double& dTemperature,const double &min ,const double &max, const UNIT unit)
{

	CDoubleQuantity::MaxValue(max,  unit);
	CDoubleQuantity::MinValue(min,  unit);
	Value(dTemperature, unit);
}

CTemperatureQuantity::CTemperatureQuantity(const double& value,
			                               const double &min ,
			                               const double &max,
			                               bool bIncludeMin,
			                               bool bIncludeMax,
							               const UNIT unit)
{
	m_bIncludeMin=bIncludeMin;
	m_bIncludeMax=bIncludeMax;
	MaxValue(max,  unit);
	MinValue(min,  unit);
	Value(value, unit);
}



double CTemperatureQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
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

	return tmp;
}

std::string	CTemperatureQuantity::QuantityName() const
{
	return "Temperature";
}

std::string CTemperatureQuantity::UnitName(const UNIT unit) const 
{
	switch(unit)
	{
	case SI_UNIT:
		return "C";
		break;
	case SI_USER_UNIT:
		return "C";
		break;
	case FIELD_UNIT:
		return "C";
		break;
	case US_UNIT:
		return "F";
		break;
	default:
		assert(false);
		break;
	}

	return "";
}



	

CTemperatureQuantity::CTemperatureQuantity(const CTemperatureQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
