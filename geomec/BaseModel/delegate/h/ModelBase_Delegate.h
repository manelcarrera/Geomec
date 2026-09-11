#ifndef _ModelBase_Delegate_h_
#define _ModelBase_Delegate_h_

class CModelBase;

#include "FemAppModel_Delegate.h"

class CModelBase_Delegate : public CFemAppModel_Delegate {
public:
  CModelBase_Delegate(CModelBase *modelBase);

  virtual bool IsMesh() const;

  void AppendSwitchTo(CContextMenuInvoker &invoker);

private:
  CModelBase_Delegate(const CModelBase_Delegate &rhs);
  CModelBase_Delegate &operator=(const CModelBase_Delegate &rhs);

  CModelBase *m_modelBase;

  REGISTER_DELEGATE(CModelBase, CModelBase_Delegate);
};

#endif // _ModelBase_Delegate_h_
