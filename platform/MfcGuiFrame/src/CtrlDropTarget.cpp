// CtrlDropTarget.cpp: implementation of the CCtrlDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "CtrlDropTarget.h"
#include "ITreeObject.h"
#include "ListCtrlBase.h"
#include "TreeCtrlBase.h"
#include "stdafx.h"
#define RECT_BORDER 10

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCtrlObjectVec CCtrlDropTarget::m_vcDraggedObject;

CCtrlDropTarget::CCtrlDropTarget() {}

CCtrlDropTarget::~CCtrlDropTarget() {}

/********************************************************************
OnDragEnter()
  Called when the user drags the object in Tree control.
********************************************************************/
DROPEFFECT CCtrlDropTarget::OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point) {
  if (m_vcDraggedObject.size() == 0)
    return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);

  CTreeCtrl *pTreeCtrl = dynamic_cast<CTreeCtrl *>(pWnd);
  CListCtrl *pListCtrl = dynamic_cast<CListCtrl *>(pWnd);

  if (pTreeCtrl) {
    HTREEITEM hItem = pTreeCtrl->HitTest(point);

    if (hItem) {
      ITreeObject *pObject = (ITreeObject *)pTreeCtrl->GetItemData(hItem);

      if (pObject)
        return pObject->CanDrop(m_vcDraggedObject, (dwKeyState & MK_SHIFT) == MK_SHIFT);
    }
  }

  if (pListCtrl) {
    int nItem = pListCtrl->HitTest(point);

    if (nItem != -1) {
      IListObject *pObject = (IListObject *)pListCtrl->GetItemData(nItem);

      if (pObject)
        return pObject->CanDrop(m_vcDraggedObject, (dwKeyState & MK_SHIFT) == MK_SHIFT);
    }
  }

  // TODO : List ctrl code ...
  return DROPEFFECT_NONE;
}

/********************************************************************
OnDragOver()
  Called when the user drags the object over Tree control.
********************************************************************/

DROPEFFECT CCtrlDropTarget::OnDragOver(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point) {
  if (m_vcDraggedObject.size() == 0)
    return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);

  CTreeCtrlBase *pTreeCtrl = dynamic_cast<CTreeCtrlBase *>(pWnd);
  CListCtrlBase *pListCtrl = dynamic_cast<CListCtrlBase *>(pWnd);

  // Expand and highlight the item under the mouse and
  if (pTreeCtrl)
    return pTreeCtrl->OnDragOver(m_vcDraggedObject, pDataObject, dwKeyState, point);

  if (pListCtrl)
    return pListCtrl->OnDragOver(m_vcDraggedObject, pDataObject, dwKeyState, point);

  return DROPEFFECT_NONE;
}

/********************************************************************
OnDragLeave()
  Called when the user drags the object out of Tree control.
********************************************************************/
void CCtrlDropTarget::OnDragLeave(CWnd *pWnd) {
  if (m_vcDraggedObject.size() == 0)
    COleDropTarget::OnDragLeave(pWnd);

  CTreeCtrl *pTreeCtrl = dynamic_cast<CTreeCtrl *>(pWnd);
  CListCtrl *pListCtrl = dynamic_cast<CListCtrl *>(pWnd);

  // Remove Highlighting
  if (pTreeCtrl) {
    CTreeCtrlBase *pTreeCtrlBase = dynamic_cast<CTreeCtrlBase *>(pTreeCtrl);
    if (pTreeCtrlBase)
      pTreeCtrlBase->OnDragLeave(m_vcDraggedObject);

    pTreeCtrl->SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE, 0);
  }

  //	if(pListCtrl)
  //		pTreeCtrl->SendMessage(LVM_SELECTITEM, LVGN_DROPHILITE,0);
}

/********************************************************************
OnDrop()
  Called when the user drops the object in the  Tree control.
********************************************************************/
BOOL CCtrlDropTarget::OnDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point) {
  if (m_vcDraggedObject.size() == 0)
    COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);

  CTreeCtrlBase *pTreeCtrl = dynamic_cast<CTreeCtrlBase *>(pWnd);
  CListCtrlBase *pListCtrl = dynamic_cast<CListCtrlBase *>(pWnd);

  // Expand and highlight the item under the mouse and
  if (pTreeCtrl)
    pTreeCtrl->OnDrop(m_vcDraggedObject, pDataObject, dropEffect, point);

  if (pListCtrl)
    pListCtrl->OnDrop(m_vcDraggedObject, pDataObject, dropEffect, point);

  return FALSE;
}
