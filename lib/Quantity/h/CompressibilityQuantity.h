// CompressibilityQuantity.h: interface for the CCompressibilityQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPRESSIBILITYQUANTITY_H__013C9CCB_05B5_4087_8F55_D52771304746__INCLUDED_)
#define AFX_COMPRESSIBILITYQUANTITY_H__013C9CCB_05B5_4087_8F55_D52771304746__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CCompressibilityQuantity : public CDoubleQuantity
{
public:
	CCompressibilityQuantity();

	CCompressibilityQuantity(const double &value, const UNIT unit = SI_UNIT);
	
    CCompressibilityQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

	
	CCompressibilityQuantity(const double& value,
							 const double &min ,
							 const double &max,
							 bool bIncludeMin=true,
							 bool bIncludeMax=true,
							 const UNIT unit= SI_UNIT);

	CCompressibilityQuantity(const CCompressibilityQuantity& rhs);
	
	virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
	virtual std::string	QuantityName() const;
	virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_COMPRESSIBILITYQUANTITY_H__013C9CCB_05B5_4087_8F55_D52771304746__INCLUDED_)
