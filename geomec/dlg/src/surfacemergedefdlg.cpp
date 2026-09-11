// surfacemergedefdlg.cpp : implementation file
//

#include "surfacemergedefdlg.h"
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
// CSurfaceMergeDefDlg dialog

CSurfaceMergeDefDlg::CSurfaceMergeDefDlg(MERGE_TYPE &type, CWnd *pParent /*=NULL*/)
    : CDialog(CSurfaceMergeDefDlg::IDD, pParent), m_type(type) {
  //{{AFX_DATA_INIT(CSurfaceMergeDefDlg)
  m_checked = 0;
  //}}AFX_DATA_INIT
}

void CSurfaceMergeDefDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSurfaceMergeDefDlg)
  DDX_Radio(pDX, IDC_CONNECT_RADIO, m_checked);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSurfaceMergeDefDlg, CDialog)
//{{AFX_MSG_MAP(CSurfaceMergeDefDlg)
ON_BN_CLICKED(IDC_CONNECT_RADIO, OnConnectRadio)
ON_BN_CLICKED(IDC_REMESH_RADIO, OnRemeshRadio)

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSurfaceMergeDefDlg message handlers

void CSurfaceMergeDefDlg::OnConnectRadio() { m_checked = 0; }
//
void CSurfaceMergeDefDlg::OnRemeshRadio() { m_checked = 1; }

void CSurfaceMergeDefDlg::OnCancel() {
  // TODO: Add extra cleanup here
  m_type = UNDEFINED;
  CDialog::OnCancel();
}

BOOL CSurfaceMergeDefDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CButton *but = (CButton *)(GetDlgItem(IDC_CONNECT_RADIO));
  but->SetCheck(1);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CSurfaceMergeDefDlg::OnOK() {
  // TODO: Add extra validation here
  if (m_checked == 0)
    m_type = FACES;
  else
    m_type = POINTS;
  CDialog::OnOK();
}
