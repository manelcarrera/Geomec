// CheckedNodeObserver.h: interface for the CCheckedNodeObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLNODEOBSERVER_H__09B8B560_89A8_471A_A29D_A674074F9FB1__INCLUDED_)
#define AFX_OPENGLNODEOBSERVER_H__09B8B560_89A8_471A_A29D_A674074F9FB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geomec.h"
#include "ModelBase.h"
#include "NodeObserver.h"
#include "openglscenenode.h"
#include "resourceIDI.h"

template <class OBSERVED_TYPE, class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class COpenGLNodeObserver : public CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE> {
public:
  COpenGLNodeObserver(OBSERVED_TYPE &node, CTreeNode &parent, const BOOL rename, const enum REMOVE_TYPE remove,
                      HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = FALSE)
      : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(
            node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion) {
    Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  }

  COpenGLNodeObserver(OBSERVED_TYPE &node, CTreeCtrl &ctrl, const BOOL rename, const enum REMOVE_TYPE remove,
                      HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE,
                      BOOL bSortAfterInsertion = FALSE)
      : CNodeObserver<OBSERVED_TYPE, CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>(
            node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate, bSortAfterInsertion) {
    Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  }

  virtual ~COpenGLNodeObserver() {}

  virtual unsigned int StateIcon() const {
    CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
    assert(pDoc->Model());
    const CModelBase &model = dynamic_cast<const CModelBase &>(*pDoc->Model());
    if (pDoc->CurrentScene()) {
      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_RADIO) {
        if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
          return IDI_RADIO_CHECKED;
        if (pDoc->CurrentScene()->CanConnectItem(ObservedItem()))
          return IDI_RADIO_UNCHECKED;
      }

      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_CHECK) {
        if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem()))
          return IDI_CHECKED;
        if (pDoc->CurrentScene()->CanConnectItem(ObservedItem()))
          return IDI_UNCHECKED;
      }
    }

    return 0;
  }

  virtual void ToggleState() {
    CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
    assert(pDoc->CurrentScene());

    if (pDoc->CurrentScene()->IsLinkedTo(ObservedItem())) {
      if (pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_CHECK)
        pDoc->CurrentScene()->UnLink(ObservedItem());
    } else {
      assert(pDoc->CurrentScene()->CanConnectItem(ObservedItem()));
      pDoc->CurrentScene()->ConnectItem(ObservedItem());
    }

    if (Parent())
      Parent()->Update();
  }
};

#endif // !defined(AFX_OPENGLNODEOBSERVER_H__09B8B560_89A8_471A_A29D_A674074F9FB1__INCLUDED_)
