#ifndef _NodeObserver_Delegate_h_
#define _NodeObserver_Delegate_h_

#include "Delegate.h"
#include "NodeObserver.h"

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class CHILD_TYPE,
  class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
  class CNodeObserver_Delegate : public CNodeObserver <OBSERVED_TYPE,
  CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:
  CNodeObserver_Delegate(OBSERVED_TYPE& node,
                         CTreeNode& parent,
                         const BOOL rename,
                         const enum REMOVE_TYPE remove,
                         HTREEITEM hInsertAfter = TVI_LAST,
                         BOOL bInitialUpdate = TRUE,
                         BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver <OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> (
      node, parent, rename, remove, hInsertAfter, bInitialUpdate,
      bSortAfterInsertion)
  {
  ACTIVATE_TEMPLATE_DELEGATE(OBSERVED_TYPE, DELEGATE_TYPE);
  }

  CNodeObserver_Delegate(OBSERVED_TYPE& node,
                         CTreeCtrl& ctrl,
                         const BOOL rename,
                         const enum REMOVE_TYPE remove,
                         HTREEITEM hParent = TVI_ROOT,
                         HTREEITEM hInsertAfter = TVI_LAST,
                         BOOL bInitialUpdate = TRUE,
                         BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver <OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> (
      node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate,
      bSortAfterInsertion)
  {
  ACTIVATE_TEMPLATE_DELEGATE(OBSERVED_TYPE, DELEGATE_TYPE);
  }

  const DELEGATE_TYPE& Delegate() const
  {
  CDelegate* delegate =
      const_cast <CGraphNode&> (ObservedItem()).getDelegate();
  DELEGATE_TYPE* newDelegate = static_cast <DELEGATE_TYPE*> (delegate);

  return const_cast <const DELEGATE_TYPE&> (*newDelegate);
  }

  DELEGATE_TYPE& Delegate()
  {
  return static_cast <DELEGATE_TYPE&> (*ObservedItem().getDelegate());
  }
};

#endif  // _NodeObserver_Delegate_h_
