#ifndef _IValueSet_Delegate_h_
#define _IValueSet_Delegate_h_

class IValueSet;

#include "StorageNode_Delegate.h"

class IValueSet_Delegate : public CStorageNode_Delegate
{
public:
  IValueSet_Delegate(IValueSet* valueSet);

private:
  IValueSet_Delegate(const IValueSet_Delegate& rhs);
  IValueSet_Delegate& operator = (const IValueSet_Delegate& rhs);

  IValueSet* m_valueSet;

  REGISTER_DELEGATE(IValueSet, IValueSet_Delegate);
};

#endif  // _IValueSet_Delegate_h_
