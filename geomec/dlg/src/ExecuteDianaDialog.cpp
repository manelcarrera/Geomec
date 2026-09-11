// ExecuteDianaDialog.cpp : implementation file
//

#include "ExecuteDianaDialog.h"
#include "DianaX.h"
#include "stdafx.h"
// #include "DianaXCtl.h"

#include <direct.h>

#include "DianaStartUp.h"
#include "GlobalMessage.h"
#include "IDCAnalysisStatus.h"
#include "IDCDianaRunner.h"
#include "lbcx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CExecuteDianaDialog dialog

#include "Global.h" // to close dlg on btn push

CExecuteDianaDialog::CExecuteDianaDialog(CWnd *pParent /*=NULL*/)
    : CDialog(CExecuteDianaDialog::IDD, pParent), m_exe_diana_signals(CDianaStartUp::instance(), this) {
  //{{AFX_DATA_INIT(CExecuteDianaDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_bStop = false;
  m_status = Running; // is not but better to init the var
  m_bAutoCloseDialog = FALSE;
  m_bAlwaysAutoCloseDialog = FALSE;

  // CDianaXCtrl

  m_bShowDialog = TRUE;
  m_bCalculationSucceeded = FALSE;
  m_nTotalMessages = 0;
  m_nTotalAppSpcMessages = 0;
  m_nCurrentMessages = 0;
  m_nCurrentAppSpcMessages = 0;
  m_bDisplayStopMessage = TRUE;
  m_bDisplayDefaultMessages = TRUE;
  m_strDialogCaption = "Calculating";
  m_bSizing = TRUE;
  m_bProgressIsLifesign = FALSE;
  m_bChildIsRunning = FALSE;

  // not needed, done in base class
  m_runner = 0;
}

CExecuteDianaDialog::~CExecuteDianaDialog() {
  // CDianaXCtrl
}

void CExecuteDianaDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CExecuteDianaDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExecuteDianaDialog, CDialog)
//{{AFX_MSG_MAP(CExecuteDianaDialog)
ON_BN_CLICKED(IDC_BUT_STOP, OnButStop)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExecuteDianaDialog message handlers

//
// run logic: 'AddMessage' and 'AddWarning' use 'ProcessStatus' imnplemented in the base class 'IExecuteDianaDialog'
// (DGlue lib)
//
void CExecuteDianaDialog::AddMessage(QString Message) {
  if (!Message.length())
    return;
  if (Message[Message.length() - 1] == '\n')
    Message = Message.left(Message.length() - 1);

  //
  // run logic
  //
  ProcessStatus(Message);

  CListBox *ListMessages;

  if (!(ListMessages = (CListBox *)GetDlgItem(IDC_LIST_MSG)))
    return;

  ListMessages->AddString(Message.toStdString().c_str());
  DisplayLastItem(ListMessages);
  if (!m_bProgressIsLifesign)
    StepProgressBar();
  CallBack();
}

void CExecuteDianaDialog::AddWarning(QString Warning) {
  if (!Warning.length())
    return;
  if (Warning[Warning.length() - 1] == '\n')
    Warning = Warning.left(Warning.length() - 1);

  //
  // run logic
  //
  ProcessStatus(Warning);

  //	m_bWarning = TRUE;
  //	MoveControls();

  CListBox *ListWarnings;
  if (!(ListWarnings = (CListBox *)GetDlgItem(IDC_LIST_WARN)))
    return;

  ListWarnings->AddString(Warning.toStdString().c_str());
  DisplayLastItem(ListWarnings);
  CallBack();
}

void CExecuteDianaDialog::DisplayError(QString Error) {
  if (Error[Error.length() - 1] == '\n')
    Error = Error.left(Error.length() - 1);

  MessageBox(Error.toStdString().c_str(), "Error", MB_OK | MB_ICONEXCLAMATION);
}

void CExecuteDianaDialog::DisplayLastItem(CListBox *ListBox) {
  /*
    CRect Rect;
    ListBox->GetClientRect( &Rect );

    int nDisplayableItems = ( Rect.bottom - Rect.top ) / ListBox->GetItemHeight();
    int nItems = ListBox->GetCount();
    ListBox->SetTopIndex( nItems - nDisplayableItems );
  */
  if (!ListBox->GetCount())
    return;
  ListBox->SetTopIndex(ListBox->GetCount() - 1);
}

