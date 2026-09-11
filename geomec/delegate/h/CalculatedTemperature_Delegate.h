#ifndef _CalculatedTemperature_Delegate_h_
#define _CalculatedTemperature_Delegate_h_

class CCalculatedTemperature;

#include "IValueComposite_Delegate.h"

class CCalculatedTemperature_Delegate : public IValueComposite_Delegate
{
public:
  CCalculatedTemperature_Delegate(
  CCalculatedTemperature* calculatedTemperature);

private:
  CCalculatedTemperature_Delegate(const CCalculatedTemperature_Delegate& rhs);
  CCalculatedTemperature_Delegate& operator = (
  const CCalculatedTemperature_Delegate& rhs);

  CCalculatedTemperature* m_calculatedTemperature;

  REGISTER_DELEGATE(CCalculatedTemperature, CCalculatedTemperature_Delegate);
};

#endif  // _CalculatedTemperature_Delegate_h_
