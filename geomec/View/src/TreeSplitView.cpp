// TreeSplitView.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "TreeSplitView.h"
#include "DataTreeView.h"
#include "ModelTreeView.h"
#include "MainFrm.h"
#include "ISettings.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeSplitView

IMPLEMENT_DYNCREATE(CTreeSplitView, CView)

//##ModelId=3B497D7A009D
CTreeSplitView::CTreeSplitView()
{
}

void CTreeSplitView::GetRowInfo(int row, int& cyCur, int& cyMin) const
{
  m_wndSplitter.GetRowInfo(row, cyCur, cyMin);
}

//##ModelId=3B497D7A00B0
CTreeSplitView::~CTreeSplitView()
{
}


BEGIN_MESSAGE_MAP(CTreeSplitView, CView)
  //{{AFX_MSG_MAP(CTreeSplitView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeSplitView drawing

//##ModelId=3B497D7A00AD
void CTreeSplitView::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeSplitView diagnostics

#ifdef _DEBUG
//##ModelId=3B497D7A00B2
void CTreeSplitView::AssertValid() const
{
  CView::AssertValid();
}

//##ModelId=3B497D7A00BD
void CTreeSplitView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeSplitView message handlers


//##ModelId=3B497D7A00C0
int CTreeSplitView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;

  CCreateContext *pContext = (CCreateContext*) lpCreateStruct->lpCreateParams;

  if(!m_wndSplitter.CreateStatic(this, 2, 1,WS_CHILD|WS_VISIBLE, AFX_IDW_PANE_FIRST+8) ||
    !m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CDataTreeView),
      CSize(0, 0), pContext) ||
    !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CModelTreeView),
      CSize(0, 0), pContext))
  {

    return FALSE;	
  }
  
  return 0;
}

//##ModelId=3B497D7A00C3
void CTreeSplitView::OnSize(unsigned int nType, int cx, int cy) 
{
  // Do the size of the window
  CView::OnSize(nType, cx, cy);

  //Increase size by 2 all around to remove hide border:
  m_wndSplitter.MoveWindow(-2, -2, cx+2, cy+2);
  m_wndSplitter.RecalcLayout();  
}

//##ModelId=3B497D7A009E
void CTreeSplitView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  CRect rect;
  GetWindowRect(&rect);	

  int iTreeSplitViewSplitter = ISettings::instance()->getProfileInt(CMainFrame::s_profileHeading, CMainFrame::s_profileTreeSplitViewSplitter, rect.Height() * 0.75);

  m_wndSplitter.SetRowInfo(0, iTreeSplitViewSplitter, 0);
  m_wndSplitter.RecalcLayout();  
}
