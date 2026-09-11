#pragma once

#include "OpenGLNode_Delegate.h"

class CGroupNode;
class CGroupNodeEntry;


class CGroupNode_Delegate : public COpenGLNode_Delegate
{
public:
  CGroupNode_Delegate(CGroupNode *groupNode);
  virtual ~CGroupNode_Delegate();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool Attributes();

private:
  CGroupNode_Delegate(const CGroupNode_Delegate& rhs);
  CGroupNode_Delegate& operator=(const CGroupNode_Delegate& rhs);

  REGISTER_DELEGATE(CGroupNode, CGroupNode_Delegate);
};


class CGroupNodeEntry_Delegate : public CStorageNodeEntry_Delegate<CGroupNode>
{
public:
  CGroupNodeEntry_Delegate(CGroupNodeEntry *groupNodeEntry);
  virtual ~CGroupNodeEntry_Delegate();

private:
  CGroupNodeEntry_Delegate(const CGroupNodeEntry_Delegate& rhs);
  CGroupNodeEntry_Delegate& operator =
    (const CGroupNodeEntry_Delegate& rhs);

  REGISTER_DELEGATE(CGroupNodeEntry, CGroupNodeEntry_Delegate);
};
