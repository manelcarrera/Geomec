// WellEditDlg.cpp : implementation file
//

#include "stdafx.h"

#include "SpreadView.h"
#include "WellEditDlg.h"
#include "WellEditFrame.h"
#include "WellEditView.h"
#include "wellpathbase.h"
// #include "FvGocadWellFile.h"
#include "GlobalMessage.h"
#include "IProgressBase.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellEditDlg dialog

CWellEditDlg::CWellEditDlg(const well::CWellPathBase &wellpath, QU::UNIT us, CWnd *pParent /*=NULL*/)
    : CDialog(CWellEditDlg::IDD, pParent) {
  m_Us = us;
  m_Height = 0;

  m_doc = new well::CWellPathBaseDocument;
  m_doc->create();

  m_pWellPath = new well::CWellPathBase();
  m_pWellPath->create(m_doc);
  m_pWellPath->HardCopyDefPointList(wellpath);

  //{{AFX_DATA_INIT(CWellEditDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

/*virtual*/ CWellEditDlg::~CWellEditDlg() { m_doc->destroy(); }

void CWellEditDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CWellEditDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWellEditDlg, CDialog)
//{{AFX_MSG_MAP(CWellEditDlg)
ON_WM_CREATE()
ON_WM_SIZE()
ON_COMMAND(IDR_MENU_WE_APPLY, OnMenuApply)
ON_COMMAND(IDR_MENU_WE_CLOSE, OnMenuClose)
ON_COMMAND(IDR_MENU_WE_OPEN, OnMenuOpen)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWellEditDlg message handlers

int CWellEditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  // Initialize a context for the view.
  CCreateContext ccc;
  ccc.m_pNewViewClass = RUNTIME_CLASS(CSpreadView);
  ccc.m_pCurrentDoc = NULL;
  ccc.m_pNewDocTemplate = NULL;
  ccc.m_pLastView = NULL;
  ccc.m_pCurrentFrame = NULL;

  // Because the CFRameWnd needs a window class, we will create
  // a new one. I just copied the sample from MSDN Help.
  // When using it in your project, you may keep CS_VREDRAW and
  // CS_HREDRAW and then throw the other three parameters.
  CString strMyClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, ::LoadCursor(NULL, IDC_ARROW),
                                           (HBRUSH)::GetStockObject(WHITE_BRUSH), ::LoadIcon(NULL, IDI_APPLICATION));

  // Create the frame window with "this" as the parent
  m_pFrameWnd = new CWellEditFrame;
  m_pFrameWnd->Create(strMyClass, "", WS_CHILD, CRect(0, 0, 1, 1), this);
  m_pFrameWnd->ShowWindow(SW_SHOW);
  m_pFrameWnd->MoveWindow(0, 0, 300, 300);
  m_pFrameWnd->EnableDocking(CBRS_ALIGN_ANY);

  CCreateContext ccc2;
  ccc2.m_pNewViewClass = RUNTIME_CLASS(CWellEditView);
  ccc2.m_pCurrentDoc = NULL;
  ccc2.m_pNewDocTemplate = NULL;
  ccc2.m_pLastView = NULL;
  ccc2.m_pCurrentFrame = NULL;

  // and finally, create the splitter with the frame as
  // the parent
  BOOL test = m_cSplitter.CreateStatic(m_pFrameWnd, 1, 2);
  test = m_cSplitter.CreateView(0, 0, RUNTIME_CLASS(CSpreadView), CSize(100, 100), &ccc);
  test = m_cSplitter.CreateView(0, 1, RUNTIME_CLASS(CWellEditView), CSize(100, 100), &ccc2);

  if (!m_3DViewToolBar.CreateEx(m_pFrameWnd) || !m_3DViewToolBar.LoadToolBar(IDR_TOOLBAR_WELL_EDIT)) {
    TRACE0("Failed to create toolbar\n");
    return -1; // fail to create
  }
  m_3DViewToolBar.EnableDocking(CBRS_ALIGN_RIGHT); // CBRS_ALIGN_ANY);
  m_pFrameWnd->DockControlBar(&m_3DViewToolBar, AFX_IDW_DOCKBAR_RIGHT);
  m_3DViewToolBar.ShowWindow(TRUE);

  if (!m_SpreadDlgbar.Create(m_pFrameWnd, IDD_DIALOGBAR_SPEAD_EDIT, CBRS_TOP, 0)) {
    TRACE0("Failed to create toolbar\n");
    return -1; // fail to create
  }

  m_SpreadDlgbar.EnableDocking(CBRS_ALIGN_TOP); // CBRS_ALIGN_ANY);
  m_pFrameWnd->DockControlBar(&m_SpreadDlgbar, AFX_IDW_DOCKBAR_TOP);
  m_SpreadDlgbar.ShowWindow(TRUE);

  // give units definition and wellpath to view
  m_pSpreadView = (CSpreadView *)m_cSplitter.GetPane(0, 0);
  m_pSpreadView->Init(*m_pWellPath, m_Us);

  m_pWellEditView = (CWellEditView *)m_cSplitter.GetPane(0, 1);
  m_pWellEditView->Init(*m_pWellPath, m_Us);

  m_pFrameWnd->Init(m_pWellEditView, this);

  CMenu *pMenu = new CMenu();
  test = pMenu->LoadMenu(IDR_MENU_WELL_EDIT);
  test = SetMenu(pMenu);
  DrawMenuBar();

  return 0;
}

