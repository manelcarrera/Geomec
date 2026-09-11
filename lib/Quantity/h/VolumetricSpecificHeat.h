// VolumetricSpecificHeat.h: interface for the CVolumetricSpecificHeatQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUMETRICSPECIFICHEAT_H__2A4A4988_EB01_464B_9FC5_4BD6D5B8641D__INCLUDED_)
#define AFX_VOLUMETRICSPECIFICHEAT_H__2A4A4988_EB01_464B_9FC5_4BD6D5B8641D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SingleQuantity.h"

class CVolumetricSpecificHeatQuantity : public CDoubleQuantity   
{
public:
  CVolumetricSpecificHeatQuantity(const CVolumetricSpecificHeatQuantity &rhs);
  CVolumetricSpecificHeatQuantity();
  virtual ~CVolumetricSpecificHeatQuantity();

  CVolumetricSpecificHeatQuantity(const double &value , const UNIT unit = SI_UNIT);
  
  CVolumetricSpecificHeatQuantity(const double& value,
              const double &min,
              const double &max,
              const UNIT unit = SI_UNIT);

  CVolumetricSpecificHeatQuantity(const double& value,
              const double &min ,
              const double &max,
              bool bIncludeMin=true,
              bool bIncludeMax=true,
              const UNIT unit= SI_UNIT);
  
  virtual double Convert(const double &value, const UNIT out, const UNIT in) const;
  virtual std::string	QuantityName() const;
  virtual	std::string UnitName(const UNIT unit = SI_UNIT) const;	
};

#endif // !defined(AFX_VOLUMETRICSPECIFICHEAT_H__2A4A4988_EB01_464B_9FC5_4BD6D5B8641D__INCLUDED_)
