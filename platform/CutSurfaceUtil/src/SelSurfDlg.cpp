// SelSurfDlg.cpp : implementation file
//

#include "stdafx.h"

#include "GeoObjectHandlerBase.h"
#include "SelSurfDlg.h"
#include "SelSurfFormView.h"
#include "SelSurfFrame.h"
#include "SelSurfGLView.h"
#include "resource.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// SelSurfDlg dialog

SelSurfDlg::SelSurfDlg(CWnd *pParent /*=NULL*/, const char *caption)
    : CDialog(IDD_DIALOG_SELECT_SURFACE, pParent), m_Caption(caption) {
  m_surfaces = new CNamedSurfaces;
  slected_surfaces = new CNamedSurfaces;
  not_slected_surfaces = new CNamedSurfaces;

  //{{AFX_DATA_INIT(SelSurfDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void SelSurfDlg::InsertSurface(const geo::ISurface *surf, const char *name) {
  m_surfaces->push_back(STNamedSurface(name, surf));
}

/*virtual*/ SelSurfDlg::~SelSurfDlg() {
  delete m_surfaces;
  delete slected_surfaces;
  delete not_slected_surfaces;
}

BOOL SelSurfDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  if (m_Caption)
    SetWindowText(m_Caption);

  // TODO: push_back extra initialization here

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void SelSurfDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(SelSurfDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SelSurfDlg, CDialog)
//{{AFX_MSG_MAP(SelSurfDlg)
ON_WM_CREATE()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelSurfDlg message handlers

int SelSurfDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  CWaitCursor wait;

  // Because the CFRameWnd needs a window class, we will create
  // a new one. I just copied the sample from MSDN Help.
  // When using it in your project, you may keep CS_VREDRAW and
  // CS_HREDRAW and then throw the other three parameters.
  CString strMyClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, ::LoadCursor(NULL, IDC_ARROW),
                                           (HBRUSH)::GetStockObject(WHITE_BRUSH), ::LoadIcon(NULL, IDI_APPLICATION));

  m_pFrameWnd = new SelSurfFrame;
  BOOL tst = m_pFrameWnd->Create(strMyClass, "", WS_CHILD, CRect(0, 0, 1, 1), this);
  m_pFrameWnd->ShowWindow(SW_SHOW);
  m_pFrameWnd->MoveWindow(0, 0, 300, 300);
  m_pFrameWnd->EnableDocking(CBRS_ALIGN_ANY);

  CSplitterWnd *pSplit = new CSplitterWnd;

  BOOL test = pSplit->CreateStatic(m_pFrameWnd, 1, 3);
  test = pSplit->CreateView(0, 0, RUNTIME_CLASS(SelSurfGLView), CSize(100, 100), NULL);
  test = pSplit->CreateView(0, 1, RUNTIME_CLASS(SelSurfFormView), CSize(100, 100), NULL);
  test = pSplit->CreateView(0, 2, RUNTIME_CLASS(SelSurfGLView), CSize(100, 100), NULL);

  CRect rc;

  this->GetWindowRect(rc);

  pSplit->SetColumnInfo(0, rc.Width() / 3.0 - 10, 10);
  pSplit->SetColumnInfo(1, rc.Width() / 3.0 + 20, 10);
  pSplit->SetColumnInfo(2, rc.Width() / 3.0 - 10, 10);

  pSplit->RecalcLayout();

  m_Form = (SelSurfFormView *)pSplit->GetPane(0, 1);
  SelSurfGLView *From = (SelSurfGLView *)pSplit->GetPane(0, 0);
  SelSurfGLView *To = (SelSurfGLView *)pSplit->GetPane(0, 2);

  m_Form->Init(this, From, To);

  return 0;
}

int SelSurfDlg::SelectedSurfacesCount() const { return slected_surfaces->size(); }

const STNamedSurface &SelSurfDlg::GetSelectedSurfaces(int index) const { return (*slected_surfaces)[index]; }

/*virtual*/ void SelSurfDlg::OnOK() {
  m_Form->GetSelecetedSurfaces(*slected_surfaces);
  m_Form->GetUnSelecetedSurfaces(*not_slected_surfaces);
  CDialog::OnOK();
}

/*virtual*/ void SelSurfDlg::OnCancel() { CDialog::OnCancel(); }

void SelSurfDlg::OnSize(unsigned int nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);

  if (m_pFrameWnd->GetSafeHwnd())
    m_pFrameWnd->MoveWindow(0, 0, cx, cy);
}
