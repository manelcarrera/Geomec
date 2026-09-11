// TestViewExeView.cpp : implementation of the CTestViewExeView class
//

#include "stdafx.h"
#include "TestViewExe.h"

#include "TestViewExeDoc.h"
#include "TestViewExeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView

IMPLEMENT_DYNCREATE(CTestViewExeView, CView)

BEGIN_MESSAGE_MAP(CTestViewExeView, CView)
	//{{AFX_MSG_MAP(CTestViewExeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_LBUTTONDBLCLK()
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView construction/destruction

CTestViewExeView::CTestViewExeView()
{
	// TODO: add construction code here

}

CTestViewExeView::~CTestViewExeView()
{
}

BOOL CTestViewExeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView drawing


/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView printing

BOOL CTestViewExeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTestViewExeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTestViewExeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView diagnostics

#ifdef _DEBUG
void CTestViewExeView::AssertValid() const
{
	CView::AssertValid();
}

void CTestViewExeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestViewExeDoc* CTestViewExeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestViewExeDoc)));
	return (CTestViewExeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeView message handlers

void CTestViewExeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CurrentScene(GetDocument()->Scene());

}
