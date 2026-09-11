// TreeViewBase.cpp : implementation file
//

#include "treeviewbase.h"
#include "TreeCtrlBase.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewBase

IMPLEMENT_DYNCREATE(CTreeViewBase, CTreeView)

CTreeViewBase::CTreeViewBase() : m_pCtrl(0) {}

CTreeViewBase::~CTreeViewBase() { delete m_pCtrl; }

BEGIN_MESSAGE_MAP(CTreeViewBase, CView)
//{{AFX_MSG_MAP(CTreeViewBase)
ON_WM_CREATE()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewBase drawing

/////////////////////////////////////////////////////////////////////////////
// CTreeViewBase diagnostics

#ifdef _DEBUG
void CTreeViewBase::AssertValid() const { CView::AssertValid(); }

void CTreeViewBase::Dump(CDumpContext &dc) const { CView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewBase message handlers

CTreeCtrlBase *CTreeViewBase::OnCreateCtrl() {
  CTreeCtrlBase *pCtrl = new CTreeCtrlBase();
  pCtrl->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
                    TVS_EDITLABELS | TVS_SHOWSELALWAYS,
                CRect(0, 0, 0, 0), this, 1);
  return pCtrl;
}

CTreeCtrlBase &CTreeViewBase::GetTreeCtrl() const {
  assert(m_pCtrl);
  return *m_pCtrl;
}

int CTreeViewBase::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_pCtrl = OnCreateCtrl();

  return 0;
}

void CTreeViewBase::OnSize(unsigned int nType, int cx, int cy) {
  CView::OnSize(nType, cx, cy);
  if (m_pCtrl)
    m_pCtrl->MoveWindow(0, 0, cx, cy);
}

void CTreeViewBase::OnDraw(CDC *pDC) {
  // Nothing to draw
}
