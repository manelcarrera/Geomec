// ThermalExpansion.h: interface for the CThermalExpansionQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THERMALEXPANSION_H__B31560C6_1504_4D09_9333_A1AF9512A757__INCLUDED_)
#define AFX_THERMALEXPANSION_H__B31560C6_1504_4D09_9333_A1AF9512A757__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CThermalExpansionQuantity : public CDoubleQuantity  
{
public:
  CThermalExpansionQuantity(const CThermalExpansionQuantity &rhs);
  CThermalExpansionQuantity();
  virtual ~CThermalExpansionQuantity();

  CThermalExpansionQuantity(const double &value, const UNIT unit = SI_UNIT);

  CThermalExpansionQuantity(const double& value,const double &min ,const double &max, const UNIT unit = SI_UNIT);

  CThermalExpansionQuantity(	const double& value,
            const double &min ,
            const double &max,
            bool bIncludeMin=true,
            bool bIncludeMax=true,
            const UNIT unit= SI_UNIT);

  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	

};

#endif // !defined(AFX_THERMALEXPANSION_H__B31560C6_1504_4D09_9333_A1AF9512A757__INCLUDED_)
