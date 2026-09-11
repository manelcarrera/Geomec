#ifndef _IMaterialServerBase_Delegate_h_
#define _IMaterialServerBase_Delegate_h_

class IMaterialServerBase;

#include "StorageNode_Delegate.h"

class IMaterialServerBase_Delegate : public CStorageNode_Delegate
{
public:
  IMaterialServerBase_Delegate(IMaterialServerBase* materialServerBase);

private:
  IMaterialServerBase_Delegate(const IMaterialServerBase_Delegate& rhs);
  IMaterialServerBase_Delegate& operator = (
  const IMaterialServerBase_Delegate& rhs);

  IMaterialServerBase* m_materialServerBase;

  REGISTER_DELEGATE(IMaterialServerBase, IMaterialServerBase_Delegate);
};

#endif  // _IMaterialServerBase_Delegate_h_
