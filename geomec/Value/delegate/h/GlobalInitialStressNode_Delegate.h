#ifndef _GlobalInitialStressNode_Delegate_h_
#define _GlobalInitialStressNode_Delegate_h_

class CGlobalInitialStressNode;

#include "StorageNode_Delegate.h"

class CGlobalInitialStressNode_Delegate : public CStorageNode_Delegate
{
public:
  CGlobalInitialStressNode_Delegate(
  CGlobalInitialStressNode* globalInitialStressNode);

  virtual bool Attributes();

private:
  CGlobalInitialStressNode_Delegate(
  const CGlobalInitialStressNode_Delegate& rhs);
  CGlobalInitialStressNode_Delegate& operator = (
  const CGlobalInitialStressNode_Delegate& rhs);

  CGlobalInitialStressNode* m_globalInitialStressNode;

  REGISTER_DELEGATE(CGlobalInitialStressNode,
  CGlobalInitialStressNode_Delegate);
};

#endif  // _GlobalInitialStressNode_Delegate_h_
