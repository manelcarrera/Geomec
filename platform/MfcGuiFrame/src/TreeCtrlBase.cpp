// TreeCtrlBase.cpp : implementation file
//

#include "treectrlbase.h"
#include "ContextMenuInvoker.h"
#include "ITreeObject.h"
#include "stdafx.h"

// Not happy about depending on this, but we shouldn't toggle state if OnLButtonDown receives an IDI_CHECK_DISABLED
#include "resourceIDI.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

#define RECT_BORDER 10
#define TIMER_SET 1
#define TIMER_OFF -1
#define TIMER_EXPIRED -2

bool CTreeCtrlBase::m_bAllowPaint = true;

void CTreeCtrlBase::AllowPaint(bool bAllowPaint) { m_bAllowPaint = bAllowPaint; }

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlBase

CTreeCtrlBase::CTreeCtrlBase()
    : m_bContextOperation(FALSE), m_bTreeDeleted(FALSE), m_timer(TIMER_OFF), m_expand(0), m_hLastDragOverItem(0) {
  m_ilNormal.Create(16, 16, ILC_COLOR16, 1, 1);
  m_ilState.Create(16, 16, ILC_COLOR16, 1, 1);
}

CTreeCtrlBase::~CTreeCtrlBase() {}

BEGIN_MESSAGE_MAP(CTreeCtrlBase, CTreeCtrl)
//{{AFX_MSG_MAP(CTreeCtrlBase)
ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDisplayInfo)
ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
ON_WM_CONTEXTMENU()
ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
ON_WM_CREATE()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONUP()
ON_NOTIFY_REFLECT(NM_RCLICK, OnRightClick)
ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteItem)
ON_WM_ACTIVATE()
ON_WM_TIMER()
ON_WM_PAINT()
ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlBase message handlers

void CTreeCtrlBase::OnGetDisplayInfo(NMHDR *pNMHDR, LRESULT *pResult) {
  TV_ITEM &item = ((TV_DISPINFO *)pNMHDR)->item;
  ITreeObject *pObject = (ITreeObject *)(item.lParam);
  if (pObject) {
    // Are displaylists initialized?
    if (GetImageList(TVSIL_NORMAL) != &m_ilNormal)
      SetImageList(&m_ilNormal, TVSIL_NORMAL);

    if (GetImageList(TVSIL_STATE) != &m_ilState)
      SetImageList(&m_ilState, TVSIL_STATE);

    // Initialize TV_ITEM
    if ((item.mask & TVIF_TEXT) == TVIF_TEXT)
      lstrcpyn(item.pszText, pObject->Text().toStdString().c_str(), item.cchTextMax);

    if (item.mask & TVIF_IMAGE)
      item.iImage = m_ilNormal.IconToImage(pObject->Icon());

    if (item.mask & TVIF_SELECTEDIMAGE)
      item.iSelectedImage = m_ilNormal.IconToImage(pObject->SelectedIcon());

    /*		if(item.mask & TVIF_STATE)
        {
          item.stateMask = TVIS_STATEIMAGEMASK;
          unsigned int stateIcon = pObject->StateIcon();
          if(stateIcon == 0)
            item.state = INDEXTOSTATEIMAGEMASK(0);
          else
            item.state = INDEXTOSTATEIMAGEMASK(m_ilState.IconToImage(stateIcon));
        }*/
  }
  *pResult = 0;
}

void CTreeCtrlBase::OnBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult) {
  TV_ITEM &item = ((TV_DISPINFO *)pNMHDR)->item;

  *pResult = TRUE;

  if (((item.mask & TVIF_TEXT) == TVIF_TEXT) && item.pszText && item.lParam) {
    ITreeObject *pObject = (ITreeObject *)(item.lParam);

    if (pObject->CanEditText())
      *pResult = FALSE;
  }
}

void CTreeCtrlBase::OnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
  if (pNMTreeView->itemNew.lParam) {
    ITreeObject *pObject = (ITreeObject *)(pNMTreeView->itemNew.lParam);
    if (pObject->IsExpanded())
      pObject->OnExpand();
    else
      pObject->OnCollapse();
  }
}

void CTreeCtrlBase::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult) {
  *pResult = FALSE;

  TV_ITEM &item = ((TV_DISPINFO *)pNMHDR)->item;

  if (((item.mask & TVIF_TEXT) == TVIF_TEXT) && item.pszText && (item.lParam)) {
    ITreeObject *pObject = (ITreeObject *)(item.lParam);
    *pResult = pObject->EditText(item.pszText);
  }
}

