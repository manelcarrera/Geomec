// CutsurfaceTreeView.cpp : implementation file
//

#include "stdafx.h"

#include "CutSurfaceFrame.h"
#include "CutsurfaceTreeView.h"
#include "GeoObjectHandlerBase.h"
#include "OpenGLScene.h"
#include "OpenGLView.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCutsurfaceTreeView

IMPLEMENT_DYNCREATE(CCutsurfaceTreeView, CTreeView)

CCutsurfaceTreeView::CCutsurfaceTreeView() {}

CCutsurfaceTreeView::~CCutsurfaceTreeView() {}

BEGIN_MESSAGE_MAP(CCutsurfaceTreeView, CTreeView)
//{{AFX_MSG_MAP(CCutsurfaceTreeView)
// NOTE - the ClassWizard will add and remove mapping macros here.
ON_WM_CREATE()
ON_WM_LBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCutsurfaceTreeView drawing

void CCutsurfaceTreeView::OnDraw(CDC *pDC) {
  CDocument *pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CCutsurfaceTreeView diagnostics

int CCutsurfaceTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  lpCreateStruct->style =
      TVS_CHECKBOXES | WS_BORDER | WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;

  // m_tree->ModifyStyleEx(0, WS_EX_CLIENTEDGE);

  if (CTreeView::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO: Add your specialized creation code here

  return 0;
}

void CCutsurfaceTreeView::OnLButtonDown(unsigned int nFlags, CPoint point) {
  // TODO: Add your message handler code here and/or call default

  CTreeView::OnLButtonDown(nFlags, point);

  CPoint ptScreen;
  if (GetCursorPos(&ptScreen)) {
    CPoint ptClient(ptScreen);
    ScreenToClient(&ptClient);
    TVHITTESTINFO HitTestInfo;
    HitTestInfo.pt = ptClient;
    HitTestInfo.flags = 0;
    HitTestInfo.hItem = 0;

    HTREEITEM hTreeItem = this->GetTreeCtrl().HitTest(&HitTestInfo);

    // The user has clicked on checkbox
    if (HitTestInfo.flags & TVHT_ONITEMSTATEICON) {

      CDisplayPair *dp1 = (CDisplayPair *)GetTreeCtrl().GetItemData(hTreeItem);
      if (dp1) {
        dp1->DrawDef().Visible(GetTreeCtrl().GetCheck(hTreeItem));
        m_pFrame->UpdateScene();
      } else {
        BOOL checked = GetTreeCtrl().GetCheck(hTreeItem);
        HTREEITEM child = GetTreeCtrl().GetChildItem(hTreeItem);
        HTREEITEM hNextItem;
        while (child != NULL) {
          hNextItem = GetTreeCtrl().GetNextItem(child, TVGN_NEXT);
          CDisplayPair *dp2 = (CDisplayPair *)GetTreeCtrl().GetItemData(child);
          GetTreeCtrl().SetCheck(child, checked);
          if (dp2) {

            dp2->DrawDef().Visible(GetTreeCtrl().GetCheck(child));
          }

          child = hNextItem;
        }
        m_pFrame->UpdateScene();
      }
    }
  }
}

#ifdef _DEBUG
void CCutsurfaceTreeView::AssertValid() const { CTreeView::AssertValid(); }

void CCutsurfaceTreeView::Dump(CDumpContext &dc) const { CTreeView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCutsurfaceTreeView message handlers
