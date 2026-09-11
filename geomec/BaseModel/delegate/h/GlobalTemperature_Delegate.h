#ifndef _GlobalTemperature_Delegate_h_
#define _GlobalTemperature_Delegate_h_

class TTemperature;
class CTemperatureGradientComponent;
class CGlobalTemperature;

#include "GlobalFormationLoad_Delegate.h"

class CGlobalTemperature_Delegate : public CGlobalFormationLoad_Delegate<TTemperature, CTemperatureGradientComponent> {
public:
  CGlobalTemperature_Delegate(CGlobalTemperature *globalTemperature);

  virtual bool Attributes();

private:
  CGlobalTemperature_Delegate(const CGlobalTemperature_Delegate &rhs);
  CGlobalTemperature_Delegate &operator=(const CGlobalTemperature_Delegate &rhs);

  CGlobalTemperature *m_globalTemperature;

  REGISTER_DELEGATE(CGlobalTemperature, CGlobalTemperature_Delegate);
};

#endif // _GlobalTemperature_Delegate_h_
