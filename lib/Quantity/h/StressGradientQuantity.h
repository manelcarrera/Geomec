// StressGradientQuantity.h: interface for the CStressGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRESSGRADIENTQUANTITY_H__6B6C4FAC_D611_45F1_A742_7772E8AB1B83__INCLUDED_)
#define AFX_STRESSGRADIENTQUANTITY_H__6B6C4FAC_D611_45F1_A742_7772E8AB1B83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CStressGradientQuantity : public CDoubleQuantity
{
public:
	CStressGradientQuantity(const CStressGradientQuantity &rhs);
	CStressGradientQuantity();
	
	CStressGradientQuantity(const double &value , const UNIT unit = SI_UNIT);

    CStressGradientQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	CStressGradientQuantity(const double& value,
						    const double &min ,
						    const double &max,
			                bool bIncludeMin=true,
			                bool bIncludeMax=true,
			                const UNIT unit= SI_UNIT);

	
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_STRESSGRADIENTQUANTITY_H__6B6C4FAC_D611_45F1_A742_7772E8AB1B83__INCLUDED_)
