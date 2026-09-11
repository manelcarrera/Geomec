#ifndef _HorizonBase_Delegate_h_
#define _HorizonBase_Delegate_h_

class CHorizonBase;

#include "ColorNode_Delegate.h"

class CHorizonBase_Delegate : public CColorNode_Delegate {
public:
  CHorizonBase_Delegate(CHorizonBase *horizonBase);

  virtual bool Destroy();

private:
  CHorizonBase_Delegate(const CHorizonBase_Delegate &rhs);
  CHorizonBase_Delegate &operator=(const CHorizonBase_Delegate &rhs);

  CHorizonBase *m_horizonBase;

  REGISTER_DELEGATE(CHorizonBase, CHorizonBase_Delegate);
};

#endif // _HorizonBase_Delegate_h_
