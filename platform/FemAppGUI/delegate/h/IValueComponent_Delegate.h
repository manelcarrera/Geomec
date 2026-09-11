#ifndef _IValueComponent_Delegate_h_
#define _IValueComponent_Delegate_h_

class IValueComponentBase;

#include "StorageNode_Delegate.h"

class IValueComponent_Delegate : public CStorageNode_Delegate {
public:
  IValueComponent_Delegate(IValueComponentBase *valueComponent);

private:
  IValueComponent_Delegate(const IValueComponent_Delegate &rhs);
  IValueComponent_Delegate &operator=(const IValueComponent_Delegate &rhs);

  IValueComponentBase *m_valueComponent;

  REGISTER_DELEGATE(IValueComponentBase, IValueComponent_Delegate);
};

#endif // _IValueComponent_Delegate_h_
