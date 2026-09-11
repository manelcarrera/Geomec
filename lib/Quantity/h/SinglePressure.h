// SinglePressure.h: interface for the CSinglePressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEPRESSURE_H__53979E91_A68D_4A32_B5C2_AA398E62F660__INCLUDED_)
#define AFX_SINGLEPRESSURE_H__53979E91_A68D_4A32_B5C2_AA398E62F660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CSinglePressure : public CDoubleQuantity
{
public:
	CSinglePressure(const CSinglePressure &rhs);
	CSinglePressure();

	//##ModelId=3BE7AEB802A6
	CSinglePressure(const double &value , const UNIT unit = SI_UNIT);

	CSinglePressure(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	CSinglePressure(    const double& value,
						const double &min ,
						const double &max,
			            bool bIncludeMin=true,
			            bool bIncludeMax=true,
			            const UNIT unit= SI_UNIT);


	//##ModelId=3BE7AEB802B6
	double Convert(const double &value, const UNIT out, const UNIT in) const;
	// Quantity interface ...
	//##ModelId=3BE7AEB802BC
	virtual std::string QuantityName() const;
	//##ModelId=3BE7AEB802C5
	virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};


#endif // !defined(AFX_SINGLEPRESSURE_H__53979E91_A68D_4A32_B5C2_AA398E62F660__INCLUDED_)
