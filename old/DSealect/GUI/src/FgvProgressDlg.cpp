// D:\users\wedx\svn_trunk\DSealect\GUI\src\FgvProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DSealect.h"
#include "FgvProgressDlg.h"
#include <afxcmn.h>

// CFgvProgressDlg dialog

IMPLEMENT_DYNAMIC(CFgvProgressDlg, CDialog)

CFgvProgressDlg::CFgvProgressDlg(int hProcess, const CString& strCaption, CWnd* pParent /*=NULL*/)
: CDialog(CFgvProgressDlg::IDD, pParent),
  m_hProcess(HANDLE(hProcess)),
  m_dProgVal(0),
  m_dProgInc(10),
  m_strCaption(strCaption)
{

}

CFgvProgressDlg::~CFgvProgressDlg()
{
}

void CFgvProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CFgvProgressDlg::OnCancel()
{
  DWORD dwExitCode;
  KillTimer(m_nTimer);
  if(!GetExitCodeProcess(m_hProcess, &dwExitCode))
    EndDialog(IDCANCEL);
  else if(dwExitCode == STILL_ACTIVE)
  {
    TerminateProcess(m_hProcess, -1);
    EndDialog(IDCANCEL);
  }
  else
    EndDialog(IDOK);
}

BOOL CFgvProgressDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  SetWindowText(m_strCaption);
  CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
  pProg->SetRange(0, 100);
  m_nTimer = SetTimer(1, 100, 0);

  return TRUE;
}

void CFgvProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
  bool bEnd = false;
  if(nIDEvent == 1)
  {
    DWORD dwExitCode;
    if(!GetExitCodeProcess(m_hProcess, &dwExitCode))
    {
      KillTimer(m_nTimer);
      EndDialog(IDCANCEL);
      bEnd = true;
    }
    else if(dwExitCode != STILL_ACTIVE)
    {
      KillTimer(m_nTimer);
      EndDialog(IDOK);
      bEnd = true;
    }
  }

  if(!bEnd)
  {
    if(m_dProgVal >= 100 && m_dProgInc > 0 ||
       m_dProgVal <= 0   && m_dProgInc < 0)
      m_dProgInc = -m_dProgInc;

    m_dProgVal += m_dProgInc;

    CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
    pProg->SetPos(int(m_dProgVal));
  }

  CDialog::OnTimer(nIDEvent);
}


BEGIN_MESSAGE_MAP(CFgvProgressDlg, CDialog)
  ON_WM_TIMER()
END_MESSAGE_MAP()


// CFgvProgressDlg message handlers
