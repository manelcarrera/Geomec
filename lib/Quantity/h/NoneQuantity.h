// NoneQuantity.h: interface for the CNoneQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NONEQUANTITY_H__41939966_0F91_46AA_AE25_1778D40707BA__INCLUDED_)
#define AFX_NONEQUANTITY_H__41939966_0F91_46AA_AE25_1778D40707BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CNoneQuantity : public CDoubleQuantity
{
public:
  CNoneQuantity(const CNoneQuantity& rhs);
  CNoneQuantity();
  
  CNoneQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CNoneQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CNoneQuantity( const double& value,
           const double &min ,
           const double &max,
           bool bIncludeMin=true,
           bool bIncludeMax=true,
           const UNIT unit= SI_UNIT);
  
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_NONEQUANTITY_H__41939966_0F91_46AA_AE25_1778D40707BA__INCLUDED_)
