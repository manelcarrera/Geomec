// AngleQuantity.h: interface for the CLossCoefQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOSSCOEFQUANTITY_H)
#define AFX_LOSSCOEFQUANTITY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CLossCoefQuantity : public CDoubleQuantity
{
public:
	CLossCoefQuantity();

	CLossCoefQuantity(const double &value ,
		           const UNIT unit = SI_UNIT);
	
    CLossCoefQuantity(const double& value,
		           const double &min ,
				   const double &max,
				   const UNIT unit = SI_UNIT);

	CLossCoefQuantity(const double& value,
				   const double &min ,
				   const double &max,
				   bool bIncludeMin=true,
				   bool bIncludeMax=true,
				   const UNIT unit = SI_UNIT);

	CLossCoefQuantity(const CLossCoefQuantity& rhs);
	
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_LOSSCOEFQUANTITY_H)
