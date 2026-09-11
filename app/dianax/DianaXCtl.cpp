// DianaXCtl.cpp : Implementation of the CDianaXCtrl ActiveX Control class.

#include "stdafx.h"

#include "DianaX.h"
#include "DianaXCtl.h"
#include "DianaXPpg.h"

#include "QDianaStartUp.h"

#include "lbcx.h"

#include <afxcmn.h>
#include <direct.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


IMPLEMENT_DYNCREATE(CDianaXCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CDianaXCtrl, COleControl)
  //{{AFX_MSG_MAP(CDianaXCtrl)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
  ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CDianaXCtrl, COleControl)
  //{{AFX_DISPATCH_MAP(CDianaXCtrl)
  //}}AFX_DISPATCH_MAP
  // version 1.0
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "ShowDialog", dispidShowDialog, GetShowDialog, SetShowDialog, VT_BOOL)
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "SuccessMessageString", dispidSuccessMessageString, GetSuccessMessageString, SetSuccessMessageString, VT_BSTR)
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "NumberOfMessages", dispidNumberOfMessages, GetNumberOfMessages, SetNumberOfMessages, VT_I4)
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "DisplayStopMessage", dispidDisplayStopMessage, GetDisplayStopMessage, SetDisplayStopMessage, VT_BOOL)
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "ShowDefaultMessages", dispidShowDefaultMessages, GetShowDefaultMessages, SetShowDefaultMessages, VT_BOOL)
  DISP_FUNCTION_ID(CDianaXCtrl, "SetEnvironmentVar", dispidSetEnvironmentVar, SetEnvironmentVar, VT_BOOL, VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CDianaXCtrl, "ExecuteDiana", dispidExecuteDiana, ExecuteDiana, VT_I2, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CDianaXCtrl, "GetCalculationResult", dispidGetCalculationResult, GetCalculationResult, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CDianaXCtrl, "SetUserMagic", dispidSetUserMagic, SetUserMagic, VT_BOOL, VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CDianaXCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)

  // version 1.1
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "DialogCaption", dispidDialogCaption, GetDialogCaption, SetDialogCaption, VT_BSTR)
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "AutoCloseDialog", dispidAutoCloseDialog, GetAutoCloseDialog, SetAutoCloseDialog, VT_BOOL)

  // version 2.1
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "ProgressIsLifesign", dispidProgressIsLifesign, GetProgressIsLifesign, SetProgressIsLifesign, VT_BOOL)

  // version 2.2
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "NumberOfAppSpcMessages", dispidNumberOfAppSpcMessages, GetNumberOfAppSpcMessages, SetNumberOfAppSpcMessages, VT_I4)

  //vesrsion 2.3
  DISP_PROPERTY_EX_ID(CDianaXCtrl, "AlwaysAutoCloseDialog", dispidAlwaysAutoCloseDialog, GetAlwaysAutoCloseDialog, SetAlwaysAutoCloseDialog, VT_BOOL)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CDianaXCtrl, COleControl)
  //{{AFX_EVENT_MAP(CDianaXCtrl)
  //}}AFX_EVENT_MAP
  // version 1.0
  EVENT_CUSTOM_ID("OnDiaMessage", eventidOnDiaMessage, FireOnDiaMessage, VTS_BSTR)
  EVENT_CUSTOM_ID("OnDiaWarning", eventidOnDiaWarning, FireOnDiaWarning, VTS_BSTR)
  EVENT_CUSTOM_ID("OnDiaError", eventidOnDiaError, FireOnDiaError, VTS_BSTR)
  EVENT_CUSTOM_ID("OnCallback", eventidOnCallback, FireOnCallback, VTS_PBOOL)
  EVENT_CUSTOM_ID("OnStopButton", eventidOnStopButton, FireOnStopButton, VTS_PBOOL)
  EVENT_CUSTOM_ID("OnCloseDialog", eventidOnCloseDialog, FireOnCloseDialog, VTS_NONE)
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CDianaXCtrl, 1)
  PROPPAGEID(CDianaXPropPage::guid)
END_PROPPAGEIDS(CDianaXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CDianaXCtrl, "DIANAX.DianaXCtrl.1",
  0x3a01bc17, 0xba9f, 0x11d2, 0xb3, 0xb4, 0, 0x10, 0x4b, 0x25, 0x56, 0xd9)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CDianaXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DDianaX =
    { 0x3a01bc15, 0xba9f, 0x11d2, { 0xb3, 0xb4, 0, 0x10, 0x4b, 0x25, 0x56, 0xd9 } };
