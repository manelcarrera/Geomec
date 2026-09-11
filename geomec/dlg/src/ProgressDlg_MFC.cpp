#include "stdafx.h"
//
#include "FemAppMainWindow.h"
#include "GeomecStringTable.h"
#include "GlobalMessage.h"
#include "ProgressDlg_MFC.h"
#include "resourceIDS.h"
//
// to cancel run in dsa mode
//
#include "Events.h"
#include "Global.h"

#include <cassert>

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

#if _MSC_VER <= 1200 // we don't support VS6.0 or before anymore
#error This version of the Visual Studio compiler is no longer supported
#endif

CProgressDlg_MFC::CProgressDlg_MFC(bool bCancel, CWnd *pParent)
    : CDialog(IDD_DOUBLE_PROGRESS, pParent), m_bCancelEnable(bCancel), m_pPrimary(nullptr), m_pSecundary(nullptr),
      m_nPreviousValue(-1) {
  Init(1, bCancel);
}

CProgressDlg_MFC::CProgressDlg_MFC(int nJobs, const CString &sPrimary, bool bCancel, CWnd *pParent)
    : CDialog(IDD_DOUBLE_PROGRESS, pParent), m_bCancelEnable(bCancel), m_pPrimary(nullptr), m_pSecundary(nullptr),
      m_strPrimary(sPrimary), m_nPreviousValue(-1) {
  Init(nJobs, bCancel);
}

CProgressDlg_MFC::CProgressDlg_MFC(int nJobs, unsigned int uPrimary, bool bCancel, CWnd *pParent)
    : CDialog(IDD_DOUBLE_PROGRESS, pParent), m_bCancelEnable(bCancel), m_pPrimary(nullptr), m_pSecundary(nullptr),
      m_nPreviousValue(-1) {
  m_strPrimary = getStringTableEntry(uPrimary);
  Init(nJobs, bCancel);
}

void CProgressDlg_MFC::Init(int nJobs, bool bCancel) {
  assert(nJobs > 0);

#ifdef DEBUG_PROGRESS_DLG
  m_count = 0;
#endif

  // Delete old controls
  //
  // mcr 2020-09-16: why delete?
  //
  delete m_pPrimary;
  if (m_pSecundary)
    delete m_pSecundary;

  m_pPrimary = nullptr;
  m_pSecundary = nullptr;

  m_nPreviousValue = -1;

  m_pPrimary = new CProgressCtrl;

  if (nJobs > 1)
    m_pSecundary = new CProgressCtrl;

  // Initialize jobs
  m_nTotalJobs = nJobs;
  m_nCurJob = 0;
}

void CProgressDlg_MFC::SetPrimaryTitle(unsigned int uTitle) {
  assert(m_nTotalJobs > 1);
  m_strPrimary = getStringTableEntry(uTitle);
  UpdateData(FALSE);
}

void CProgressDlg_MFC::SetPrimaryTitle(const CString &sTitle) {
  assert(m_nTotalJobs > 1);
  m_strPrimary = sTitle;
  UpdateData(FALSE);
}

void CProgressDlg_MFC::SetTitle(unsigned int uTitle) {
  if (m_pSecundary)
    m_strSecundary = getStringTableEntry(uTitle);
  else
    m_strPrimary = getStringTableEntry(uTitle);
  UpdateData(FALSE);
}

void CProgressDlg_MFC::SetTitle(const CString &sTitle) {
  if (m_pSecundary)
    m_strSecundary = sTitle;
  else
    m_strPrimary = sTitle;
  UpdateData(FALSE);
}

CProgressDlg_MFC::~CProgressDlg_MFC() {
  delete m_pPrimary;

  if (m_pSecundary)
    delete m_pSecundary;
}

void CProgressDlg_MFC::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProgressDlg_MFC)
  DDX_Control(pDX, IDCANCEL, m_btCancel);

  //}}AFX_DATA_MAP

  // Do we need to disable the cancel button ....
  if (m_bCancelEnable)
    m_btCancel.ShowWindow(SW_SHOW);
  else
    m_btCancel.ShowWindow(SW_HIDE);

  // Exchange of progress controls
  DDX_Control(pDX, IDC_PB_PRIMARY, *m_pPrimary);
  DDX_Text(pDX, IDC_ST_PRIMARY, m_strPrimary);

  if (m_pSecundary) {
    DDX_Control(pDX, IDC_PB_SECUNDARY, *m_pSecundary);
    DDX_Text(pDX, IDC_ST_SECUNDARY, m_strSecundary);
  }

  //	this->RedrawWindow();
}

BEGIN_MESSAGE_MAP(CProgressDlg_MFC, CDialog)
//{{AFX_MSG_MAP(CProgressDlg_MFC)
ON_WM_DESTROY()
// ON_MESSAGE(WM_PROGRESS, OnProgress)
// }}AFX_MSG_MAP
//	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg_MFC message handlers

