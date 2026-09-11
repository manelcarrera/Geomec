// DSealectDrawView.cpp : implementation file
//

#include "stdafx.h"
#include "DrawProxy.h"
#include "dsealect.h"
#include "DSealectDrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSealectDrawView

IMPLEMENT_DYNCREATE(CDSealectDrawView, CScrollView)

//##ModelId=3C0DC15C016C
CDSealectDrawView::CDSealectDrawView()
{
}

//##ModelId=3C0DC15C0189
CDSealectDrawView::~CDSealectDrawView()
{
}


BEGIN_MESSAGE_MAP(CDSealectDrawView, CScrollView)
	//{{AFX_MSG_MAP(CDSealectDrawView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSealectDrawView drawing


/////////////////////////////////////////////////////////////////////////////
// CDSealectDrawView diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15C0197
void CDSealectDrawView::AssertValid() const
{
	CView::AssertValid();
}

//##ModelId=3C0DC15C0199
void CDSealectDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDSealectDrawView message handlers

//##ModelId=3C0DC15C0178
int CDSealectDrawView::ViewID()
{
	return m_ViewID;
}


//##ModelId=3C0DC15C0186
void CDSealectDrawView::OnDraw(CDC* pDC) 
{
	// TODO: Add your specialized code here and/or call the base class

	
}

//##ModelId=3C6A14C603A7
void CDSealectDrawView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	SetScrollSizes(MM_TEXT,CSize(100,100)); // ,CSize(0,cy),CSize(0,10));
	
}
