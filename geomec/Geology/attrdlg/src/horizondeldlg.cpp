// horizondeldlg.cpp : implementation file
//

#include "horizondeldlg.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CHorizonDelDlg dialog

CHorizonDelDlg::CHorizonDelDlg(CHexaFormation *pLower, CHexaFormation *pUpper, CWnd *pParent /*=NULL*/)
    : CDialog(CHorizonDelDlg::IDD, pParent), m_pLower(pLower), m_pUpper(pUpper) {
  //{{AFX_DATA_INIT(CHorizonDelDlg)
  m_nUpper = -1;
  //}}AFX_DATA_INIT
}

void CHorizonDelDlg::DoDataExchange(CDataExchange *pDX) {
  if (!pDX->m_bSaveAndValidate) {
    m_nUpper = 0;

    if (m_pUpper) {
      GetDlgItem(IDC_RD_UPPER)->SetWindowText(m_pUpper->Name().toStdString().c_str());
      GetDlgItem(IDC_RD_UPPER)->EnableWindow(TRUE);
    } else {
      GetDlgItem(IDC_RD_UPPER)->EnableWindow(FALSE);
      m_nUpper = 1;
    }

    if (m_pLower) {
      GetDlgItem(IDC_RD_LOWER)->SetWindowText(m_pLower->Name().toStdString().c_str());
      GetDlgItem(IDC_RD_LOWER)->EnableWindow(TRUE);
    } else
      GetDlgItem(IDC_RD_LOWER)->EnableWindow(FALSE);
  }

  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHorizonDelDlg)
  DDX_Radio(pDX, IDC_RD_UPPER, m_nUpper);
  //}}AFX_DATA_MAP
}

const CHexaFormation &CHorizonDelDlg::Selection() const {
  if (m_nUpper == 0)
    return *m_pUpper;

  return *m_pLower;
}

BEGIN_MESSAGE_MAP(CHorizonDelDlg, CDialog)
//{{AFX_MSG_MAP(CHorizonDelDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHorizonDelDlg message handlers
