// GVTAnalysisDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Geomec.h"
#include "GVTAnalysisDlg.h"
#include "GVTStartDlg.h"
#include "GVTSettings.h"


// CGVTAnalysisDlg dialog

IMPLEMENT_DYNAMIC(CGVTAnalysisDlg, CDialog)

CGVTAnalysisDlg::CGVTAnalysisDlg(CGVTSettings &settings, CWnd* pParent /*=NULL*/)
  : CDialog(CGVTAnalysisDlg::IDD, pParent),
  m_GVTSettings(settings),
  m_CheckForcePointSets(settings.ForceNewPointSets()==true?1:0)
{
  //{{AFX_DATA_INIT(CGVTAnalysisDlg)
  //}}AFX_DATA_INIT
}

void CGVTAnalysisDlg::OnPaint() 
{
  CPaintDC dc(this); // device context for painting

  DrawCheck(dc, m_GridCheck, m_GVTSettings.GridReady());
  DrawCheck(dc, m_VelModFileCheck, m_GVTSettings.VelocityModelFileReady());
  DrawCheck(dc, m_TimeLapseCheck, m_GVTSettings.TimeLapseReady());

  
  // Do not call CDialog::OnPaint() for painting messages
}

void CGVTAnalysisDlg::DrawCheck(CPaintDC &dc, CStatic &IconCheck, bool bCheck)
{
  RECT rect;
  IconCheck.GetWindowRect(&rect);
  ScreenToClient(&rect);

  unsigned int CheckID;
  if(bCheck)
    CheckID = IDI_CHECK_RED;
  else
    CheckID = IDI_CROSS_RED;

  DrawIconEx(dc, rect.left, rect.top, LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(CheckID)), 16, 16, 0, 0, DI_NORMAL);	
}

void CGVTAnalysisDlg::SetStartState()
{
  if(!m_GVTSettings.AnalysisIsGo())
  {
  GetDlgItem(IDC_GVTSART_STATIC)->SetWindowTextA("Unable to start the GVT analysis, make sure the settings are set up correctly. ");
  GetDlgItem(IDC_GVT_START_BUTTON)->EnableWindow(FALSE);
  }
  else
  {
  GetDlgItem(IDC_GVTSART_STATIC)->SetWindowTextA("Start the GVT analysis.");
  GetDlgItem(IDC_GVT_START_BUTTON)->EnableWindow(TRUE);
  }
}

CGVTAnalysisDlg::~CGVTAnalysisDlg()
{
}

void CGVTAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_GRID_CHECK, m_GridCheck);
  DDX_Control(pDX, IDC_VELOC_CHECK, m_VelModFileCheck);
  DDX_Control(pDX, IDC_TIMELAPSE_CHECK, m_TimeLapseCheck);
  DDX_Check(pDX, IDC_CHECK_NEW_POINTSETS, m_CheckForcePointSets);
  
  SetStartState();
}

BEGIN_MESSAGE_MAP(CGVTAnalysisDlg, CDialog)
  ON_BN_CLICKED(IDC_GVT_SETT_BUTTON, &CGVTAnalysisDlg::OnBnClickedGvtSettButton)
  ON_BN_CLICKED(IDC_GVT_START_BUTTON, &CGVTAnalysisDlg::OnBnClickedGvtStartButton)
  ON_WM_PAINT()
  ON_BN_CLICKED(IDC_CHECK_NEW_POINTSETS, &CGVTAnalysisDlg::OnBnClickedCheckNewPointsets)
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CGVTAnalysisDlg message handlers

void CGVTAnalysisDlg::OnBnClickedGvtSettButton()
{
  CGVTSettingsDlg settings(m_GVTSettings);
  if(settings.DoModal() == IDOK)
  {
  Invalidate();
  UpdateData(FALSE);
  }
}

void CGVTAnalysisDlg::OnBnClickedGvtStartButton()
{
//#ifdef _WIN64
  m_GVTSettings.Calculate();
  EndDialog(IDOK);
//#else
//  _m()->msg("Currently GVT is only available in the 64-bit version of GEOMEC");
//#endif
}


void CGVTAnalysisDlg::OnBnClickedCheckNewPointsets()
{
  m_GVTSettings.ForceNewPointSets(!m_GVTSettings.ForceNewPointSets());
  // TODO: Add your control notification handler code here
}

HBRUSH CGVTAnalysisDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, unsigned int nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
  if(pWnd->GetDlgCtrlID() == IDC_WARNING)
  {
  pDC->SetTextColor(RGB(255, 0, 0));
  pDC->SetBkMode(TRANSPARENT);
  }

  return hbr;
}
