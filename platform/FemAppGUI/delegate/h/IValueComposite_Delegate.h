#ifndef _IValueComposite_Delegate_h_
#define _IValueComposite_Delegate_h_

class IValueComposite;

#include "StorageNode_Delegate.h"

class IValueComposite_Delegate : public CStorageNode_Delegate {
public:
  IValueComposite_Delegate(IValueComposite *valueComposite);

private:
  IValueComposite_Delegate(const IValueComposite_Delegate &rhs);
  IValueComposite_Delegate &operator=(const IValueComposite_Delegate &rhs);

  IValueComposite *m_valueComposite;

  REGISTER_DELEGATE(IValueComposite, IValueComposite_Delegate);
};

#endif // _IValueComposite_Delegate_h_
