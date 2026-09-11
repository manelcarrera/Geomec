// RadActTagDlg.cpp : implementation file
//

#include "RadActTagDlg.h"
#include "stdafx.h"
#include "wellschemeutils.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadActTagDlg dialog

CRadActTagDlg::CRadActTagDlg(const well::CWellLog *pWellLog, CWnd *pParent /*=NULL*/)
    : CDialog(CRadActTagDlg::IDD, pParent), m_pWellLog(pWellLog) {
  //{{AFX_DATA_INIT(CRadActTagDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CRadActTagDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRadActTagDlg)
  DDX_Control(pDX, IDC_LIST_RADACT, m_RadActListCtrl);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRadActTagDlg, CDialog)
//{{AFX_MSG_MAP(CRadActTagDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadActTagDlg message handlers

BOOL CRadActTagDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  int iColom = 0;
  CRect rect;
  m_RadActListCtrl.GetClientRect(&rect);
  double width = rect.right / 4;

  m_RadActListCtrl.InsertColumn(iColom++, _T("TMD"), LVCFMT_LEFT, width);
  m_RadActListCtrl.InsertColumn(iColom++, _T("TVD"), LVCFMT_LEFT, width);
  m_RadActListCtrl.InsertColumn(iColom++, _T("Relative displacement"), LVCFMT_LEFT, width);
  m_RadActListCtrl.InsertColumn(iColom++, _T("Axial strain"), LVCFMT_LEFT, width);
  m_RadActListCtrl.InsertColumn(iColom++, _T("Average compressibility"), LVCFMT_LEFT, width);

  int nIndex;
  CString strValue;
  int i = 0;
  int j;

  for (j = 0; j < m_pWellLog->RTagSize(); j++) {
    iColom = 0;
    nIndex = m_RadActListCtrl.InsertItem(i++, "");

    strValue.Format("%f", m_pWellLog->RTTMD(j));
    m_RadActListCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->RTTVD(j));
    m_RadActListCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->RTRelDispl(j));
    m_RadActListCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->RTAxialStrain(j));
    m_RadActListCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->RTAvgComp(j));
    m_RadActListCtrl.SetItemText(nIndex, iColom++, strValue);
  }

  // TODO: Add extra initialization here

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