//
// mcr 2020-08-28: not really a callback, just a local update method
//
// PeekMessage:
//		Dispatches incoming sent messages, checks the thread message queue for a posted message, and retrieves the message
//(if any exist). 		Return value:	If a message is available, the return value is nonzero.
//
int CExecuteDianaDialog::CallBack() {

  bool bIdle = TRUE;
  LONG lIdleCount = 0;
  HWND hWndParent = ::GetParent(m_hWnd);
  bool bShowIdle = TRUE;
#if _MSC_VER >= 1400 // VS2005
  MSG *pMsg = &AfxGetThreadState()->m_msgCur;
#else // VS60
  MSG *pMsg = &AfxGetThread()->m_msgCur;
#endif

  if (m_bProgressIsLifesign)
    StepProgressBar();

  // mantis #2581
  // wedx 06032008: return m_bStop when unable to peek message
  // so window is closed even when it's not in the foreground
  if (!::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE))
    return m_bStop;

  // pump messages while available
  do {
    if (!ContinueModal())
      break;

    // pump message, but quit on WM_QUIT
    if (!AfxGetThread()->PumpMessage()) {
      AfxPostQuitMessage(0);
      return 1; // return 1 to make DIANA quit
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

  return m_bStop;
}

//
// run logic
//
void CExecuteDianaDialog::OnButStop() {
  bool bHaveResults = false, bCalcResult = false;

  if (m_status == Finished) {
    _chdir(m_pchOldCwd);
    m_bStop = true;

    if (_g->dsa()) // FIXME: mcr 2020-09-29, why not also in non-dsa ???
      CloseDialog();

    return;
  } else // Running
  {
    bool b_quit = true;
    if (m_bDisplayStopMessage) {
      //
      // FIXME: process continues and confirmation button can't be pushed
      //
      b_quit = _m()->msg(IDP_CONFIRM_STOP, MB_YESNO | MB_ICONQUESTION) == IDYES;
    }

    if (b_quit) {
      m_bStop = true;
      //
      // if we have a result, we assume that there is no actual cancel
      // but a stop button click to close the dialog when we haven't detected that Diana finished
      // This may go wrong when a user wants to cancel across multiple branches, etc:
      // user will have to cancel again in that case
      //
      // FIXME: in dsa this is per stage so even with results execution has to be cancelled
      //
      if (m_runner) {
        //
        // FIXME:
        //
        // m_runner->GetResultInfo(bHaveResults, bCalcResult);
        // if (!bCalcResult)
        m_runner->Cancel();
      } else // dsa ... if( _g->dsa())
      {
        _g->status(CGlobal::Cancelling);
        _e->broadcast(CancelRun);
        //
        // this freezes teh gui
        //
        // CloseDialog();
        //
        // it freezes either
        //
        // end(1);
        //

        //
        // next works Ok ... could be removed as dlg is deleted in main work flow when it receives the CancelRun event
        //
        EndDialog(0);
        //
      }
      //
      //
      //
    }
  }
}

//
// run logic
//
void CExecuteDianaDialog::Finish() {
  m_status = Finished;

  CWnd *StopButton;
  if (!(StopButton = GetDlgItem(IDC_BUT_STOP)))
    return;

  if (m_runner) {
    if (!m_bCalculationSucceeded) {
      bool bHaveResults = false;
      m_runner->GetResultInfo(bHaveResults, m_bCalculationSucceeded);
    }
  } else // dsa, dlg is not diana wrapper, but only a dlg
  {
  }

  if ((m_bAutoCloseDialog && m_bCalculationSucceeded) || m_bAlwaysAutoCloseDialog) {
    _chdir(m_pchOldCwd);
    m_bStop = true;
    StopButton->EnableWindow(FALSE); // we don't want the user to press stop anymore
    return;
  }

  StopButton->SetWindowText("OK");
}

void CExecuteDianaDialog::Start(bool itemsValid) {
  m_status = Running;

  if (itemsValid) {
    CWnd *StopButton;
    if (!(StopButton = GetDlgItem(IDC_BUT_STOP)))
      return;

    m_bStop = false;
    StopButton->EnableWindow(TRUE);

    StopButton->SetWindowText("Stop");
  }
}

// CDianaXCtrl

void CExecuteDianaDialog::OnDraw(CDC *pdc, const CRect &rcBounds, const CRect &rcInvalid) {
  // Insert the Control's Bitmap in the control
  CBitmap bitmap;
  CDC dcDisplayMemory;
  bitmap.LoadBitmap(IDB_CONTROL);
  dcDisplayMemory.CreateCompatibleDC(pdc);
  dcDisplayMemory.SelectObject(&bitmap);
  pdc->BitBlt(0, 0, 48, 48, &dcDisplayMemory, 0, 0, SRCCOPY);

  // Do not allow resizing
  if (m_bSizing) {
    //		SetControlSize( 48, 48 );
    m_bSizing = FALSE;
  }
}

/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::AboutBox - Display an "About" box to the user

void CExecuteDianaDialog::AboutBox() {
  CDialog dlgAbout(IDD_ABOUTBOX_DIANAX);
  dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl message handlers

bool CExecuteDianaDialog::GetShowDialog() { return m_bShowDialog; }
void CExecuteDianaDialog::SetShowDialog(bool bNewValue) { m_bShowDialog = bNewValue; }

void CExecuteDianaDialog::DoFireOnCallback(int *bStop) {
  unsigned long long bUserStop = FALSE;
  bStop = (int *)bUserStop;
}

bool CExecuteDianaDialog::SetEnvironmentVar(QString VarName, QString Value) {
  int rv = vDiSetenv("%s=%s", VarName.toStdString().c_str(), Value.toStdString().c_str());
  TRACE("DianaX: Set variable %s to value %s\n", VarName.toStdString().c_str(), Value.toStdString().c_str());
  return (rv == 0);
}

short CExecuteDianaDialog::init() {
  // doe not work if this is set to false (htg/gra)
  m_bShowDialog = true;

  Start(m_hWnd != NULL);
  m_bCalculationSucceeded = FALSE;

  if (m_bShowDialog) {
    if (m_hWnd == NULL) {
      Create(IDD_DIANAX);
      SetWindowText(m_strDialogCaption.toStdString().c_str());
    }

    ShowWindow(SW_SHOW);
    CenterWindow();

    if (m_nTotalMessages < 1 && m_nTotalAppSpcMessages < 1 && !m_bProgressIsLifesign) {
      CWnd *ProgressBar;
      CWnd *ProgressBarText;

      if (!(ProgressBar = GetDlgItem(IDC_PROGRESS)))
        return -4;
      if (!(ProgressBarText = GetDlgItem(IDC_TXT_PROGRESS)))
        return -4;

      ProgressBar->ShowWindow(SW_HIDE);
      ProgressBarText->ShowWindow(SW_HIDE);
    } else {
      CProgressCtrl *ProgressBar;
      CWnd *ProgressBarText;

      if (!(ProgressBar = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS)))
        return -4;
      if (!(ProgressBarText = GetDlgItem(IDC_TXT_PROGRESS)))
        return -4;

      if (m_bProgressIsLifesign) {
        ProgressBar->SetRange(0, 100);
        ProgressBar->SetStep(5);
        ProgressBarText->SetWindowText("");
      } else {
        ProgressBar->SetRange(0, 100);
        ProgressBar->SetStep(1);
        ProgressBarText->SetWindowText("0%");
      }
      ProgressBar->SetPos(0);
      m_nCurrentMessages = 0;
      m_nCurrentAppSpcMessages = 0;
    }
  }

  m_bChildIsRunning = TRUE;

  return 0;
}

//
// it waits till the user closes the dlg
//
short CExecuteDianaDialog::end(short iret) {
  m_bChildIsRunning = FALSE;

  if (iret == 0) // Ok
  {
    while (m_bShowDialog) {
      if (CallBack())
        break;
      Sleep(100);
    }
  }

  CProgressCtrl *ProgressBar;
  if (!(ProgressBar = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS))) {
    int nLower, nUpper;
    ProgressBar->GetRange(nLower, nUpper);
    ProgressBar->SetPos(nUpper);
  }

  CloseDialog();

  _chdir(m_pchOldCwd);
  return iret; // 0 is Ok
}

