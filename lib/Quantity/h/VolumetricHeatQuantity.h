// VolumetricHeatQuantity.h: interface for the CVolumetricHeatQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMETRICHEATQUANTITY_H__502E221B_2CC2_4E04_97C9_39BA776A99A0__INCLUDED_)
#define AFX_VOLUMETRICHEATQUANTITY_H__502E221B_2CC2_4E04_97C9_39BA776A99A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CVolumetricHeatQuantity : public CDoubleQuantity   
{
public:
  CVolumetricHeatQuantity(const CVolumetricHeatQuantity &rhs);
  CVolumetricHeatQuantity();
  virtual ~CVolumetricHeatQuantity();

  CVolumetricHeatQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CVolumetricHeatQuantity(const double& value,
              const double &min,
              const double &max,
              const UNIT unit = SI_UNIT);

  CVolumetricHeatQuantity(const double& value,
              const double &min ,
              const double &max,
              bool bIncludeMin=true,
              bool bIncludeMax=true,
              const UNIT unit= SI_UNIT);
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};

#endif // !defined(AFX_VOLUMETRICHEATQUANTITY_H__502E221B_2CC2_4E04_97C9_39BA776A99A0__INCLUDED_)
