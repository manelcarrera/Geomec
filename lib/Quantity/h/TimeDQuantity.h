// TimeDQuantity.h: interface for the CTimeDQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEDQUANTITY_H__87D9DA03_2569_4759_BF04_E793703E2B88__INCLUDED_)
#define AFX_TIMEDQUANTITY_H__87D9DA03_2569_4759_BF04_E793703E2B88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CTimeDQuantity : public CDoubleQuantity
{
public:
  CTimeDQuantity(const CTimeDQuantity &rhs);
  CTimeDQuantity();
  
  CTimeDQuantity(const double &value , const UNIT unit = SI_UNIT);

  CTimeDQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CTimeDQuantity(const double& value,
           const double &min ,
           const double &max,
             bool bIncludeMin=true,
             bool bIncludeMax=true,
             const UNIT unit= SI_UNIT);
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};



#endif // !defined(AFX_TIMEDQUANTITY_H__87D9DA03_2569_4759_BF04_E793703E2B88__INCLUDED_)