const IID BASED_CODE IID_DDianaXEvents =
    { 0x3a01bc16, 0xba9f, 0x11d2, { 0xb3, 0xb4, 0, 0x10, 0x4b, 0x25, 0x56, 0xd9 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwDianaXOleMisc =
  OLEMISC_INVISIBLEATRUNTIME |
  OLEMISC_ACTIVATEWHENVISIBLE |
  OLEMISC_SETCLIENTSITEFIRST |
  OLEMISC_INSIDEOUT |
  OLEMISC_CANTLINKINSIDE |
  OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDianaXCtrl, IDS_DIANAX, _dwDianaXOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::CDianaXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CDianaXCtrl

BOOL CDianaXCtrl::CDianaXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
  // TODO: Verify that your control follows apartment-model threading rules.
  // Refer to MFC TechNote 64 for more information.
  // If your control does not conform to the apartment-model rules, then
  // you must modify the code below, changing the 6th parameter from
  // afxRegApartmentThreading to 0.

  if (bRegister)
    return AfxOleRegisterControlClass(
      AfxGetInstanceHandle(),
      m_clsid,
      m_lpszProgID,
      IDS_DIANAX,
      IDB_DIANAX,
      afxRegApartmentThreading,
      _dwDianaXOleMisc,
      _tlid,
      _wVerMajor,
      _wVerMinor);
  else
    return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::CDianaXCtrl - Constructor

//CDianaXCtrl *CDianaXCtrl::m_pCtrl = NULL;

CList <CDianaXCtrl *, CDianaXCtrl *> CDianaXCtrl::s_lstControlStack;

CDianaXCtrl *CDianaXCtrl::GetMe()
{
  if( s_lstControlStack.GetCount() > 0 ) return s_lstControlStack.GetHead();
  assert( FALSE );
  return NULL;
}

CDianaXCtrl::CDianaXCtrl()
{
  InitializeIIDs(&IID_DDianaX, &IID_DDianaXEvents);
  s_lstControlStack.AddHead( this );
  m_pDlg = NULL;
  m_bShowDialog = TRUE;
  m_bCalculationSucceeded = FALSE;
  m_strSucceedMessage = "/DIANA/DC/END";
  m_nTotalMessages = 0;
  m_nTotalAppSpcMessages = 0;
  m_nCurrentMessages = 0;
  m_nCurrentAppSpcMessages = 0;
  m_bDisplayStopMessage = TRUE;
  m_bDisplayDefaultMessages = TRUE;
  m_strDialogCaption = "Calculating";
  m_bAutoCloseDialog = FALSE;
  m_bAlwaysAutoCloseDialog=FALSE;
  m_bSizing = TRUE;
  m_bProgressIsLifesign = FALSE;
  m_bChildIsRunning = FALSE;

}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::~CDianaXCtrl - Destructor

CDianaXCtrl::~CDianaXCtrl()
{
  VERIFY( s_lstControlStack.RemoveHead() == this );

  if( m_pDlg )
  {
    FireOnCloseDialog();
    m_pDlg->DestroyWindow();
    delete m_pDlg;
    m_pDlg = NULL;
  }
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::OnDraw - Drawing function

void CDianaXCtrl::OnDraw(
      CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
  //Insert the Control's Bitmap in the control
  CBitmap bitmap;
  CDC dcDisplayMemory;
  bitmap.LoadBitmap( IDB_CONTROL );
  dcDisplayMemory.CreateCompatibleDC( pdc );
  dcDisplayMemory.SelectObject( &bitmap );
  pdc->BitBlt( 0, 0, 48, 48, &dcDisplayMemory, 0, 0, SRCCOPY );

  //Do not allow resizing
  if( m_bSizing )
  {
    SetControlSize( 48, 48 );
    m_bSizing = FALSE;
  }

}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::DoPropExchange - Persistence support

void CDianaXCtrl::DoPropExchange(CPropExchange* pPX)
{
  ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
  COleControl::DoPropExchange(pPX);

  // TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::OnResetState - Reset control to default state

void CDianaXCtrl::OnResetState()
{
  COleControl::OnResetState();  // Resets defaults found in DoPropExchange

  // TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl::AboutBox - Display an "About" box to the user

void CDianaXCtrl::AboutBox()
{
  CDialog dlgAbout(IDD_ABOUTBOX_DIANAX);
  dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl message handlers

void CDianaXCtrl::DeleteDlg()
{
  if( m_bShowDialog )
  {
    assert( m_pDlg );
    FireOnCloseDialog();
    m_pDlg->DestroyWindow();
    delete m_pDlg;
    m_pDlg = NULL;
  }
}

BOOL CDianaXCtrl::GetShowDialog() 
{
  return m_bShowDialog;
}

void CDianaXCtrl::SetShowDialog(BOOL bNewValue) 
{
  m_bShowDialog = bNewValue;

  SetModifiedFlag();
}

void CDianaXCtrl::DoFireOnCallback( int *bStop )
{
  BOOL bUserStop = FALSE;

  FireOnCallback( &bUserStop );

  bStop = (int *) bUserStop;
}

BOOL CDianaXCtrl::SetEnvironmentVar(LPCTSTR VarName, LPCTSTR Value) 
{
  int rv = vDiSetenv( "%s=%s", VarName, Value );

  TRACE( "DianaX: Set variable %s to value %s\n", VarName, Value );

  return ( rv == 0 );
}


void CDianaXCtrl::Connect()
{
  CDianaStartUp* pStart= CDianaStartUp::GetInstance();

  connect(CDianaStartUp::GetInstance() , SIGNAL(OnMessage(QString)),       this, SLOT(OnMessage(QString))); 
  connect(CDianaStartUp::GetInstance() , SIGNAL(OnAppSpcMessage(QString)), this, SLOT(OnAppSpcMessage(QString))); 
  connect(CDianaStartUp::GetInstance() , SIGNAL(OnWarning(QString)),       this, SLOT(OnWarning(QString))); 
  connect(CDianaStartUp::GetInstance() , SIGNAL(OnError(QString)),         this, SLOT(OnError(QString))); 
  connect(CDianaStartUp::GetInstance() , SIGNAL(OnFinished()),             this, SLOT(OnFinished())); 
  connect(CDianaStartUp::GetInstance() , SIGNAL(OnStopExecuting(bool&)),   this ,SLOT(OnStopExecuting(bool&))); 
}

void CDianaXCtrl::DisConnect()
{
  CDianaStartUp* pStart= CDianaStartUp::GetInstance();

  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnMessage(QString)),      this, SLOT(OnMessage(QString))); 
  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnAppSpcMessage(QString)),this, SLOT(OnAppSpcMessage(QString))); 
  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnWarning(QString)),      this, SLOT(OnWarning(QString))); 
  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnError(QString)),        this, SLOT(OnError(QString))); 
  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnFinished()),            this, SLOT(OnFinished())); 
  disconnect(CDianaStartUp::GetInstance() , SIGNAL(OnStopExecuting(bool&)),  this, SLOT(OnStopExecuting(bool&))); 
}