void CTreeCtrlBase::OnSelChanged(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;

  int ret = 0;
  ITreeObject *pOld = (ITreeObject *)(pNMTreeView->itemOld.lParam);
  ITreeObject *pNew = (ITreeObject *)(pNMTreeView->itemNew.lParam);

  if (pNew) {
    if (pNew->Enable()) {
      // Added LVM_FIRST; this is the action we receive sometimes (eg expanding results) that causes the selection of
      // Global Temperature
      if (!m_bContextOperation && pNMTreeView->action != TVC_UNKNOWN &&
          pNMTreeView->action !=
              LVM_FIRST) // TVC_UNKNOWN: software-generated selchange, don't respond (e.g. collapse of branch)
      {
        if (pNew->OnSelect()) {
          OnSelect(pOld, pNew);
        } else
          ret = 1;
      }
    } else
      ret = 1;
  }

  *pResult = ret;
}

void CTreeCtrlBase::OnSelchanging(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
  int ret = 0;
  ITreeObject *pObject = (ITreeObject *)(pNMTreeView->itemNew.lParam);
  if (pObject) {
    if (!pObject->Enable())
      ret = 1;
  }

  *pResult = ret;
}

void CTreeCtrlBase::OnContextMenu(CWnd *pWnd, CPoint point) {
  CPoint ptCtrl(point);
  ScreenToClient(&ptCtrl);
  HTREEITEM hItem = HitTest(ptCtrl);

  if (hItem) {
    ITreeObject *pObject = (ITreeObject *)(GetItemData(hItem));

    if (pObject) {
      HTREEITEM hPreviousSelect = GetSelectedItem();
      m_bContextOperation = TRUE;
      m_bTreeDeleted = FALSE;
      SelectItem(pObject->Handle());
      OnContext(GetSelection());
      CMenu menu;
      menu.CreatePopupMenu();
      pObject->AppendToMenu(menu);
      if (menu.GetMenuItemCount() != 0) {
        CPoint ptContext(point);
        GetContextWnd().ScreenToClient(&ptContext);
        menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, &GetContextWnd());
      } else {
        // Only use the new implementation when the old one isn't used.
        CContextMenuInvoker menuInvoker;
        pObject->AppendContextMenu(menuInvoker);
        menuInvoker.TrackMenu(point.x, point.y, GetContextWnd());
      }

      // don't select previous when entire tree was rebuilt (i.e. DeleteTree() was called)
      if (!m_bTreeDeleted)
        SelectItem(hPreviousSelect);

      m_bTreeDeleted = FALSE;
      m_bContextOperation = FALSE;
    }
  }
}

void CTreeCtrlBase::OnBeginDrag(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
  HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
  // Highlight selected item
  //	SelectItem(hSelItem);
  Select(hSelItem, TVGN_DROPHILITE);

  m_timer = TIMER_OFF;
  m_expand = 0;

  // Do the selected item in a set
  ITreeObject *pObject = (ITreeObject *)GetItemData(hSelItem);
  if (pObject && pObject->OnBeginDrag()) {
    // Set Dragged objects
    TCtrlObjectVec vcDragged;
    vcDragged.push_back(pObject);

    COleDataSource *poleSourceObj = new COleDataSource;
    CCtrlDropTarget::m_vcDraggedObject = vcDragged;

    // Begin Drag operation
    DROPEFFECT dropeffect = poleSourceObj->DoDragDrop();
    // Remove the highlighting
    SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE, 0);
    // If user is moving item by pressing Shift, delete selected item
    //	if ( dropeffect == DROPEFFECT_MOVE)
    //		DeleteItem(hTSelItem);
    delete poleSourceObj;
  }
  *pResult = 0;
}

