// TimeHQuantity.h: interface for the CTimeHQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEHQUANTITY_H__7AFA8960_8EE0_4759_A79A_36F1ED999577__INCLUDED_)
#define AFX_TIMEHQUANTITY_H__7AFA8960_8EE0_4759_A79A_36F1ED999577__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CTimeHQuantity : public CDoubleQuantity {
public:
  CTimeHQuantity(const CTimeHQuantity &rhs);
  CTimeHQuantity();

  CTimeHQuantity(const double &value, const UNIT unit = SI_UNIT);

  CTimeHQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CTimeHQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                 bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_TIMEHQUANTITY_H__7AFA8960_8EE0_4759_A79A_36F1ED999577__INCLUDED_)
