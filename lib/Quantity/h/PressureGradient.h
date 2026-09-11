// PressureGradient.h: interface for the CPressureGradientQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSUREGRADIENT_H__6F178623_C832_445E_B8E8_982BC38AFB91__INCLUDED_)
#define AFX_PRESSUREGRADIENT_H__6F178623_C832_445E_B8E8_982BC38AFB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CPressureGradientQuantity : public CDoubleQuantity
{
public:
	CPressureGradientQuantity(const CPressureGradientQuantity &rhs);
	//##ModelId=3BE7AEB80323
	CPressureGradientQuantity();

	CPressureGradientQuantity(const double &value, const UNIT unit = SI_UNIT);

	CPressureGradientQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	CPressureGradientQuantity(  const double& value,
						const double &min ,
						const double &max,
			            bool bIncludeMin=true,
			            bool bIncludeMax=true,
			            const UNIT unit= SI_UNIT);


	//##ModelId=3BE7AEB80326
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	//##ModelId=3BE7AEB80334
	virtual std::string QuantityName() const;
	//##ModelId=3BE7AEB80336
	virtual std::string UnitName(const UNIT unit = SI_UNIT) const;

};

class CPressureGradient2 : public CDoubleQuantity
{
public:
	CPressureGradient2(const CPressureGradient2 &rhs);

	CPressureGradient2();

	CPressureGradient2(const double &value, const UNIT unit = SI_UNIT);

	CPressureGradient2(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	CPressureGradient2(  const double& value,
						const double &min ,
						const double &max,
			            bool bIncludeMin=true,
			            bool bIncludeMax=true,
			            const UNIT unit= SI_UNIT);


	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;

	virtual std::string QuantityName() const;

	virtual std::string UnitName(const UNIT unit = SI_UNIT) const;

};

#endif // !defined(AFX_PRESSUREGRADIENT_H__6F178623_C832_445E_B8E8_982BC38AFB91__INCLUDED_)
