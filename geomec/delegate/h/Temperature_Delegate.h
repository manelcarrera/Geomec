#ifndef _Temperature_Delegate_h_
#define _Temperature_Delegate_h_

class TTemperature;
class CTemperatureGradientComponent;
class CTemperatureRepeaterComponent;
class CTemperatureGWCDummyComponent;
class CTemperature;

#include "FormationLoad_Delegate.h"

typedef CFormationLoad_Delegate <TTemperature, CTemperatureGradientComponent,
  CTemperatureRepeaterComponent, CTemperatureGWCDummyComponent>
    TTemperatureBase_Delegate;

class CTemperature_Delegate : public TTemperatureBase_Delegate
{
public:
  CTemperature_Delegate(CTemperature* temperature);

  virtual bool Attributes();

private:
  CTemperature_Delegate(const CTemperature_Delegate& rhs);
  CTemperature_Delegate& operator = (const CTemperature_Delegate& rhs);

  CTemperature* m_temperature;

  REGISTER_DELEGATE(CTemperature, CTemperature_Delegate);
};

#endif  // _Temperature_Delegate_h_
