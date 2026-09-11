#if !defined(AFX_STATEBRANCH_H__INCLUDED_)
#define AFX_STATEBRANCH_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<class OBSERVED_TYPE, class CHILD_TYPE, class CHILD_OBS_TYPE, class UNDEFINEDICON_PROVIDER, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CStateBranch : public CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:

  CStateBranch(OBSERVED_TYPE& node,
         CTreeNode &parent,
         const BOOL rename,
         const enum REMOVE_TYPE remove,
         HTREEITEM hInsertAfter = TVI_LAST,
         const BOOL bInitialUpdate = TRUE,
         BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
    // Set the pointer ...
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }

  CStateBranch(OBSERVED_TYPE& node,
         CTreeCtrl &ctrl,
         const BOOL rename,
         const enum REMOVE_TYPE remove,
         HTREEITEM hParent = TVI_ROOT,
         HTREEITEM hInsertAfter = TVI_LAST,
         const BOOL bInitialUpdate = TRUE,
         BOOL bSortAfterInsertion = TRUE)
  : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
    // Set the pointer ...
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }



  //##ModelId=3BC55D64026F
  virtual void ToggleState()
  {
    if(AllNoState())
      return;

    COperation* pOperation = 0;
    CGraphEntry* pEntry = dynamic_cast<CGraphEntry*> (&ObservedItem());
    if(pEntry)
      pOperation = new COperation(pEntry->Model());

    if(AllSameState())
    {
      // Just toggle ...
      TChildren children = Children();
      for(TChildren::iterator it = children.begin(); it != children.end(); it++)
      {
    if((*it)->StateIcon() != 0)
    {
          (*it)->ToggleState();
          (*it)->Update();
    }
      }

      if(pOperation) delete pOperation;
      return;
    }

    CTreeView *pView = 0;
    // Take first state as target ...
    TChildren children = Children();
    unsigned int uTarget = (*children.begin())->StateIcon();
    for(TChildren::iterator it = children.begin(); it != children.end(); it++)
    {
      ITreeObject *pObserver = (ITreeObject*)(*it);

      if(pObserver->StateIcon() != 0 && pObserver->StateIcon() != uTarget)
      {
        pObserver->ToggleState();
        pObserver->Update();

      }
    }

    if(pOperation) delete pOperation;
    assert(AllSameState());
  }	

  virtual unsigned int StateIcon() const
  {
    if(AllNoState())
      return 0;

    if(AllSameState())
    {
      int nZero = 0;
      int nDisabled = 0;

      TChildren children = Children();
      for(TChildren::iterator it = children.begin(); it != children.end(); ++it)
      {
    unsigned int stateIcon = (*it)->StateIcon();

    if (stateIcon != 0)
    {
          if (stateIcon == IDI_CHECK_DISABLED)
      ++nDisabled;
          else
      return stateIcon;
    }
    else
          ++nZero;
      }
  
      if (!nZero && nDisabled)
    return IDI_CHECK_DISABLED;
  }

  UNDEFINEDICON_PROVIDER provider;
  return provider.UndefinedIconId();
  }

};



          



#endif // !defined(AFX_STATEBRANCH_H__INCLUDED_)