int CTreeCtrlBase::OnCreate(LPCREATESTRUCT lpCreateStruct) {

  if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
    return -1;

  RegisterDropTarget();

  return 0;
}
/*
void CTreeCtrlBase::OnLButtonDblClk(unsigned int nFlags, CPoint point)
{
  DWORD dwPos = ::GetMessagePos();
  POINTS points = MAKEPOINTS(dwPos);
  CPoint point1(points.x, points.y);
  ScreenToClient(&point1);
  unsigned int uFlags=0;
  HTREEITEM hItem = HitTest(point1,&uFlags);

  ITreeObject *pObject = (ITreeObject*)(GetItemData(hItem));
  if(pObject)
  {
    pObject->DisplayObject(TRUE);
    SelectItem(hItem);
    Select(hItem, TVGN_CARET);
    SendMessage(TVM_SELECTITEM);
  }

  CTreeCtrl::OnLButtonDblClk(nFlags, point);
}
*/
void CTreeCtrlBase::OnLButtonDown(unsigned int nFlags, CPoint point) {
  DWORD dwPos = ::GetMessagePos();
  POINTS points = MAKEPOINTS(dwPos);
  CPoint point1(points.x, points.y);
  ScreenToClient(&point1);
  unsigned int uFlags = 0;
  HTREEITEM hItem = HitTest(point1, &uFlags);

  if ((hItem) && (uFlags & TVHT_ONITEMSTATEICON)) {
    ITreeObject *pObject = (ITreeObject *)(GetItemData(hItem));
    if (pObject) {

      // Is the item in a state?
      unsigned int state = pObject->StateIcon();
      if (state != 0 && state != IDI_CHECK_DISABLED)
        pObject->ToggleState();
      UpdateTree();
      return; // no need to go further...
    }
  }

  CTreeCtrl::OnLButtonDown(nFlags, point);
}

/*
void CTreeCtrlBase::OnLButtonUp(unsigned int nFlags, CPoint point)
{
  DWORD dwPos = ::GetMessagePos();
  POINTS points = MAKEPOINTS(dwPos);
  CPoint point1(points.x, points.y);
  ScreenToClient(&point1);
  unsigned int uFlags=0;
  HTREEITEM hItem = HitTest(point1,&uFlags);

  if((hItem) && ( uFlags & TVHT_ONITEMSTATEICON ))
  {
    ITreeObject* pObject = (ITreeObject*) (GetItemData(hItem));
    if(pObject) {
      // Is the item in a state?
      if(pObject->StateIcon() != 0)
      {
        pObject->ToggleState();
      }
    }
  }

  CTreeCtrl::OnLButtonUp(nFlags, point);
}
*/
void CTreeCtrlBase::OnRightClick(NMHDR *pNMHDR, LRESULT *pResult) {
  DWORD dwPos = ::GetMessagePos();
  POINTS points = MAKEPOINTS(dwPos);
  CPoint point(points.x, points.y);
  CPoint abspoint = point;
  ScreenToClient(&point);

  HTREEITEM hItem = HitTest(point);

  if (hItem && (GetItemData(hItem)))
    OnContextMenu(this, abspoint);

  *pResult = 0;
}
/*
TListObjectVec CTreeCtrlBase::GetSelection() const
{
  // Return selected objects ....
  TListObjectVec vcSelected;
  POSITION pos = GetFirstSelectedItemPosition();
  while(pos)
  {
    IListObject *pObject = (IListObject*)( GetItemData(GetNextSelectedItem(pos)));
    if(pObject)
      vcSelected.push_back(pObject);
  }

  return vcSelected;
}
*/

void CTreeCtrlBase::OnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult) {
  TV_ITEM &item = ((NM_TREEVIEW *)pNMHDR)->itemOld;

  // Delete object if available
  ITreeObject *pObject = (ITreeObject *)item.lParam;
  if (pObject) {
    SetItemData(pObject->Handle(), 0);
    delete pObject;
  }

  *pResult = 0;
}

DROPEFFECT CTreeCtrlBase::OnDragOver(TCtrlObjectVec vcDragged, COleDataObject *pDataObject, DWORD dwKeyState,
                                     CPoint point) {
  DROPEFFECT deRet = DROPEFFECT_NONE;

  HTREEITEM hItem = HitTest(point);
  if (hItem) {
    m_hLastDragOverItem = hItem;
    ITreeObject *pObject = (ITreeObject *)GetItemData(hItem);

    if (pObject) {
      deRet = pObject->CanDrop(vcDragged, (dwKeyState & MK_SHIFT) == MK_SHIFT);
      if (deRet != DROPEFFECT_NONE)
        SelectDropTarget(hItem);
    }

    switch (m_timer) {
    case TIMER_SET:
      if (hItem != m_expand) {
        // Set the time again
        KillTimer(m_timer);
        m_timer = SetTimer(TIMER_SET, 400, 0);
        assert(m_timer == TIMER_SET);
      }
      break;
    case TIMER_OFF:
      m_timer = SetTimer(TIMER_SET, 400, 0);
      assert(m_timer == TIMER_SET);
      break;
    case TIMER_EXPIRED:
      if (hItem == m_expand)
        Expand(hItem, TVE_EXPAND);
      m_timer = TIMER_OFF;
      break;
    default:
      assert(FALSE);
    }

    m_expand = hItem;
  } else {
    m_hLastDragOverItem = 0;
  }

  // Scroll Tree control depending on mouse position
  CRect rectClient;
  GetClientRect(&rectClient);
  ClientToScreen(rectClient);
  ClientToScreen(&point);
  int nScrollDir = -1;
  if (point.y >= rectClient.bottom - RECT_BORDER)
    nScrollDir = SB_LINEDOWN;
  else if ((point.y <= rectClient.top + RECT_BORDER))
    nScrollDir = SB_LINEUP;

  if (nScrollDir != -1) {
    int nScrollPos = GetScrollPos(SB_VERT);
    WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
    SendMessage(WM_VSCROLL, wParam);
  }

  nScrollDir = -1;
  if (point.x <= rectClient.left + RECT_BORDER)
    nScrollDir = SB_LINELEFT;
  else if (point.x >= rectClient.right - RECT_BORDER)
    nScrollDir = SB_LINERIGHT;

  if (nScrollDir != -1) {
    int nScrollPos = GetScrollPos(SB_VERT);
    WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
    SendMessage(WM_HSCROLL, wParam);
  }
  return deRet;
}