void CProgressDlg_MFC::OnCancel() {
  if (_g->dsa() && _g->status() == CGlobal::Running) {
    _g->status(CGlobal::Cancelling);
    _e->broadcast(CancelRun);
  } else {
    if (!m_bCancelEnable)
      return;
    //
    // FIXME: in dsa mode MFC dlg is behing and process goes on
    //
    if (_m()->msg(IDS_CANCEL_PROGRESS, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES) {
      m_bCancel = true;

      CDialog::OnCancel();
    }
  }
}

void CProgressDlg_MFC::NextJob(const CString &sTitle) {
  m_nCurJob++;

  assert(m_nCurJob <= m_nTotalJobs);

  m_strSecundary = sTitle;

  UpdateData(false);
}

void CProgressDlg_MFC::NextJob(unsigned int uTitle) {
  CString sTitle;
  sTitle = getStringTableEntry(uTitle);
  NextJob(sTitle);
}

void CProgressDlg_MFC::OnDestroy() {
  CDialog::OnDestroy();

  CWnd *parent = (m_pParentWnd != NULL ? m_pParentWnd : FemAppGetMainWnd());

  if (m_bOriginalState)
    parent->EnableWindow(m_bOriginalState);

  parent->SetForegroundWindow();
  parent->BringWindowToTop();
}

void CProgressDlg_MFC::DoMessagePump() {
  BOOL bIdle = TRUE;
  LONG lIdleCount = 0;
  HWND hWndParent = ::GetParent(m_hWnd);
  BOOL bShowIdle = TRUE;

  MSG *pMsg = &AfxGetThreadState()->m_msgCur;

  if (!::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE))
    return;

  // pump messages while available
  do {
    if (!ContinueModal())
      break;

    // pump message, but quit on WM_QUIT
    if (!AfxGetThread()->PumpMessage()) {
      AfxPostQuitMessage(0);
      return;
    }

    // show the window when certain special messages rec'd
    if (bShowIdle && (pMsg->message == 0x118 || pMsg->message == WM_SYSKEYDOWN)) {
      ShowWindow(SW_SHOWNORMAL);
      UpdateWindow();
      bShowIdle = FALSE;
    }

    if (!ContinueModal())
      break;

    // reset "no idle" state after pumping "normal" message
    if (AfxGetThread()->IsIdleMessage(pMsg))
      break;

  } while (::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE));
}

void CProgressDlg_MFC::StartDialog() {
  CWnd *parent = (m_pParentWnd != NULL ? m_pParentWnd : FemAppGetMainWnd());

  m_bOriginalState = parent->IsWindowEnabled();
  parent->EnableWindow(FALSE);
  m_bCancel = false;

  m_nCurJob = 0;

  if (m_pSecundary)
    Create(IDD_DOUBLE_PROGRESS);
  else
    Create(IDD_SINGLE_PROGRESS);

  this->ShowWindow(SW_SHOW);
}

bool CProgressDlg_MFC::OnProgress(int p) {

  UpdateWindow();

  if (!_g->dsa() || (_g->dsa() && _g->status() == CGlobal::Idle))
    DoMessagePump();

  if (m_pSecundary) {
    // Set Primary Bar
    m_pPrimary->SetRange32(0, m_nTotalJobs * 1000);
    m_pPrimary->SetPos(p + (m_nCurJob - 1) * 1000);

    // Set Secundary Bar
    m_pSecundary->SetRange(0, 1000);
    m_pSecundary->SetPos((int)p);
  } else {
    // Set Primary Bar only
    m_pPrimary->SetRange(0, 1000);
    m_pPrimary->SetPos(p);
  }

  m_nPreviousValue = p;

  // UpdateWindow();

  //
  // FIXME: just to test
  //
#ifdef DEBUG_PROGRESS_DLG
  if (_g->running()) {
    // static int count=0;
    m_count++;
    if (m_count == 10) {
      // count=0;
      OnCancel();
    }
  }
#endif
  //
  //
  //

  if (m_bCancel)
    return false;

  return true;
}

bool CProgressDlg_MFC::SetProgress(int perc) {
  //	assert(perc >= 0 && perc <= 100);
  if (m_nPreviousValue != (perc * 10))
    return OnProgress(perc * 10) == 1;

  return true;
}

bool CProgressDlg_MFC::SetProgress(int nTotalSteps, int nStep) {
  //
  // mcr 2020-09-30, why by sudden is this hapenning?
  //
  if (!nTotalSteps)
    return true;

  int nProgress = nStep / (nTotalSteps * 0.001);
  if (nProgress != m_nPreviousValue)
    return OnProgress(nProgress) == 1;

  return true;
}