#ifndef _OIVSCENEBASE_DELEGATE_H_
#define _OIVSCENEBASE_DELEGATE_H_

class COIVSceneBase;

#include "StorageNode_Delegate.h"

class COIVSceneBase_Delegate : public CStorageNode_Delegate {
  COIVSceneBase_Delegate(const COIVSceneBase_Delegate &rhs);
  COIVSceneBase_Delegate &operator=(const COIVSceneBase_Delegate &rhs);

  COIVSceneBase *m_Node;

  REGISTER_DELEGATE(COIVSceneBase, COIVSceneBase_Delegate);

public:
  COIVSceneBase_Delegate(COIVSceneBase *node);
};

#endif // _OIVSCENEBASE_DELEGATE_H_
