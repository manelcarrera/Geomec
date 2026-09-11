#ifndef _RpnValueSet_Delegate_h_
#define _RpnValueSet_Delegate_h_

class CRpnValueSet;

#include "IValueSet_Delegate.h"

class CRpnValueSet_Delegate : public IValueSet_Delegate
{
public:
  CRpnValueSet_Delegate(CRpnValueSet* rpnValueSet);

  virtual bool Attributes();

  virtual bool CanDestroy() const;

private:
  CRpnValueSet_Delegate(const CRpnValueSet_Delegate& rhs);
  CRpnValueSet_Delegate& operator = (const CRpnValueSet_Delegate& rhs);

  CRpnValueSet* m_rpnValueSet;

  REGISTER_DELEGATE(CRpnValueSet, CRpnValueSet_Delegate);
};

#endif  // _RpnValueSet_Delegate_h_
