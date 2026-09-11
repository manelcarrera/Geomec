// StrainQuantity.h: interface for the CStrainQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRAINQUANTITY_H__E6DDEE89_33FD_4B45_A84F_3E8591C06B7C__INCLUDED_)
#define AFX_STRAINQUANTITY_H__E6DDEE89_33FD_4B45_A84F_3E8591C06B7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CStrainQuantity : public CDoubleQuantity
{
public:
  CStrainQuantity(const CStrainQuantity &rhs);
  CStrainQuantity();

  CStrainQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CStrainQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CStrainQuantity(const double& value,
           const double &min ,
           const double &max,
           bool bIncludeMin=true,
           bool bIncludeMax=true,
           const UNIT unit= SI_UNIT);
  
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};


#endif // !defined(AFX_STRAINQUANTITY_H__E6DDEE89_33FD_4B45_A84F_3E8591C06B7C__INCLUDED_)
