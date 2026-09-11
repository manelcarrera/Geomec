// CurvatureQuantity.h: interface for the CCurvatureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVATUREQUANTITY_H__AC20D804_6F18_4D70_823C_1279CA6B7D52__INCLUDED_)
#define AFX_CURVATUREQUANTITY_H__AC20D804_6F18_4D70_823C_1279CA6B7D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"


class CCurvatureQuantity : public CDoubleQuantity
{
public:
	CCurvatureQuantity();
	CCurvatureQuantity(const double value,const UNIT unit);

	virtual ~CCurvatureQuantity();

	// Quantity description ...
	virtual std::string	QuantityName() const;
	// Unit description ...
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;
	// Value discription ...
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;

};

#endif // !defined(AFX_CURVATUREQUANTITY_H__AC20D804_6F18_4D70_823C_1279CA6B7D52__INCLUDED_)
