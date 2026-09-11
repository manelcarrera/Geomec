#ifndef _GAMMAAXIALVIEW_H_
#define _GAMMAAXIALVIEW_H_

#include "GammaView.h"

class CGammaAxialView : public CGammaView {
protected:
  CGammaAxialView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CGammaAxialView)

public:
  virtual double GetStressOrStrain(CStressStrain const &StressStrain) const;
  virtual double GetStrain(CStressStrain const &StressStrain) const;
  virtual double GetStress(CStressStrain const &StressStrain) const;

protected:
  virtual ~CGammaAxialView();
};

#endif
