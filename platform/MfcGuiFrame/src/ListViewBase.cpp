// ListViewBase.cpp : implementation file
//

#include "listviewbase.h"
#include "listctrlbase.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CListViewBase

IMPLEMENT_DYNCREATE(CListViewBase, CView)

CListViewBase::CListViewBase() {}

CListViewBase::~CListViewBase() {}

BEGIN_MESSAGE_MAP(CListViewBase, CView)
//{{AFX_MSG_MAP(CListViewBase)
ON_WM_SIZE()
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListViewBase drawing

void CListViewBase::OnDraw(CDC *pDC) {
  CDocument *pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListViewBase diagnostics

#ifdef _DEBUG
void CListViewBase::AssertValid() const { CView::AssertValid(); }

void CListViewBase::Dump(CDumpContext &dc) const { CView::Dump(dc); }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListViewBase message handlers

void CListViewBase::OnSize(unsigned int nType, int cx, int cy) {
  CView::OnSize(nType, cx, cy);
  if (m_pCtrl)
    m_pCtrl->MoveWindow(0, 0, cx, cy);
}

int CListViewBase::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_pCtrl = OnCreateCtrl();

  return 0;
}

CListCtrlBase *CListViewBase::OnCreateCtrl() {
  CListCtrlBase *pCtrl = new CListCtrlBase();
  pCtrl->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | TVS_SHOWSELALWAYS, CRect(0, 0, 0, 0), this, 1);
  return pCtrl;
}

CListCtrlBase &CListViewBase::GetListCtrl() const { return (CListCtrlBase &)(*m_pCtrl); }