void CTreeCtrlBase::OnDragLeave(TCtrlObjectVec vcDragged) {
  if (m_hLastDragOverItem) {
    ITreeObject *pObject = (ITreeObject *)GetItemData(m_hLastDragOverItem);
    if (pObject)
      pObject->OnDragLeave(vcDragged);
  }
}

BOOL CTreeCtrlBase::OnDrop(TCtrlObjectVec vcDragged, COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point) {
  BOOL bRet = FALSE;

  HTREEITEM hItem = HitTest(point);
  if (hItem) {
    ITreeObject *pObject = (ITreeObject *)GetItemData(hItem);

    if (pObject) {
      if (pObject->CanDrop(vcDragged, dropEffect == DROPEFFECT_MOVE)) {
        bRet = TRUE;
        pObject->Drop(vcDragged, dropEffect == DROPEFFECT_MOVE);
      }
    }

    // Remove highlighting
    SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE, 0);
  }

  return bRet;
}

TTreeObjectVec CTreeCtrlBase::GetSelection() const {
  TTreeObjectVec vcSelected;
  HTREEITEM hItem = GetSelectedItem();

  if (hItem && GetItemData(hItem))
    vcSelected.push_back((ITreeObject *)GetItemData(hItem));

  return vcSelected;
}

void CTreeCtrlBase::UnSelectAll() {
  HTREEITEM hItem = GetSelectedItem();
  SetItemState(hItem, 0, TVIS_SELECTED);
}

void CTreeCtrlBase::UnSelectStateless() // Bug 121718
{
  HTREEITEM hItem = GetSelectedItem();
  if (hItem) {
    ITreeObject *pObject = (ITreeObject *)GetItemData(hItem);
    if (pObject && pObject->StateIcon() == 0)
      SelectItem(NULL);
  }
}

void CTreeCtrlBase::OnSelect(ITreeObject *pOld, ITreeObject *pNew) {
  // Std handler
}

CWnd &CTreeCtrlBase::GetContextWnd() const { return (CWnd &)(*this); }

void CTreeCtrlBase::OnActivate(unsigned int nState, CWnd *pWndOther, BOOL bMinimized) {
  CTreeCtrl::OnActivate(nState, pWndOther, bMinimized);

  if ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE)) {
    ITreeObject *pObject = 0;

    HTREEITEM hItem = GetSelectedItem();
    if (hItem && GetItemData(hItem))
      pObject = (ITreeObject *)GetItemData(hItem);

    OnSelect(0, pObject);
  }
}

void CTreeCtrlBase::RegisterDropTarget() { m_target.Register(this); }

void CTreeCtrlBase::OnCopy() {
  // Copy the selection
  TTreeObjectVec vcSel = GetSelection();

  m_vcClipBoard.clear();

  for (size_t i = 0; i < vcSel.size(); i++)
    m_vcClipBoard.push_back(vcSel[i]);

  assert(m_vcClipBoard.size() == vcSel.size());
}

BOOL CTreeCtrlBase::CanPaste() {
  // Scan for paste ...
  TTreeObjectVec vcSelection = GetSelection();

  BOOL bCanPaste = vcSelection.size() > 0;

  for (size_t i = 0; i < vcSelection.size(); i++)
    if (vcSelection[i]->CanDrop(m_vcClipBoard, FALSE) == DROPEFFECT_NONE)
      bCanPaste = FALSE;

  return bCanPaste;
}

