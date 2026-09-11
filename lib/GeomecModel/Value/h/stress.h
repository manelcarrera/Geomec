// stress.h: interface for the stress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRESS_H__11DE1320_2F11_417D_A403_A8DEC9797C36__INCLUDED_)
#define AFX_STRESS_H__11DE1320_2F11_417D_A403_A8DEC9797C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PressureGradient.h"
#include "SinglePressure.h"

class CStressGradient : public CPressureGradientQuantity {
public:
  CStressGradient(const double &value, const UNIT unit = SI_UNIT);
  unsigned int QuantityNameId() const;
};

class CSingleStress : public CSinglePressure {
public:
  CSingleStress(const double &value, const UNIT unit = SI_UNIT);
  unsigned int QuantityNameId() const;
};

#endif // !defined(AFX_STRESS_H__11DE1320_2F11_417D_A403_A8DEC9797C36__INCLUDED_)
