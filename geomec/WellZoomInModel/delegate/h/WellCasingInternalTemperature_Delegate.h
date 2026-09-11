#ifndef _WellCasingInternalTemperature_Delegate_h_
#define _WellCasingInternalTemperature_Delegate_h_

class TTemperature;
class CWellCasingInternalTemperature;

#include "IWellCasingInternalLoad_Delegate.h"

class CWellCasingInternalTemperature_Delegate :
  public IWellCasingInternalLoad_Delegate <TTemperature>
{
public:
  CWellCasingInternalTemperature_Delegate(
  CWellCasingInternalTemperature* wellCasingInternalTemperature);

  virtual bool Attributes();

private:
  CWellCasingInternalTemperature_Delegate(
  const CWellCasingInternalTemperature_Delegate& rhs);
  CWellCasingInternalTemperature_Delegate& operator = (
  const CWellCasingInternalTemperature_Delegate& rhs);

  CWellCasingInternalTemperature* m_wellCasingInternalTemperature;

  REGISTER_DELEGATE(CWellCasingInternalTemperature,
  CWellCasingInternalTemperature_Delegate);
};

#endif  // _WellCasingInternalTemperature_Delegate_h_
