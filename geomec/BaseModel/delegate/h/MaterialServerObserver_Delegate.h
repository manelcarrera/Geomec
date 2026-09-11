#ifndef _MaterialServerObserver_Delegate_h_
#define _MaterialServerObserver_Delegate_h_

#include "DeletableNodeObserver_Delegate.h"
#include "MaterialServer_Delegate.h"

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class CHILD_TYPE,
  class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
  class CMaterialServerObserver_Delegate : public CDeletableNodeObserver_Delegate <
  OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:
  CMaterialServerObserver_Delegate(OBSERVED_TYPE& node,
                                   CTreeNode& parent,
                                   const BOOL rename,
                                   const enum REMOVE_TYPE remove,
                                   HTREEITEM hInsertAfter = TVI_LAST,
                                   BOOL bInitialUpdate = TRUE,
                                   BOOL bSortAfterInsertion = TRUE)
  : CDeletableNodeObserver_Delegate <OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE,
      CHILD_OBS_TYPE, RENAME, REMOVE> (node, parent, rename, remove,
      hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
  }

  CMaterialServerObserver_Delegate(OBSERVED_TYPE& node,
                                   CTreeCtrl& ctrl,
                                   const BOOL rename,
                                   const enum REMOVE_TYPE remove,
                                   HTREEITEM hParent = TVI_ROOT,
                                   HTREEITEM hInsertAfter = TVI_LAST,
                                   BOOL bInitialUpdate = TRUE,
                                   BOOL bSortAfterInsertion = TRUE)
  : CDeletableNodeObserver_Delegate <OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE,
      CHILD_OBS_TYPE, RENAME, REMOVE> (node, ctrl, rename, remove, hParent,
      hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
  }

  virtual void ToggleState()
  {
  TChildren children = Children();
  for(int i = 0; i < children.size(); ++i)
      children[i]->ToggleState();

  CDeletableNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>::ToggleState();
  }

  virtual BOOL CanDelete() const
  {
  return ObservedItem().CanDestroy();
  }

  virtual void Delete()
  {
  ObservedItem().Destroy();
  }
};

class CDistributedMaterialPropertyObserver : public CValueCompositeObserver
{
public:
  CDistributedMaterialPropertyObserver(IValueComposite& node,
                                       CTreeNode& parent,
                                       const BOOL rename,
                                       const enum REMOVE_TYPE remove,
                                       HTREEITEM hInsertAfter = TVI_LAST,
                                       BOOL bInitialUpdate = FALSE,
                                       BOOL bSortAfterInsertion = TRUE)
  : CValueCompositeObserver(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
  }

  virtual void AppendContextMenu(CContextMenuInvoker& invoker)
  {
  assert(dynamic_cast<CTreeNode*>(Parent()));
  CTreeNode& parent = static_cast<CTreeNode&>(*Parent());

  CGraphNode& parent_node = parent.ObservedItem();
  assert(dynamic_cast<CMaterialServer*>(&parent_node));
  CMaterialServer& server = static_cast<CMaterialServer&>(parent_node);
  CMaterialServer_Delegate& delegate = static_cast<CMaterialServer_Delegate&>(*server.getDelegate());
  
  invoker.AddCommand("Configure distributed properties",
      *new CSingleCommandTemplate<CMaterialServer_Delegate>(delegate, &CMaterialServer_Delegate::ConfigureDistributions, &CMaterialServer_Delegate::CanConfigureDistributions));

  CValueCompositeObserver::AppendContextMenu(invoker);
  }
};

#endif  // _MaterialServerObserver_Delegate_h_
