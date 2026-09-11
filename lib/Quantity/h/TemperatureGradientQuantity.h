// TemperatureGradientQuantity.h: interface for the CTemperatureGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPERATUREGRADIENTQUANTITY_H__D21EC5B0_31EA_4893_8A1E_62E09228DF28__INCLUDED_)
#define AFX_TEMPERATUREGRADIENTQUANTITY_H__D21EC5B0_31EA_4893_8A1E_62E09228DF28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CTemperatureGradientQuantity : public CDoubleQuantity
{
public:
	CTemperatureGradientQuantity(const CTemperatureGradientQuantity& rhs);
	CTemperatureGradientQuantity();
	
	CTemperatureGradientQuantity(const double &value , const UNIT unit = SI_UNIT);

	CTemperatureGradientQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);
	

	CTemperatureGradientQuantity(const double& value,
						         const double &min ,
						         const double &max,
			                     bool bIncludeMin=true,
			                     bool bIncludeMax=true,
			                     const UNIT unit= SI_UNIT);

	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};


#endif // !defined(AFX_TEMPERATUREGRADIENTQUANTITY_H__D21EC5B0_31EA_4893_8A1E_62E09228DF28__INCLUDED_)
