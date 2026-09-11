// WeightPerUnitLengthQuantity.h: interface for the CWeightPerUnitLengthQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTPERUNITLENGTHQUANTITY_H__3D340308_783D_426E_A677_A531B1111503__INCLUDED_)
#define AFX_WEIGHTPERUNITLENGTHQUANTITY_H__3D340308_783D_426E_A677_A531B1111503__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CWeightPerUnitLengthQuantity : public CDoubleQuantity {
public:
  CWeightPerUnitLengthQuantity(const CWeightPerUnitLengthQuantity &rhs);
  CWeightPerUnitLengthQuantity();
  virtual ~CWeightPerUnitLengthQuantity();

  CWeightPerUnitLengthQuantity(const double &value, const UNIT unit = SI_UNIT);

  CWeightPerUnitLengthQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CWeightPerUnitLengthQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                               bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};
#endif // !defined(AFX_WEIGHTPERUNITLENGTHQUANTITY_H__3D340308_783D_426E_A677_A531B1111503__INCLUDED_)
