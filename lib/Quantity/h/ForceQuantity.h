// ForceQuantity.h: interface for the CForceQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORCEQUANTITY_H__26901591_8E45_4369_9E0C_D2AEE39487F9__INCLUDED_)
#define AFX_FORCEQUANTITY_H__26901591_8E45_4369_9E0C_D2AEE39487F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CForceQuantity : public CDoubleQuantity {
public:
  CForceQuantity();

  CForceQuantity(const double &value, const UNIT unit = SI_UNIT);

  CForceQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CForceQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                 bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  CForceQuantity(const CForceQuantity &rhs);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_FORCEQUANTITY_H__26901591_8E45_4369_9E0C_D2AEE39487F9__INCLUDED_)
