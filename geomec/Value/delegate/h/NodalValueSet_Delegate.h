#ifndef _NodalValueSet_Delegate_h_
#define _NodalValueSet_Delegate_h_

class CNodalValueSet;

#include "IValueSet_Delegate.h"

class CNodalValueSet_Delegate : public IValueSet_Delegate
{
public:
  CNodalValueSet_Delegate(CNodalValueSet* nodalValueSet);

  virtual bool Attributes();

private:
  CNodalValueSet_Delegate(const CNodalValueSet_Delegate& rhs);
  CNodalValueSet_Delegate& operator = (const CNodalValueSet_Delegate& rhs);

  CNodalValueSet* m_nodalValueSet;

  REGISTER_DELEGATE(CNodalValueSet, CNodalValueSet_Delegate);
};

#endif  // _NodalValueSet_Delegate_h_
