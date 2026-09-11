#ifndef _IMaterialBase_Delegate_h_
#define _IMaterialBase_Delegate_h_

class IMaterialBase;

#include "StorageNode_Delegate.h"

class IMaterialBase_Delegate : public CStorageNode_Delegate
{
public:
  IMaterialBase_Delegate(IMaterialBase* materialBase);

private:
  IMaterialBase_Delegate(const IMaterialBase_Delegate& rhs);
  IMaterialBase_Delegate& operator = (const IMaterialBase_Delegate& rhs);

  IMaterialBase* m_materialBase;

  REGISTER_DELEGATE(IMaterialBase, IMaterialBase_Delegate);
};

#endif  // _IMaterialBase_Delegate_h_
