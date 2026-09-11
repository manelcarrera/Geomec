#ifndef _GAMMARADIALVIEW_H_
#define _GAMMARADIALVIEW_H_

#include "GammaView.h"

class CGammaRadialView : public CGammaView
{
protected:
  CGammaRadialView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CGammaRadialView)

public:
  virtual double GetStressOrStrain( CStressStrain const &StressStrain ) const;
  virtual double GetStrain( CStressStrain const &StressStrain ) const;
  virtual double GetStress( CStressStrain const &StressStrain ) const;

protected:
  virtual ~CGammaRadialView();
};
#endif