//
// run logic
//
short CExecuteDianaDialog::ExecuteDiana() {
  short res = init();
  if (res)
    return res; // -4

  CDianaStartUp *dsu = CDianaStartUp::instance();

  dsu->var(CDianaStartUp::DISPLAY, m_bDisplayDefaultMessages ? "yes" : "");
  dsu->var(CDianaStartUp::USER_LEADER, m_strUserLeader);
  dsu->var(CDianaStartUp::USER_TRAILER, m_strUserTrailer);

  int iret = dsu->RunDiana();

  return end(iret);
}

void CExecuteDianaDialog::ExecuteDianaCleanup() { CDianaStartUp::instance()->RunDianaCleanup(); }

void CExecuteDianaDialog::StepProgressBar() {
  if (m_nTotalMessages < 1 && m_nTotalAppSpcMessages < 1 && !m_bProgressIsLifesign)
    return;

  CProgressCtrl *ProgressBar;
  CWnd *ProgressBarText;

  if (!(ProgressBar = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS)))
    return;
  if (!(ProgressBarText = GetDlgItem(IDC_TXT_PROGRESS)))
    return;

  assert(::IsWindow(GetDlgItem(IDC_PROGRESS)->GetSafeHwnd()));

  if (m_bProgressIsLifesign) {
    if (m_bChildIsRunning) {
      if (ProgressBar->StepIt() >= 100)
        ProgressBar->SetPos(0);
    } else {
      ProgressBar->SetPos(100);
    }
  } else {
    long current = 0;
    long total = 0;
    if (m_nTotalMessages > 0) {
      total = m_nTotalMessages;
      current = m_nCurrentMessages;
    } else {
      assert(m_nTotalAppSpcMessages > 0);
      total = m_nTotalAppSpcMessages;
      current = m_nCurrentAppSpcMessages;
    }

    int pos = 100. * (double)current / (double)total;

    ProgressBar->SetPos(pos);

    CString txt;
    txt.Format("%d%%", pos);
    ProgressBarText->SetWindowText(txt);
  }
}