short CDianaXCtrl::ExecuteDiana(LPCTSTR ExeName, LPCTSTR WorkingDir, LPCTSTR ComFile, LPCTSTR DatFile, LPCTSTR FilosFile, LPCTSTR BaseName) 
{
  //doe not work if this is set to false (htg/gra)	
  m_bShowDialog=true;
  
  CDianaStartUp* pDianaStartUp = CDianaStartUp::GetInstance();

  m_bCalculationSucceeded = FALSE;

  if( WorkingDir && WorkingDir[0] )
  {
    if( _chdir( WorkingDir ) == -1 )
    {
      return -1;
    }
  }

  if( m_bShowDialog )
  {
    assert( !m_pDlg );
    m_pDlg = new CProgressDlg;
    m_pDlg->m_bAutoCloseDialog = m_bAutoCloseDialog;
    m_pDlg->m_bAlwaysAutoCloseDialog= m_bAlwaysAutoCloseDialog;
    m_pDlg->Create( IDD_DIANAX );
    m_pDlg->SetWindowText( m_strDialogCaption );
    m_pDlg->ShowWindow( SW_SHOW );
    m_pDlg->CenterWindow();

    if( m_nTotalMessages < 1 && m_nTotalAppSpcMessages < 1 && !m_bProgressIsLifesign )
    {
      CWnd *ProgressBar;
      CWnd *ProgressBarText;

      if( !( ProgressBar = m_pDlg->GetDlgItem( IDC_PROGRESS ) ) ) return -4;
      if( !( ProgressBarText = m_pDlg->GetDlgItem( IDC_TXT_PROGRESS ) ) ) return -4;

      ProgressBar->ShowWindow( SW_HIDE );
      ProgressBarText->ShowWindow( SW_HIDE );
    }
    else
    {
      CProgressCtrl *ProgressBar;
      CWnd *ProgressBarText;

      if( !( ProgressBar = (CProgressCtrl *)m_pDlg->GetDlgItem( IDC_PROGRESS ) ) ) return -4;
      if( !( ProgressBarText = m_pDlg->GetDlgItem( IDC_TXT_PROGRESS ) ) ) return -4;

      if( m_bProgressIsLifesign )
      {
        ProgressBar->SetRange( 0, 100 );
        ProgressBar->SetStep( 5 );
        ProgressBarText->SetWindowText( "" );
      }
      else
      {
        ProgressBar->SetRange( 0, 100 );
        ProgressBar->SetStep( 1 );
        ProgressBarText->SetWindowText( "0%" );
      }
      ProgressBar->SetPos( 0 );
      m_nCurrentMessages = 0;
      m_nCurrentAppSpcMessages = 0;
    }
  }


  CDianaStartUp* pStart= CDianaStartUp::GetInstance();
  Connect();	
  m_bChildIsRunning = TRUE;
    int iret = pStart->RunDiana(ExeName,
                  WorkingDir,
                  ComFile,
                  DatFile,
                  FilosFile,
                  BaseName,
                  m_bDisplayDefaultMessages,
                  (LPCSTR)m_strUserLeader,
                  (LPCSTR)m_strUserTrailer);
  m_bChildIsRunning = FALSE;

  while(m_bShowDialog && m_pDlg )
  {
    if(m_pDlg->CallBack()) break;
    Sleep( 100 );
  }

  CProgressCtrl *ProgressBar;
  if( !( ProgressBar = (CProgressCtrl *)m_pDlg->GetDlgItem( IDC_PROGRESS ) ) ) 
  {
    int nLower, nUpper;
    ProgressBar->GetRange( nLower, nUpper );
    ProgressBar->SetPos(nUpper);
  }

  DisConnect();	

  CloseDialog();

  _chdir( m_pchOldCwd );
  return iret;
}


