// PercentageQuantity.h: interface for the CPercentageQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERCENTAGEQUANTITY_H__49E042D9_EB09_4B0D_BEF4_CDB2E44E2352__INCLUDED_)
#define AFX_PERCENTAGEQUANTITY_H__49E042D9_EB09_4B0D_BEF4_CDB2E44E2352__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CPercentageQuantity : public CDoubleQuantity
{
public:
  CPercentageQuantity(const CPercentageQuantity & rhs);
  CPercentageQuantity();

  CPercentageQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CPercentageQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);
  

  CPercentageQuantity(const double& value,
            const double &min ,
            const double &max,
            bool bIncludeMin=true,
            bool bIncludeMax=true,
            const UNIT unit= SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};


#endif // !defined(AFX_PERCENTAGEQUANTITY_H__49E042D9_EB09_4B0D_BEF4_CDB2E44E2352__INCLUDED_)
