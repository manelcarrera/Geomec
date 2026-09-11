// TemperatureQuantity.h: interface for the CTemperatureQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPERATUREQUANTITY_H__4B5824EA_891A_4263_A414_958DA1DB42FD__INCLUDED_)
#define AFX_TEMPERATUREQUANTITY_H__4B5824EA_891A_4263_A414_958DA1DB42FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CTemperatureQuantity : public CDoubleQuantity {
public:
  CTemperatureQuantity(const CTemperatureQuantity &rhs);
  CTemperatureQuantity();

  CTemperatureQuantity(const double &value, const UNIT unit = SI_UNIT);

  CTemperatureQuantity(const double &value, const double &min, const double &max, const UNIT unit = SI_UNIT);

  CTemperatureQuantity(const double &value, const double &min, const double &max, bool bIncludeMin = true,
                       bool bIncludeMax = true, const UNIT unit = SI_UNIT);

  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string QuantityName() const;
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;
};

#endif // !defined(AFX_TEMPERATUREQUANTITY_H__4B5824EA_891A_4263_A414_958DA1DB42FD__INCLUDED_)
