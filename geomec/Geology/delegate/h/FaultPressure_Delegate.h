#ifndef _FaultPressure_Delegate_h_
#define _FaultPressure_Delegate_h_

class CFaultPressure;

#include "IValueComposite_Delegate.h"

class CFaultPressure_Delegate : public IValueComposite_Delegate {
public:
  CFaultPressure_Delegate(CFaultPressure *faultPressure);

  virtual bool Attributes();

private:
  CFaultPressure_Delegate(const CFaultPressure_Delegate &rhs);
  CFaultPressure_Delegate &operator=(const CFaultPressure_Delegate &rhs);

  CFaultPressure *m_faultPressure;

  REGISTER_DELEGATE(CFaultPressure, CFaultPressure_Delegate);
};

#endif // _FaultPressure_Delegate_h_
