#ifndef _ElementValueSet_Delegate_h_
#define _ElementValueSet_Delegate_h_

class CElementValueSet;

#include "IValueSet_Delegate.h"

class CElementValueSet_Delegate : public IValueSet_Delegate {
public:
  CElementValueSet_Delegate(CElementValueSet *elementValueSet);

  virtual bool Attributes();

private:
  CElementValueSet_Delegate(const CElementValueSet_Delegate &rhs);
  CElementValueSet_Delegate &operator=(const CElementValueSet_Delegate &rhs);

  CElementValueSet *m_elementValueSet;

  REGISTER_DELEGATE(CElementValueSet, CElementValueSet_Delegate);
};

#endif // _ElementValueSet_Delegate_h_
