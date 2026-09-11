// CrackOpeningQuantity.h: interface for the CCrackOpeningQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRACKOPENINGQUANTITY_H__09511E18_45CB_4B9A_B560_41452A7A7F55__INCLUDED_)
#define AFX_CRACKOPENINGQUANTITY_H__09511E18_45CB_4B9A_B560_41452A7A7F55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CCrackOpeningQuantity : public CDoubleQuantity
{
public:
  CCrackOpeningQuantity();

  CCrackOpeningQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CCrackOpeningQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CCrackOpeningQuantity(const double& value,
           const double &min ,
           const double &max,
           bool bIncludeMin=true,
           bool bIncludeMax=true,
           const UNIT unit= SI_UNIT);

  CCrackOpeningQuantity(const CCrackOpeningQuantity& rhs);
  
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_CRACKOPENINGQUANTITY_H__09511E18_45CB_4B9A_B560_41452A7A7F55__INCLUDED_)


  
