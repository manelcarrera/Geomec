// treetestview.cpp : implementation file
//

#include "stdafx.h"
#include "TestExe.h"
#include "TestExeDoc.h"
#include "treetestview.h"
#include "MfcTreeLeafObserverTemp.h"
#include "MfcTreeBranchObjectObserverTemp.h"
#include "2DPolyLine.h"
#include "2DDocument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeTestView

IMPLEMENT_DYNCREATE(CTreeTestView, CTreeViewBase)

CTreeTestView::CTreeTestView()
{
}

CTreeTestView::~CTreeTestView()
{
}


BEGIN_MESSAGE_MAP(CTreeTestView, CTreeViewBase)
  //{{AFX_MSG_MAP(CTreeTestView)
    // NOTE - the ClassWizard will add and remove mapping macros here.
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeTestView drawing

void CTreeTestView::OnDraw(CDC* pDC)
{
  CDocument* pDoc = GetDocument();
  // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeTestView diagnostics

#ifdef _DEBUG
void CTreeTestView::AssertValid() const
{
  CView::AssertValid();
}

void CTreeTestView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeTestView message handlers

void CTreeTestView::OnInitialUpdate() 
{

  CTestExeDoc* pDoc = (CTestExeDoc*)GetDocument();
  if(pDoc->Model() == 0)
    return ;

  // Set observers
//	typedef CMfcOpenGLObjectObserverTemp<C2DVertex, COpenGLObjectObserver> T2DSegmentObserver;
  typedef CMfcTreeLeafObserverTemp<IDI_POLY_LINE> TPolyLineObserver;
  typedef CMfcTreeBranchObserverTemp<IDI_POLY_LINES, C2DPolyLine, TPolyLineObserver> TPolyLineContainerObserver;

  new TPolyLineContainerObserver(pDoc->Model()->PolyLines(), GetTreeCtrl(), TVI_ROOT, TVI_LAST);

  typedef CMfcTreeLeafObserverTemp<IDI_BODY> TBodyObserver;
  typedef CMfcTreeBranchObserverTemp<IDI_BODIES, C2DBody, TBodyObserver> TBodyContainerObserver;

  new TBodyContainerObserver(pDoc->Model()->Bodies(), GetTreeCtrl(), TVI_ROOT, TVI_LAST);


  CTreeViewBase::OnInitialUpdate();

}