void CTreeCtrlBase::OnPaste() {
  // Scan for paste ...
  TTreeObjectVec vcSelection = GetSelection();

  for (size_t i = 0; i < vcSelection.size(); i++)
    if (vcSelection[i]->CanDrop(m_vcClipBoard, FALSE))
      vcSelection[i]->Drop(m_vcClipBoard, FALSE);
}

void CTreeCtrlBase::OnContext(const TTreeObjectVec &vec) {
  // Std handler ...
}

void CTreeCtrlBase::UpdateTree() {
  HTREEITEM hRoot = GetRootItem();

  while (hRoot) {
    ITreeObject *pObject = (ITreeObject *)(GetItemData(hRoot)); // Data must be available
    if (pObject)
      pObject->Update();

    // Update childs of current object
    UpdateChildren(pObject->Handle());

    hRoot = GetNextItem(hRoot, TVGN_NEXT);
  };
}

BOOL CTreeCtrlBase::DeleteTree() {
  m_bTreeDeleted = TRUE;
  return DeleteAllItems();
}

void CTreeCtrlBase::UpdateChildren(HTREEITEM hParent) {
  HTREEITEM hChildItem = GetChildItem(hParent);

  while (hChildItem) {
    // Update the ITreeObject if any ...
    ITreeObject *pObject = (ITreeObject *)(GetItemData(hChildItem)); // Data must be available
    if (pObject) {
      pObject->Update();

      // Update children of current object
      if (pObject->Handle())
        UpdateChildren(pObject->Handle());
    }

    hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
  }
}

void CTreeCtrlBase::OnPaint() {
  if (!m_bAllowPaint)
    return;

  CPaintDC dc(this); // device context for painting

  // Create a memory DC compatible with the paint DC
  CDC memDC;
  memDC.CreateCompatibleDC(&dc);

  CRect rcClip, rcClient;
  dc.GetClipBox(&rcClip);
  GetClientRect(&rcClient);

  // Select a compatible bitmap into the memory DC
  CBitmap bitmap;
  bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
  memDC.SelectObject(&bitmap);

  // Set clip region to be same as that in paint DC
  CRgn rgn;
  rgn.CreateRectRgnIndirect(&rcClip);
  memDC.SelectClipRgn(&rgn);
  rgn.DeleteObject();

  // First let the control do its default drawing.
  CWnd::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);

  HTREEITEM hItem = GetFirstVisibleItem();

  int n = GetVisibleCount() + 1;
  while (hItem && n--) {
    CRect rect;
    ITreeObject *pObject = (ITreeObject *)(GetItemData(hItem));

    // Do not meddle with selected items or drop highlighted items
    unsigned int selflag = TVIS_DROPHILITED | TVIS_SELECTED;

    if (!(GetItemState(hItem, selflag) & selflag) && pObject->HasSpecialProps()) {

      CFont *pFontDC;
      CFont fontDC;
      LOGFONT logfont;

      logfont = pObject->Font();

      if (!(logfont.lfFaceName[0] != '\0')) {
        // No font specified, so use window font
        CFont *pFont = GetFont();
        pFont->GetLogFont(&logfont);
      }

      if (pObject->Bold())
        logfont.lfWeight = 700;

      fontDC.CreateFontIndirect(&logfont);
      pFontDC = memDC.SelectObject(&fontDC);

      if (pObject->Color() != (COLORREF)-1)
        memDC.SetTextColor(pObject->Color());

      CString sItem = GetItemText(hItem);

      GetItemRect(hItem, &rect, TRUE);
      memDC.SetBkColor(GetSysColor(COLOR_WINDOW));
      memDC.TextOut(rect.left + 2, rect.top + 1, sItem);

      memDC.SelectObject(pFontDC);
    }
    hItem = GetNextVisibleItem(hItem);
  }

  dc.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, rcClip.left, rcClip.top, SRCCOPY);

  // Do not call CTreeView::OnPaint() for painting messages
}

void CTreeCtrlBase::OnTimer(UINT_PTR nIDEvent) {
  if (nIDEvent == TIMER_SET)
    m_timer = TIMER_EXPIRED;

  CTreeCtrl::OnTimer(nIDEvent);
}

CIconImageList *
CTreeCtrlBase::GetIconImageList(unsigned int nImage) // Need state list before activated (state is not a callback)
{
  switch (nImage) {
  case TVSIL_STATE:
    return &m_ilState;
    break;
  case TVSIL_NORMAL:
    return &m_ilNormal;
    break;
  }
  assert(false);
  return 0;
}