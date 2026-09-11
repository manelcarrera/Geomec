#ifndef _OpenGLNodeObserver_Delegate_h_
#define _OpenGLNodeObserver_Delegate_h_

#include "Geomec.h"
#include "ModelBase.h"


#include "DeletableNodeObserver_Delegate.h"
#include "openglscenenode.h"
#include "OpenGLSceneBase.h"
#include "ISceneWrapper.h"

template <class OBSERVED_TYPE, class DELEGATE_TYPE, class CHILD_TYPE,
class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class COpenGLNodeObserver_Delegate : public CDeletableNodeObserver_Delegate <
  OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
public:
  COpenGLNodeObserver_Delegate(OBSERVED_TYPE& node,
    CTreeNode& parent,
    const BOOL rename,
    const enum REMOVE_TYPE remove,
    HTREEITEM hInsertAfter = TVI_LAST,
    BOOL bInitialUpdate = TRUE,
    BOOL bSortAfterInsertion = FALSE)
    : CDeletableNodeObserver_Delegate <OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE,
    CHILD_OBS_TYPE, RENAME, REMOVE> (node, parent, rename, remove,
    hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }

  COpenGLNodeObserver_Delegate(OBSERVED_TYPE& node,
    CTreeCtrl& ctrl,
    const BOOL rename,
    const enum REMOVE_TYPE remove,
    HTREEITEM hParent = TVI_ROOT,
    HTREEITEM hInsertAfter = TVI_LAST,
    BOOL bInitialUpdate = TRUE,
    BOOL bSortAfterInsertion = FALSE)
    : CDeletableNodeObserver_Delegate <OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE,
    CHILD_OBS_TYPE, RENAME, REMOVE> (node, ctrl, rename, remove, hParent,
    hInsertAfter, bInitialUpdate, bSortAfterInsertion)
  {
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);
  }

  virtual ~COpenGLNodeObserver_Delegate()
  {
  }

  virtual unsigned int StateIcon() const
  {
    CGeomecDoc* pDoc = &((CGeomecApp*) AfxGetApp())->GetDoc();

    //assert(pDoc->Model());
    // we're getting called when closing GM while in zoom-in model; so we change the assert into a check
    if (!pDoc->Model())
          return 0;

    if(pDoc->ChangeMode())
    {
          return CDeletableNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>::StateIcon();
    }

    const CModelBase& model = dynamic_cast <const CModelBase&> (*pDoc->Model());

    if (pDoc->CurrentScene())
    {
      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_RADIO)
      {
        if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
        {
          return IDI_RADIO_CHECKED;
        }

        if (pDoc->CurrentScene()->CanConnectItem(ObservedItem()))
        {
          return IDI_RADIO_UNCHECKED;
        }
      }

      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_CHECK)
      {
        if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
        {
          return pDoc->CurrentScene()->CanConnectToMultipleItems(ObservedItem()) ? IDI_CHECKED : IDI_RADIO_CHECKED;
        }

        if (pDoc->CurrentScene()->CanConnectItem(ObservedItem()))
        {
          return pDoc->CurrentScene()->CanConnectToMultipleItems(ObservedItem()) ? IDI_UNCHECKED : IDI_RADIO_UNCHECKED;
        }

        if (pDoc->CurrentScene()->CanConnectToMultipleItems(ObservedItem()))
        {
          return IDI_CHECK_DISABLED;
        }
      }
    }

    return 0;
  }

  virtual void ToggleState()
  {
    CGeomecDoc* pDoc = &((CGeomecApp*) AfxGetApp())->GetDoc();

    if(pDoc->ChangeMode())
    {
          CDeletableNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>::ToggleState();
    }
    else
    {
    assert(pDoc->CurrentScene());

    if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
    {
      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_CHECK)
      {
        pDoc->CurrentScene()->UnLink(ObservedItem());
      }
    }
    else
    {
      // assert(pDoc->CurrentScene()->CanConnectItem(ObservedItem()));
          if (pDoc->CurrentScene()->CanConnectItem(ObservedItem()))
      pDoc->CurrentScene()->ConnectItem(ObservedItem());
    }
    }

    /*if (Parent())
    {
      Parent()->Update();
    }*/
  }

  // Let OIV explicitly unlink the node; it will be too late in ~GraphNode(), as OIV won't recognize the stripped class
  virtual void Delete()
  {
      CGeomecDoc* pDoc = &((CGeomecApp*) AfxGetApp())->GetDoc();

      assert(pDoc->CurrentScene());

      if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
      {
    static_cast<CGraphNode&>(ObservedItem()).HandleStatus(CGraphNode::FORCE_DELETE);
    pDoc->CurrentScene()->UnLink(ObservedItem());
      }

      typename CNodeObserver_Delegate<OBSERVED_TYPE, DELEGATE_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>::Delete();
  }

};

#endif  // _OpenGLNodeObserver_Delegate_h_