void CDianaXCtrl::StepProgressBar()
{
  if( m_nTotalMessages < 1 && m_nTotalAppSpcMessages < 1 && !m_bProgressIsLifesign ) return;

  CProgressCtrl *ProgressBar;
  CWnd *ProgressBarText;

  if( !( ProgressBar = (CProgressCtrl *)m_pDlg->GetDlgItem( IDC_PROGRESS ) ) ) return;
  if( !( ProgressBarText = m_pDlg->GetDlgItem( IDC_TXT_PROGRESS ) ) ) return;

  assert( ::IsWindow( m_pDlg->GetDlgItem( IDC_PROGRESS )->GetSafeHwnd() ) );

  if( m_bProgressIsLifesign )
  {
    if( m_bChildIsRunning )
    {
      if( ProgressBar->StepIt() >= 100 ) ProgressBar->SetPos( 0 );
    }
    else ProgressBar->SetPos( 100 );
  }
  else
  {
    long current = 0;
    long total   = 0;
    if( m_nTotalMessages > 0 ) {
      total = m_nTotalMessages;
      current = m_nCurrentMessages;
    } else {
      assert( m_nTotalAppSpcMessages > 0 );
      total = m_nTotalAppSpcMessages;
      current = m_nCurrentAppSpcMessages;
    }

    int pos = 100. * (double) current / (double) total;

    ProgressBar->SetPos( pos );

    CString txt;
    txt.Format( "%d%%", pos );
    ProgressBarText->SetWindowText( txt );
  }
}

BOOL CDianaXCtrl::GetCalculationResult() 
{
  return m_bCalculationSucceeded;
}

BSTR CDianaXCtrl::GetSuccessMessageString() 
{
  CString strResult;
  strResult = m_strSucceedMessage;

  return strResult.AllocSysString();
}

void CDianaXCtrl::SetSuccessMessageString(LPCTSTR lpszNewValue) 
{
  CString strTemp = lpszNewValue;
  if( !strTemp.IsEmpty() )
  {
    m_strSucceedMessage = lpszNewValue;
    SetModifiedFlag();
  }
}

long CDianaXCtrl::GetNumberOfMessages() 
{
  return m_nTotalMessages;
}

void CDianaXCtrl::SetNumberOfMessages(long nNewValue) 
{
  if( nNewValue > 0 ) m_nTotalMessages = nNewValue;
  SetModifiedFlag();
}

long CDianaXCtrl::GetNumberOfAppSpcMessages() 
{
  return m_nTotalAppSpcMessages;
}

