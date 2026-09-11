// SmallLengthQuantity.h: interface for the CSmallLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMALLLENGTHQUANTITY_H__E35CF865_EBCA_4270_B111_054157DB9270__INCLUDED_)
#define AFX_SMALLLENGTHQUANTITY_H__E35CF865_EBCA_4270_B111_054157DB9270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CSmallLengthQuantity : public CDoubleQuantity {
public:
  CSmallLengthQuantity(const CSmallLengthQuantity &rhs);
  CSmallLengthQuantity();
  CSmallLengthQuantity(const double &value, const UNIT unit = SI_UNIT);

  CSmallLengthQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CSmallLengthQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                       bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_SMALLLENGTHQUANTITY_H__E35CF865_EBCA_4270_B111_054157DB9270__INCLUDED_)
