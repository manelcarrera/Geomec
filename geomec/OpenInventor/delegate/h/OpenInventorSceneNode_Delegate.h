#ifndef _OpenInventorSceneNode_Delegate_h_
#define _OpenInventorSceneNode_Delegate_h_

class COpenInventorSceneNode;

#include "StorageNode_Delegate.h"

class COpenInventorSceneNode_Delegate : public CStorageNode_Delegate {
public:
  COpenInventorSceneNode_Delegate(COpenInventorSceneNode *node);

  virtual bool Attributes();

private:
  COpenInventorSceneNode_Delegate(const COpenInventorSceneNode_Delegate &rhs);
  COpenInventorSceneNode_Delegate &operator=(const COpenInventorSceneNode_Delegate &rhs);

  COpenInventorSceneNode *m_Node;

  REGISTER_DELEGATE(COpenInventorSceneNode, COpenInventorSceneNode_Delegate);
};

#endif // _OpenInventorSceneNode_Delegate_h_
