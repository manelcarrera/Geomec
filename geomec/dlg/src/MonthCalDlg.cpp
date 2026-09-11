// MonthCalDlg.cpp : implementation file
//

#include "MonthCalDlg.h"
#include "geomec.h"
#include "stdafx.h"

#include <afxdtctl.h>

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonthCalDlg dialog

CMonthCalDlg::CMonthCalDlg(const COleDateTime &time, const CPoint &point, CWnd *pParent /*=NULL*/)
    : CDialog(CMonthCalDlg::IDD, pParent), m_DateTime(time), m_pt(point), m_bExiting(FALSE) {
  //{{AFX_DATA_INIT(CMonthCalDlg)
  //}}AFX_DATA_INIT
}

void CMonthCalDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMonthCalDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMonthCalDlg, CDialog)
//{{AFX_MSG_MAP(CMonthCalDlg)
ON_WM_LBUTTONDOWN()
ON_NOTIFY(MCN_SELECT, IDC_MONTHCALENDAR, OnSelectMonthcalendar)
//}}AFX_MSG_MAP
ON_WM_NCACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonthCalDlg message handlers

BOOL CMonthCalDlg::OnNcActivate(BOOL bActive) {
  if (!bActive && !m_bExiting)
    OnCancel();

  return TRUE;
}

BOOL CMonthCalDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  CRect rectWindow;
  GetWindowRect(&rectWindow);

  MoveWindow(m_pt.x, m_pt.y, rectWindow.Width(), rectWindow.Height());

  CMonthCalCtrl *pCtrl = (CMonthCalCtrl *)GetDlgItem(IDC_MONTHCALENDAR);
  pCtrl->SetCurSel(m_DateTime);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

const COleDateTime &CMonthCalDlg::DateTime() const { return m_DateTime; }

void CMonthCalDlg::OnLButtonDown(unsigned int nFlags, CPoint point) {
  CRect rectWindow;
  GetWindowRect(&rectWindow);
  rectWindow.OffsetRect(-rectWindow.TopLeft());

  if (!rectWindow.PtInRect(point))
    OnCancel();

  CDialog::OnLButtonDown(nFlags, point);
}

void CMonthCalDlg::OnSelectMonthcalendar(NMHDR *pNMHDR, LRESULT *pResult) {
  *pResult = 0;

  NMSELCHANGE *pSelChange = (NMSELCHANGE *)pNMHDR;
  m_DateTime = COleDateTime(pSelChange->stSelStart);

  m_bExiting = TRUE;
  OnOK();
}