BOOL CWellEditDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // Move the frame window
  CRect cRect;
  GetClientRect(&cRect);
  m_pFrameWnd->MoveWindow(0, m_Height, cRect.Width(), cRect.Height() - m_Height);
  m_pFrameWnd->ShowWindow(SW_SHOW);

  // equal width for both panes
  m_cSplitter.SetColumnInfo(0, 2 * cRect.Width() / 3, 100);
  m_cSplitter.SetColumnInfo(1, 1 * cRect.Width() / 3, 100);
  m_cSplitter.RecalcLayout();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CWellEditDlg::OnSize(unsigned int nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);

  // Move the frame window
  m_pFrameWnd->MoveWindow(0, m_Height, cx, cy - m_Height);
  // m_pFrameWnd->ShowWindow(SW_SHOW);
}

well::CWellPathBase &CWellEditDlg::WellPath() const { return *m_pWellPath; }

void CWellEditDlg::SetAzimuth(bool azimuth) { m_pSpreadView->SetAzimuth(azimuth); }

void CWellEditDlg::SetType(DEF_TYPE type) { m_pSpreadView->SetDefType(type); }

/*virtual*/ void CWellEditDlg::OnOK() {
  if (m_pSpreadView->GetNumberOfValidRows() <= 1) {
    CString msg;
    msg = "Need at least two valid rows for creating a deviated wellpath?";
    msg = msg + "\nDo you want to exit anyway?";
    int ret = _m()->msg(msg, MB_YESNO);

    if (IDYES == ret) {
      CDialog::OnCancel();
      return;
    } else
      return;
  }

  CDialog::OnOK();
}

/*virtual*/ void CWellEditDlg::OnCancel() {

  if (m_pFrameWnd->IsDirty() || m_pSpreadView->IsDirty()) {
    int ret = IDYES;
    ret = _m()->msg("Apply changes?", MB_YESNOCANCEL);

    if (IDCANCEL == ret)
      return;
    else if (IDYES == ret)
      OnOK();
    else
      CDialog::OnCancel();

    return;
  }

  CDialog::OnCancel();
}

void CWellEditDlg::OnMenuApply() { OnOK(); }

void CWellEditDlg::OnMenuClose() { OnCancel(); }

void CWellEditDlg::OnMenuOpen() {
  /*	CString strFilter = "Data Exchange Files (*.dxd)|*.dxd| All Files |*.*||";

    CFileDialog dlg(TRUE, "dex", "", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
    dlg.m_ofn.Flags |= OFN_EXPLORER;


    if(dlg.DoModal() ==IDOK)
    {


      CGoCadWellFile f;
      QString fname(dlg.GetPathName());
      bool t = f.Open(fname,IProgressBase());

      int size = f.pointSize(0);
      assert(f.wellSize()==1);

      QValueVector<geo::CPoint> vcPoint;
      //vcPoint.resize(size);
      //vcTmd.resize(size);

      for(int i=0 ; i< size;i++)
      {
        const geo::IPoint& p = f.pointAt(0,i);
        vcPoint.push_back( geo::CPoint( p ) );
      }
      m_pWellPath->InitFromPointArray( vcPoint );
      m_pSpreadView->UpdateCells();
      m_pWellEditView->UpdateWindow();

    }*/
}
