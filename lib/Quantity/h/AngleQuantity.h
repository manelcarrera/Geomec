// AngleQuantity.h: interface for the CAngleQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANGLEQUANTITY_H__7131D306_5E86_48C5_AD6E_A284EBA45DDB__INCLUDED_)
#define AFX_ANGLEQUANTITY_H__7131D306_5E86_48C5_AD6E_A284EBA45DDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CAngleQuantity : public CDoubleQuantity
{
public:
	CAngleQuantity();

	CAngleQuantity(const double &value ,
		           const UNIT unit = SI_UNIT);
	
    CAngleQuantity(const double& value,
		           const double &min ,
				   const double &max,
				   const UNIT unit = SI_UNIT);

	CAngleQuantity(const double& value,
				   const double &min ,
				   const double &max,
				   bool bIncludeMin=true,
				   bool bIncludeMax=true,
				   const UNIT unit = SI_UNIT);

	CAngleQuantity(const CAngleQuantity& rhs);
	
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_ANGLEQUANTITY_H__7131D306_5E86_48C5_AD6E_A284EBA45DDB__INCLUDED_)