long CExecuteDianaDialog::GetNumberOfMessages() { return m_nTotalMessages; }
void CExecuteDianaDialog::SetNumberOfMessages(long nNewValue) {
  if (nNewValue > 0)
    m_nTotalMessages = nNewValue;
}

long CExecuteDianaDialog::GetNumberOfAppSpcMessages() { return m_nTotalAppSpcMessages; }

void CExecuteDianaDialog::SetNumberOfAppSpcMessages(long n) {
  if (n >= 0)
    m_nTotalAppSpcMessages = n;
}

bool CExecuteDianaDialog::GetDisplayStopMessage() { return m_bDisplayStopMessage; }
void CExecuteDianaDialog::SetDisplayStopMessage(bool bNewValue) { m_bDisplayStopMessage = bNewValue; }

bool CExecuteDianaDialog::GetShowDefaultMessages() { return m_bDisplayDefaultMessages; }
void CExecuteDianaDialog::SetShowDefaultMessages(bool bNewValue) { m_bDisplayDefaultMessages = bNewValue; }

bool CExecuteDianaDialog::SetUserMagic(QString Leader, QString Trailer) {
  m_strUserLeader = Leader;
  m_strUserTrailer = Trailer;

  return TRUE;
}

void CExecuteDianaDialog::SetDialogCaption(QString val) { m_strDialogCaption = val; }
QString CExecuteDianaDialog::GetDialogCaption() { return m_strDialogCaption; }

void CExecuteDianaDialog::SetAlwaysAutoCloseDialog(bool val) { m_bAlwaysAutoCloseDialog = val; }
bool CExecuteDianaDialog::GetAlwaysAutoCloseDialog() { return m_bAlwaysAutoCloseDialog; }

void CExecuteDianaDialog::SetAutoCloseDialog(bool val) { m_bAutoCloseDialog = val; }
bool CExecuteDianaDialog::GetAutoCloseDialog() { return m_bAutoCloseDialog; }

void CExecuteDianaDialog::OnSize(unsigned int nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_bSizing = TRUE;
}

bool CExecuteDianaDialog::GetProgressIsLifesign() { return m_bProgressIsLifesign; }
void CExecuteDianaDialog::SetProgressIsLifesign(bool val) { m_bProgressIsLifesign = val; }

void CExecuteDianaDialog::OnMessage(QString str) {
  m_nCurrentMessages++;

  AddMessage(str.toStdString().c_str());
  CallBack();

  IExecuteDianaDialog::OnMessage(str);
}

void CExecuteDianaDialog::OnAppSpcMessage(QString str) {
  m_nCurrentAppSpcMessages++;

  AddMessage(str.toStdString().c_str());
  CallBack();

  if (str.indexOf(m_succeed_msg) != -1)
    m_bCalculationSucceeded = true;
}

void CExecuteDianaDialog::OnError(QString str) {
  AddWarning(str.toStdString().c_str());
  CallBack();
}

void CExecuteDianaDialog::OnWarning(QString str) {
  AddWarning(str.toStdString().c_str());
  CallBack();
}

void CExecuteDianaDialog::OnStopExecuting(bool &bStop) {
  int rv = 0;
  rv = CallBack();
  bStop = (rv != 0);
}

void CExecuteDianaDialog::CloseDialog() {
  OnFinished();
  m_bShowDialog = FALSE;
  ShowWindow(SW_HIDE);

  clear();
}

void CExecuteDianaDialog::clear() {
  CListBox *ListMessages;
  CListBox *ListWarnings;

  if (!(ListMessages = (CListBox *)GetDlgItem(IDC_LIST_MSG)))
    return;

  if (!(ListWarnings = (CListBox *)GetDlgItem(IDC_LIST_WARN)))
    return;

  ListMessages->ResetContent();
  ListWarnings->ResetContent();
}
