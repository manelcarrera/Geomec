// LengthQuantity.h: interface for the CLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LENGTHQUANTITY_H__60CAB61F_8F53_43F2_A6D9_566565982C0E__INCLUDED_)
#define AFX_LENGTHQUANTITY_H__60CAB61F_8F53_43F2_A6D9_566565982C0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CLengthQuantity : public CDoubleQuantity {
public:
  CLengthQuantity(const CDoubleQuantity &rhs);
  CLengthQuantity();
  CLengthQuantity(const double &value, CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT);

  CLengthQuantity(const double &value, const double &min, const double &max,
                  const CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT);

  CLengthQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                  bool bIncludeMax = true, const CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT);

  virtual double Convert(const double &value, const CDoubleQuantity::UNIT out, const CDoubleQuantity::UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT) const;
};

#endif // !defined(AFX_LENGTHQUANTITY_H__60CAB61F_8F53_43F2_A6D9_566565982C0E__INCLUDED_)
