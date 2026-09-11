// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DianaX.h"
#include "ProgressDlg.h"
#include "DianaXCtl.h"

#include <direct.h>

#include "GlobalMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg *CProgressDlg::m_pDlg = NULL;

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
  : CDialog(CProgressDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_pDlg = this;
  m_bStop = FALSE;
  m_bStopIsOk = FALSE;
  m_bAutoCloseDialog = FALSE;
  m_bAlwaysAutoCloseDialog = FALSE;
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProgressDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
  //{{AFX_MSG_MAP(CProgressDlg)
  ON_BN_CLICKED(IDC_BUT_STOP, OnButStop)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

void CProgressDlg::AddMessage( CString Message )
{
  if ( !Message.GetLength() ) return;
  if( Message[Message.GetLength()-1] == '\n' )
    Message = Message.Left( Message.GetLength() - 1 );

  CListBox *ListMessages;

  if( !( ListMessages = (CListBox *) GetDlgItem( IDC_LIST_MSG ) ) ) return;

  ListMessages->AddString( Message );
  DisplayLastItem( ListMessages );
  CDianaXCtrl *pCtrl = GetControl();
  if( !pCtrl->m_bProgressIsLifesign ) pCtrl->StepProgressBar();
  CallBack();
}

void CProgressDlg::AddWarning( CString Warning )
{
  if ( !Warning.GetLength() ) return;
  if( Warning[Warning.GetLength()-1] == '\n' )
    Warning = Warning.Left( Warning.GetLength() - 1 );

//	m_bWarning = TRUE;
//	MoveControls();

  CListBox *ListWarnings;
  if( !( ListWarnings = (CListBox *) GetDlgItem( IDC_LIST_WARN ) ) ) return;

  ListWarnings->AddString( Warning );
  DisplayLastItem( ListWarnings );
  CallBack();
}

void CProgressDlg::DisplayError( CString Error )
{
  if( Error[Error.GetLength()-1] == '\n' )
    Error = Error.Left( Error.GetLength() - 1 );

  MessageBox( Error, "Error", MB_OK | MB_ICONEXCLAMATION );
}

void CProgressDlg::DisplayLastItem( CListBox *ListBox )
{
/*
  CRect Rect;
  ListBox->GetClientRect( &Rect );

  int nDisplayableItems = ( Rect.bottom - Rect.top ) / ListBox->GetItemHeight();
  int nItems = ListBox->GetCount();
  ListBox->SetTopIndex( nItems - nDisplayableItems );
*/
  if( !ListBox->GetCount() ) return;
  ListBox->SetTopIndex( ListBox->GetCount() - 1 );
}

int CProgressDlg::CallBack()
{

  BOOL bIdle = TRUE;
  LONG lIdleCount = 0;
  HWND hWndParent = ::GetParent(m_hWnd);
  BOOL bShowIdle = TRUE;
#if _MSC_VER >= 1400 // VS2005
  MSG* pMsg = &AfxGetThreadState()->m_msgCur;
#else                // VS60
  MSG* pMsg = &AfxGetThread()->m_msgCur;
#endif

  CDianaXCtrl *pCtrl = GetControl();
  if( pCtrl->m_bProgressIsLifesign ) pCtrl->StepProgressBar();

  // mantis #2581
  // wedx 06032008: return m_bStop when unable to peek message
  // so window is closed even when it's not in the foreground
  if( !::PeekMessage( pMsg, NULL, NULL, NULL, PM_NOREMOVE ) ) return m_bStop;

  // pump messages while available
  do
  {
    if( !ContinueModal() ) break;

    // pump message, but quit on WM_QUIT
    if (!AfxGetThread()->PumpMessage())
    {
      AfxPostQuitMessage(0);
      return 1; // return 1 to make DIANA quit
    }

    // show the window when certain special messages rec'd
    if (bShowIdle &&
      (pMsg->message == 0x118 || pMsg->message == WM_SYSKEYDOWN))
    {
      ShowWindow(SW_SHOWNORMAL);
      UpdateWindow();
      bShowIdle = FALSE;
    }

    if (!ContinueModal())
    {
      break;
    }

    // reset "no idle" state after pumping "normal" message
    if (AfxGetThread()->IsIdleMessage(pMsg)) break;

  } while (::PeekMessage(pMsg, NULL, NULL, NULL, PM_NOREMOVE));

  return m_bStop;
}


void CProgressDlg::OnButStop() 
{
  if( m_bStopIsOk )
  {
    _chdir( GetControl()->m_pchOldCwd );
    m_bStop = TRUE;
    return;
  }
  else
  {
    if( GetControl()->m_bDisplayStopMessage )
    {
      if( globalMessage( IDP_CONFIRM_STOP, MB_YESNO | MB_ICONQUESTION ) == IDYES )
      {
        m_bStop = TRUE;
        GetControl()->DoFireOnStop( &m_bStop );
      }
    }
    else
    {
      m_bStop = TRUE;
      GetControl()->DoFireOnStop( &m_bStop );
    }
  }
}

CDianaXCtrl *CProgressDlg::GetControl()
{
  return CDianaXCtrl::GetMe();
}

void CProgressDlg::Finish()
{
  m_bStopIsOk = TRUE;

  CWnd *StopButton;
  if( !( StopButton = GetDlgItem( IDC_BUT_STOP ) ) ) return;

  if( (m_bAutoCloseDialog && GetControl()->m_bCalculationSucceeded) || m_bAlwaysAutoCloseDialog)
  {
    _chdir( GetControl()->m_pchOldCwd );
    m_bStop = TRUE;
    StopButton->EnableWindow( FALSE ); // we don't want the user to press stop anymore
    return;
  }

  StopButton->SetWindowText( "OK" );
}