void CDianaXCtrl::SetNumberOfAppSpcMessages(long n) 
{
  if( n >= 0 ) m_nTotalAppSpcMessages = n;
  SetModifiedFlag();
}

BOOL CDianaXCtrl::GetDisplayStopMessage() 
{
  return m_bDisplayStopMessage;
}

void CDianaXCtrl::SetDisplayStopMessage(BOOL bNewValue) 
{
  m_bDisplayStopMessage = bNewValue;

  SetModifiedFlag();
}

BOOL CDianaXCtrl::GetShowDefaultMessages() 
{
  return m_bDisplayDefaultMessages;
}

void CDianaXCtrl::SetShowDefaultMessages(BOOL bNewValue) 
{
  m_bDisplayDefaultMessages = bNewValue;

  SetModifiedFlag();
}

BOOL CDianaXCtrl::SetUserMagic(LPCTSTR Leader, LPCTSTR Trailer) 
{
  m_strUserLeader = Leader;
  m_strUserTrailer = Trailer;

  return TRUE;
}

void CDianaXCtrl::SetDialogCaption(LPCTSTR lpszNewValue)
{
  m_strDialogCaption = lpszNewValue;
}

BSTR CDianaXCtrl::GetDialogCaption()
{
  CString strResult;
  strResult = m_strDialogCaption;

  return strResult.AllocSysString();
}

void CDianaXCtrl::SetAlwaysAutoCloseDialog(BOOL bNewValue)
{
  m_bAlwaysAutoCloseDialog = bNewValue;
}

BOOL CDianaXCtrl::GetAlwaysAutoCloseDialog()
{
  return m_bAlwaysAutoCloseDialog;
}


void CDianaXCtrl::SetAutoCloseDialog(BOOL bNewValue)
{
  m_bAutoCloseDialog = bNewValue;
}

BOOL CDianaXCtrl::GetAutoCloseDialog()
{
  return m_bAutoCloseDialog;
}

void CDianaXCtrl::OnSize(unsigned int nType, int cx, int cy) 
{
  COleControl::OnSize(nType, cx, cy);
  
  m_bSizing = TRUE;
}

BOOL CDianaXCtrl::GetProgressIsLifesign() 
{
  return m_bProgressIsLifesign;
}

void CDianaXCtrl::SetProgressIsLifesign(BOOL bNewValue) 
{
  m_bProgressIsLifesign = bNewValue;

  SetModifiedFlag();
}




void CDianaXCtrl::OnMessage(QString str)
{
  m_nCurrentMessages++;

  if( m_pDlg )
  {
    m_pDlg->AddMessage( str.toAscii().constData() );
    m_pDlg->CallBack();
  }

  if( str.indexOf( (LPCSTR)m_strSucceedMessage ) != -1 )
    m_bCalculationSucceeded = TRUE;

  DoFireOnDiaMessage(str.toAscii().constData());
}

void CDianaXCtrl::OnAppSpcMessage(QString str)
{
  m_nCurrentAppSpcMessages++;

  if( m_pDlg )
  {
    m_pDlg->AddMessage( str.toAscii().constData() );
    m_pDlg->CallBack();
  }

  if( str.indexOf( (LPCSTR)m_strSucceedMessage ) != -1 )
    m_bCalculationSucceeded = TRUE;

  DoFireOnDiaMessage(str.toAscii().constData());
}

void CDianaXCtrl::OnError(QString str)
{
  if( m_pDlg )
  {
    m_pDlg->AddWarning(str.toAscii().constData());
    m_pDlg->CallBack();
  }

  DoFireOnDiaError(str.toAscii().constData());
}

void CDianaXCtrl::OnWarning(QString str)
{
  if( m_pDlg )
  {
    m_pDlg->AddWarning( str.toAscii().constData() );
    m_pDlg->CallBack();
  }
  DoFireOnDiaWarning(str.toAscii().constData());
}

void CDianaXCtrl::OnStopExecuting(bool &bStop)
{
  int rv = 0;

  if( m_pDlg )
  {
    rv = m_pDlg->CallBack();
  }

  if( !rv )
    DoFireOnCallback( &rv );


  bStop = (rv != 0);
}

void CDianaXCtrl::CloseDialog()
{
  FireOnCloseDialog();
  if(m_pDlg)
  {
    m_pDlg->DestroyWindow();
    delete m_pDlg;
    m_pDlg = NULL;
  }
  m_bShowDialog=FALSE;
}

void CDianaXCtrl::OnFinished()
{
  if(m_pDlg) m_pDlg->Finish();
}
