// CutSurfaceDlg.cpp : implementation file
//

#include "stdafx.h"

#include "CutSurfaceDlg.h"
#include <afxcview.h>
#include "GLView.h"
#include "resource.h"
#include "CutSurfaceFrame.h"
#include "EditForm.h"
#include "TriaIntersecBox.h"
#include "CutsurfaceTreeView.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceDlg dialog

CCutSurfaceDlg::CCutSurfaceDlg(const geo::CBox& box,
                 CDoubleQuantity::UNIT unit /*=CDoubleQuantity::SI_UNIT*/,
                 CWnd* pParent /*=NULL*/)
  : CDialog(IDD_SURFACE_CUT_DLG, pParent), m_pBox(&box)
{
  m_OrgSurfaces = new CNamedSurfaces;

  m_allow_edit_box = false;
  m_unit=unit;
  m_pIntersec=0;

}


void CCutSurfaceDlg::InsertSurface(const geo::ISurface* surf,const std::string& name )
{
  m_OrgSurfaces->push_back(STNamedSurface(name,surf));// 
}


CCutSurfaceDlg::CCutSurfaceDlg(CDoubleQuantity::UNIT unit, CWnd* pParent /*=NULL*/)
  : CDialog(IDD_SURFACE_CUT_DLG, pParent), m_pBox(0)
{

  m_OrgSurfaces = new CNamedSurfaces;

  m_unit=unit;
  m_pIntersec=0;
  m_allow_edit_box=true;

  //{{AFX_DATA_INIT(CCutSurfaceDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


/*virtual*/ CCutSurfaceDlg::~CCutSurfaceDlg()
{
  delete m_OrgSurfaces;
}


void CCutSurfaceDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CCutSurfaceDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCutSurfaceDlg, CDialog)
  //{{AFX_MSG_MAP(CCutSurfaceDlg)
  ON_WM_CREATE()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCutSurfaceDlg message handlers

int CCutSurfaceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  m_Height=0;

  // Because the CFRameWnd needs a window class, we will create
  // a new one. I just copied the sample from MSDN Help.
  // When using it in your project, you may keep CS_VREDRAW and
  // CS_HREDRAW and then throw the other three parameters.
  CString strMyClass = AfxRegisterWndClass(CS_VREDRAW |
                         CS_HREDRAW,
                         ::LoadCursor(NULL, IDC_ARROW),
                         (HBRUSH) ::GetStockObject(WHITE_BRUSH),
                         ::LoadIcon(NULL, IDI_APPLICATION));

  m_pFrameWnd = new CCutSurfaceFrame;
  BOOL tst = m_pFrameWnd->Create(strMyClass,"", WS_CHILD,CRect(0,0,1,1), this);
  m_pFrameWnd->ShowWindow(SW_SHOW);
  m_pFrameWnd->MoveWindow(0,0,300,300);
  m_pFrameWnd->EnableDocking(CBRS_ALIGN_ANY);

  // and finally, create the splitter with the frame as
  // the parent

  BOOL test =m_cSplitter.CreateStatic(m_pFrameWnd,1, 2);
  test = m_cSplitter.CreateView(0,1, RUNTIME_CLASS(CGLView),CSize(100,100), NULL);
  test = m_cSplitter.CreateView(0,0, RUNTIME_CLASS(CFrameWnd),CSize(100,100), NULL);

  
  CFrameWnd* frame = (CFrameWnd*)m_cSplitter.GetPane( 0, 0 );
  CSplitterWnd* pSplit = new CSplitterWnd;
  test =pSplit->CreateStatic(frame,2, 1);

  test = pSplit->CreateView(0,0, RUNTIME_CLASS(CEditForm),CSize(100,100), NULL);
  test = pSplit->CreateView(1,0, RUNTIME_CLASS(CCutsurfaceTreeView),CSize(100,100), NULL);

  m_cSplitter.SetColumnInfo(0, 260, 10);
  pSplit->SetRowInfo(0, 500, 10);
      

  CEditForm* form = (CEditForm*)pSplit->GetPane( 0, 0 );
  CCutsurfaceTreeView* treeview = (CCutsurfaceTreeView*)pSplit->GetPane( 1, 0 );

  COpenGLView* pView =(COpenGLView*)m_cSplitter.GetPane( 0, 1 );

  m_pFrameWnd->Init(treeview ,pView,this,m_unit);

  form->Init(m_pFrameWnd, this, m_unit, m_pBox == 0, m_pBox);

  if (!m_3DViewToolBar.CreateEx(m_pFrameWnd) ||
    !m_3DViewToolBar.LoadToolBar(IDR_TOOLBAR_OPENGL_VIEW))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }
  m_3DViewToolBar.EnableDocking(CBRS_ALIGN_RIGHT); //CBRS_ALIGN_ANY);
  m_pFrameWnd->DockControlBar(&m_3DViewToolBar,AFX_IDW_DOCKBAR_RIGHT );
  m_3DViewToolBar.ShowWindow(TRUE);

  return 0;
}

const CString& CCutSurfaceDlg::BaseName() const
{
  return m_BaseName;
}


void CCutSurfaceDlg::OnSize(unsigned int nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);
  if(m_pFrameWnd->GetSafeHwnd())
    m_pFrameWnd->MoveWindow(0,m_Height,cx,cy-m_Height);
  
}

/*virtual*/ void CCutSurfaceDlg::OnOK()
{
  CDialog::OnOK();
}

/*virtual*/ void CCutSurfaceDlg::OnCancel()
{
  CDialog::OnCancel(); 
}


const CNamedSurfaces& CCutSurfaceDlg::OrgSurfaces() const
{
  return *m_OrgSurfaces;
}

const std::vector<SIDE_ID> CCutSurfaceDlg::GetSides() const
{
  return  m_pIntersec->GetSides(); 
}

const geo::CTriSurface* CCutSurfaceDlg::GetSideSurfaces(SIDE_ID SideID) const
{
  return  m_pIntersec->GetSideSurface(SideID);
}

const std::map <SIDE_ID, geo::CTriSurface*>&
  CCutSurfaceDlg::getSideSurfaces() const
{
  return m_pIntersec->getSideSurfaces();
}


int CCutSurfaceDlg::CutSurfacesCount() const
{
  return m_pIntersec->CutSurfacesCount(); 
}

const STNamedSurface& CCutSurfaceDlg::GetCutSurface(int index) const
{
  return m_pIntersec->GetCutSurfaces(index);
}

const std::vector <STNamedSurface>& CCutSurfaceDlg::getCutSurfaces() const
{
  return m_pIntersec->getCutSurfaces();
}


CTriaIntersecBox* CCutSurfaceDlg::CreateNewIntersec(const geo::CBox& box)
{
  if(m_pIntersec)
    delete m_pIntersec;

  m_pIntersec = new CTriaIntersecBox(box);
  return m_pIntersec;
}


