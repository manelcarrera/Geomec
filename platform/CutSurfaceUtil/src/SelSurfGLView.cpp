// WellEditView.cpp : implementation file
//

#include "stdafx.h"

#include "SelSurfGLView.h"
#include "OpenGLScene.h"
#include "openglaxis.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// SelSurfGLView

IMPLEMENT_DYNCREATE(SelSurfGLView, COpenGLView)

SelSurfGLView::SelSurfGLView()
{
}

SelSurfGLView::~SelSurfGLView()
{
}

COpenGLScene* SelSurfGLView::CurrentScene() 
{
  assert(ChildSize() ==1);
  return (COpenGLScene*)&Child(0);
}


BEGIN_MESSAGE_MAP(SelSurfGLView, COpenGLView)
  //{{AFX_MSG_MAP(SelSurfGLView)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_SETCURSOR()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_PALETTECHANGED()
    ON_WM_QUERYNEWPALETTE()
    ON_WM_MOUSEWHEEL()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelSurfGLView drawing

/////////////////////////////////////////////////////////////////////////////
// SelSurfGLView diagnostics

#ifdef _DEBUG
void SelSurfGLView::AssertValid() const
{
  CView::AssertValid();
}

void SelSurfGLView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}
#endif //_DEBUG


void SelSurfGLView::OnSize(unsigned int nType, int cx, int cy) 
{
  COpenGLView::OnSize( nType,  cx,  cy) ;
}


int SelSurfGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

  int ret = COpenGLView::OnCreate(lpCreateStruct);
  m_pScene = new COpenGLScene(false);
  //m_pScene->Axis().AxisText(geo::CVector::Xaxis, "Northing");
  //m_pScene->Axis().AxisText(geo::CVector::Yaxis, "Easting");
  //m_pScene->Axis().AxisText(geo::CVector::Zaxis, "Depth");
  m_pScene->ViewVector(geo::CVector::Xaxis*-1.0);
  m_pScene->UpVector(geo::CVector(0, 0, -1.0));
  m_pScene->DisplayPerspective(FALSE);
  m_pScene->EnableLighting(FALSE);

  m_pScene->ReParent(this);

  return ret;
}


