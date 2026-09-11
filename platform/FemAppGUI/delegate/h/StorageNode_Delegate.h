#ifndef _StorageNode_Delegate_h_
#define _StorageNode_Delegate_h_

#include "GraphNode_Delegate.h"
#include "GraphEntry_Delegate.h"
#include "StorageNode.h"

class CStorageNode_Delegate : public CGraphNode_Delegate
{
public:
  CStorageNode_Delegate(CStorageNode* storageNode);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CStorageNode_Delegate(const CStorageNode_Delegate& rhs);
  CStorageNode_Delegate& operator = (const CStorageNode_Delegate& rhs);

  CStorageNode* m_storageNode;

  REGISTER_DELEGATE(CStorageNode, CStorageNode_Delegate);
};

template <class T>
  class CStorageNodeEntry_Delegate : public CGraphEntryTemp_Delegate <T>
{
public:
  CStorageNodeEntry_Delegate(CStorageNodeEntry <T> * storageNodeEntry);

private:
  CStorageNodeEntry_Delegate(const CStorageNodeEntry_Delegate& rhs);
  CStorageNodeEntry_Delegate& operator = (const CStorageNodeEntry_Delegate& rhs);

  CStorageNodeEntry <T> * m_storageNodeEntry;

  REGISTER_DELEGATE(CStorageNodeEntry <T>, CStorageNodeEntry_Delegate <T>);
};

template <class T>
  CStorageNodeEntry_Delegate <T> ::
  CStorageNodeEntry_Delegate(CStorageNodeEntry <T> * storageNodeEntry)
: CGraphEntryTemp_Delegate(storageNodeEntry)
, m_storageNodeEntry(storageNodeEntry)
{
  ACTIVATE_TEMPLATE_DELEGATE(CStorageNodeEntry <T>,
  CStorageNodeEntry_Delegate <T>);
}

#endif  // _StorageNode_Delegate_h_
