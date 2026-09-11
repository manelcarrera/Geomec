// OpenGLSplitView.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "OpenGLSplitView.h"
#include "ModelView.h"
#include "LegendView.h"
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
// COpenGLSplitView

IMPLEMENT_DYNCREATE(COpenGLSplitView, CView)

//##ModelId=3B6549E102D0
COpenGLSplitView::COpenGLSplitView()
: m_bRecursionLock(false), m_originalCx(-1)
{
}

//##ModelId=3B6549E102E2
COpenGLSplitView::~COpenGLSplitView()
{
}


BEGIN_MESSAGE_MAP(COpenGLSplitView, CView)
  //{{AFX_MSG_MAP(COpenGLSplitView)
  ON_WM_CREATE()
  ON_WM_ERASEBKGND()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
  ON_WM_MOUSEWHEEL()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLSplitView drawing

//##ModelId=3B6549E102D3
void COpenGLSplitView::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLSplitView diagnostics

#ifdef _DEBUG
//##ModelId=3B6549E102E4
void COpenGLSplitView::AssertValid() const
{
  CView::AssertValid();
}

//##ModelId=3B6549E102E6
void COpenGLSplitView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLSplitView message handlers

//##ModelId=3B6549E102F1
int COpenGLSplitView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CView::OnCreate(lpCreateStruct) == -1)
    return -1;
  
  CCreateContext *pContext = (CCreateContext*) lpCreateStruct->lpCreateParams;

  if(!m_wndSplitter.CreateStatic(this, 1, 2,WS_CHILD|WS_VISIBLE, AFX_IDW_PANE_FIRST+8) ||
    !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CModelView),
      CSize(0, 0), pContext) ||
    !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLegendView),
      CSize(0, 0), pContext))
  {

    return FALSE;	
  }
  
  CLegendView* pLegendView = (CLegendView*)m_wndSplitter.GetPane(0, 0);
  CModelView * pModelView = (CModelView*)m_wndSplitter.GetPane(0, 1);
  CLegendFrame* pFrame = new CLegendFrame();
  pFrame->ReParent(pLegendView);
  pModelView->AttachLegendView(pLegendView);

  return 0;
}

//##ModelId=3B6549E102F4
void COpenGLSplitView::OnSize(unsigned int nType, int cx, int cy) 
{
  // Do the size of the window
  CView::OnSize(nType, cx, cy);

  //Increase size by 2 all around to remove hide border:
  m_wndSplitter.MoveWindow(-2, -2, cx+2, cy+2);
  m_wndSplitter.RecalcLayout();  
  
}

void COpenGLSplitView::GetColumnInfo(int col, int& cxCur, int& cxMin, bool bOriginal) const
{
  m_wndSplitter.GetColumnInfo(col, cxCur, cxMin);
  if (bOriginal && m_originalCx >= 0)
  cxCur = m_originalCx;
}

void COpenGLSplitView::SetColumnInfo(int col, int cxIdeal, int cxMin)
{
  if (col == 0)
  {
  int oldCx = 0;
  int oldCxMin = 0;
  m_wndSplitter.GetColumnInfo(col, oldCx, oldCxMin);

  if (oldCx > 1)
      m_originalCx = oldCx;
  }

  m_wndSplitter.SetColumnInfo(col, cxIdeal, cxMin);
  m_wndSplitter.RecalcLayout();
}

//##ModelId=3B6549E102D1
void COpenGLSplitView::OnInitialUpdate() 
{
  CView::OnInitialUpdate();

  CRect rect;
  GetWindowRect(&rect);	

  int iOpenGLSplitViewSplitter = ISettings::instance()->getProfileInt(CMainFrame::s_profileHeading, CMainFrame::s_profileOpenGLSplitViewSplitter, rect.Width() * 0.2);

  m_wndSplitter.SetColumnInfo(0, iOpenGLSplitViewSplitter, 0);
  m_wndSplitter.RecalcLayout(); 
  
}

BOOL COpenGLSplitView::OnEraseBkgnd(CDC *)
{
  return TRUE;
}

BOOL COpenGLSplitView::OnMouseWheel(unsigned int nFlags, short zDelta, CPoint pt)
{
  if(m_bRecursionLock)
    return FALSE;

  m_bRecursionLock = true;

  BOOL bRet = (m_wndSplitter.SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(pt.x, pt.y)) != 0);

  m_bRecursionLock = false;

  return bRet;
}

afx_msg void COpenGLSplitView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{ 
   m_wndSplitter.SendMessage(WM_KEYDOWN, nChar, MAKEWPARAM(nRepCnt, nFlags));
}

afx_msg void COpenGLSplitView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{ 
  m_wndSplitter.SendMessage(WM_KEYUP, nChar, MAKEWPARAM(nRepCnt, nFlags));
}
