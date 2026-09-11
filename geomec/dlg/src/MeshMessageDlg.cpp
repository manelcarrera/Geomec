// MeshMessageDlg.cpp : implementation file
//

#include "MeshMessageDlg.h"
#include "geomec.h"
#include "stdafx.h"

#include "lbcx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeshMessageDlg dialog

CMeshMessageDlg::CMeshMessageDlg(const std::vector<std::string> &message, CWnd *pParent /*=NULL*/)
    : CDialog(CMeshMessageDlg::IDD, pParent), m_message(message) {
  //{{AFX_DATA_INIT(CMeshMessageDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CMeshMessageDlg, CDialog)
//{{AFX_MSG_MAP(CMeshMessageDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeshMessageDlg message handlers

BOOL CMeshMessageDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_MESSAGES);
  int maxwidth = 0;

  int n = 0;
  for (size_t i = 0; i < m_message.size(); i++) {
    CString msg = CString(m_message[i].c_str());

    // single messages may contain linebreaks
    char *str = DiStrsave(msg);
    char *p = str;
    char *q = str;
    while (*p) {
      if (*p == '\n') {
        *p = 0;
        int width = pList->GetStringWidth(q);
        if (width > maxwidth)
          maxwidth = width;

        pList->InsertItem(n++, q);
        q = p + 1;
      }

      p++;
    }

    // write q
    int width = pList->GetStringWidth(q);
    if (width > maxwidth)
      maxwidth = width;

    pList->InsertItem(n++, q);

    // insert empty line between messages
    pList->InsertItem(n++, "");

    DiFree(str, "CMeshMessageDlg::OnInitDialog");
  }

  pList->InsertColumn(0, "", LVCFMT_LEFT, maxwidth + 10);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
