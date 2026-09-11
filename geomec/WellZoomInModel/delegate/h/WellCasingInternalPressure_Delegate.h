#ifndef _WellCasingInternalPressure_Delegate_h_
#define _WellCasingInternalPressure_Delegate_h_

class TPressure;
class CWellCasingInternalPressure;

#include "IWellCasingInternalLoad_Delegate.h"

class CWellCasingInternalPressure_Delegate : public IWellCasingInternalLoad_Delegate<TPressure> {
public:
  CWellCasingInternalPressure_Delegate(CWellCasingInternalPressure *wellCasingInternalPressure);

  virtual bool Attributes();

private:
  CWellCasingInternalPressure_Delegate(const CWellCasingInternalPressure_Delegate &rhs);
  CWellCasingInternalPressure_Delegate &operator=(const CWellCasingInternalPressure_Delegate &rhs);

  CWellCasingInternalPressure *m_wellCasingInternalPressure;

  REGISTER_DELEGATE(CWellCasingInternalPressure, CWellCasingInternalPressure_Delegate);
};

#endif // _WellCasingInternalPressure_Delegate_h_
