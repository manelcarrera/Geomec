// GigaSinglePressureQuantity.h: interface for the CGigaSinglePressureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GIGASINGLEPRESSUREQUANTITY_H__D6105C9E_1EDE_42B2_9567_C70102B1A5D8__INCLUDED_)
#define AFX_GIGASINGLEPRESSUREQUANTITY_H__D6105C9E_1EDE_42B2_9567_C70102B1A5D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CGigaSinglePressureQuantity : public CDoubleQuantity
{
public:
  CGigaSinglePressureQuantity( const CGigaSinglePressureQuantity & rhs);
  CGigaSinglePressureQuantity();

  CGigaSinglePressureQuantity(const double &value , const UNIT unit = SI_UNIT);

  CGigaSinglePressureQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CGigaSinglePressureQuantity(    const double& value,
            const double &min ,
            const double &max,
            bool bIncludeMin=true,
            bool bIncludeMax=true,
            const UNIT unit= SI_UNIT);


  double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit) const;
};

#endif // !defined(AFX_GIGASINGLEPRESSUREQUANTITY_H__D6105C9E_1EDE_42B2_9567_C70102B1A5D8__INCLUDED_)
