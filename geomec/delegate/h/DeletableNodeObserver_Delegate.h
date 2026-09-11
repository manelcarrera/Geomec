#ifndef _DELETABLENODEOBSERVER_DELEGATE_H_
#define _DELETABLENODEOBSERVER_DELEGATE_H_

#include "NodeObserver_Delegate.h"
#include "GeomecDoc.h"
#include "resourceIDI.h"

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CDeletableNodeObserver_Delegate : public CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:
  CDeletableNodeObserver_Delegate(OBSERVED_TYPE& node,
                                  CTreeNode& parent,
                                  const BOOL rename,
                                  const enum REMOVE_TYPE remove,
                                  HTREEITEM hInsertAfter = TVI_LAST,
                                  BOOL bInitialUpdate = TRUE,
                                  BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node,
                                                                                                     parent,
                                                                                                     rename,
                                                                                                     remove,
                                                                                                     hInsertAfter,
                                                                                                     bInitialUpdate,
                                                                                                     bSortAfterInsertion)
  {
  }

  CDeletableNodeObserver_Delegate(OBSERVED_TYPE& node,
                                  CTreeCtrl& ctrl,
                                  const BOOL rename,
                                  const enum REMOVE_TYPE remove,
                                  HTREEITEM hParent = TVI_ROOT,
                                  HTREEITEM hInsertAfter = TVI_LAST,
                                  BOOL bInitialUpdate = TRUE,
                                  BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node,
                                                                                                     ctrl,
                                                                                                     rename,
                                                                                                     remove,
                                                                                                     hParent,
                                                                                                     hInsertAfter,
                                                                                                     bInitialUpdate,
                                                                                                     bSortAfterInsertion)
  {
  }

  virtual unsigned int StateIcon() const
  {
    CGeomecDoc* pDoc = GetGeomecDoc();

    if(pDoc->Model() && pDoc->ChangeMode() && CanDelete())
      return (pDoc->IsChangeSelected(*this) ? IDI_CHANGE_SELECTED : IDI_CHANGE_UNSELECTED);

    return 0;
  }

  virtual void ToggleState()
  {
    CGeomecDoc* pDoc = GetGeomecDoc();

    pDoc->ToggleChangeState(*this);
    Update();
  }
};

#endif // _DELETABLENODEOBSERVER_DELEGATE_H_
