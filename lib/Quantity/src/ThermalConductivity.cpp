// ThermalConductivity.cpp: implementation of the CThermalConductivityQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "ThermalConductivity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CThermalConductivityQuantity::~CThermalConductivityQuantity()
{

}

CThermalConductivityQuantity::CThermalConductivityQuantity()
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	
}

CThermalConductivityQuantity::CThermalConductivityQuantity(const double &value, const UNIT unit)
: CDoubleQuantity()
{
	MaxValue(DOUBLE_MAX_,  SI_UNIT);
	MinValue(DOUBLE_MIN_,  SI_UNIT);
	Value(value, unit);
}

CThermalConductivityQuantity::CThermalConductivityQuantity(const double& dTimeH,const double &min ,const double &max, const UNIT unit)
:CDoubleQuantity()
{

	CDoubleQuantity::MaxValue(max,  unit);
	CDoubleQuantity::MinValue(min,  unit);
	Value(dTimeH, unit);
}

CThermalConductivityQuantity::CThermalConductivityQuantity(	const double& value,
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

double CThermalConductivityQuantity::Convert(const double &value, const UNIT out, const UNIT in) const
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
		tmp=tmp / 1.041e6;
		break;
	case US_UNIT:
		tmp=tmp / 0.5782e6;
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
		tmp=tmp * 1.041e6;
		break;
	case US_UNIT:
		tmp=tmp * 0.5782e6;
		break;
	default:
		assert(false);
		break;
	}

	return tmp;
}

std::string	CThermalConductivityQuantity::QuantityName() const
{
	return "Thermal conductivity";
}

std::string CThermalConductivityQuantity::UnitName(const UNIT unit) const 
{
	switch(unit)
	{
	case SI_UNIT:
		return "MW/(m.oC)";
		break;
	case SI_USER_UNIT:
		return "W/(m.oC)";
		break;
	case FIELD_UNIT:
		return "Btu/(hr.ft3.oC)";
		break;
	case US_UNIT:
		return "Btu/(hr.ft.oF)";
		break;
	default:
		assert(false);
		break;
	}

	return "";
}



CThermalConductivityQuantity::CThermalConductivityQuantity(const CThermalConductivityQuantity &rhs)
: CDoubleQuantity(rhs)
{

}
