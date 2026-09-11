// Acceleration.h: interface for the CAcceleration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCELERATION_H__0F6D50B5_C1E5_4255_BF91_597F0D7E1784__INCLUDED_)
#define AFX_ACCELERATION_H__0F6D50B5_C1E5_4255_BF91_597F0D7E1784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

//##ModelId=3C036365006F
class CAcceleration : public CDoubleQuantity 
{
public:
  //##ModelId=3C036365009E
  CAcceleration(const double &min = 5.0,
          const double &max = 15.0,
          const UNIT unit = SI_UNIT);
  //##ModelId=3C03636500A2
  CAcceleration(const double &value,
          const double &min,
          const double &max,
          const UNIT unit = SI_UNIT);

  CAcceleration(const CAcceleration & rhs);

  //##ModelId=3C03636500B0
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  //##ModelId=3C03636500C0
  virtual std::string QuantityName() const;
  //##ModelId=3C03636500C2
  virtual std::string UnitName(const UNIT unit = SI_UNIT) const;

};

#endif // !defined(AFX_ACCELERATION_H__0F6D50B5_C1E5_4255_BF91_597F0D7E1784__INCLUDED_)
