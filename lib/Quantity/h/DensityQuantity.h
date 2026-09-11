// DensityQuantity.h: interface for the CDensityQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DENSITYQUANTITY_H__836A46CB_C72A_45BB_A54F_138F840ECC68__INCLUDED_)
#define AFX_DENSITYQUANTITY_H__836A46CB_C72A_45BB_A54F_138F840ECC68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CDensityQuantity : public CDoubleQuantity
{
public:
	CDensityQuantity();

	CDensityQuantity(const double &value , const UNIT unit = SI_UNIT);
	
    CDensityQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	CDensityQuantity(const double& value,
					 const double &min ,
					 const double &max,
					 bool bIncludeMin=true,
					 bool bIncludeMax=true,
					 const UNIT unit= SI_UNIT);

	CDensityQuantity(const CDensityQuantity& rhs);
	
	
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_DENSITYQUANTITY_H__836A46CB_C72A_45BB_A54F_138F840ECC68__INCLUDED_)
