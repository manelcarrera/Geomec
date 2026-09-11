// DoubleSingleQuantity.h: interface for the CDoubleSingleQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOUBLESINGLEQUANTITY_H__62E98308_F62E_408E_90C5_771E3B8FB86F__INCLUDED_)
#define AFX_DOUBLESINGLEQUANTITY_H__62E98308_F62E_408E_90C5_771E3B8FB86F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

//#define DOUBLE_MIN_ -1e20
//#define DOUBLE_MAX_ 1e20

class CDoubleSingleQuantity : public CSingleQuantity<double>
{
public:
  CDoubleSingleQuantity();

  virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
  virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;	
};

#endif // !defined(AFX_DOUBLESINGLEQUANTITY_H__62E98308_F62E_408E_90C5_771E3B8FB86F__INCLUDED_)
