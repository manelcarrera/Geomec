#include "MesherDlg.h"
#include "MesherDlgHelper.h"
#include "stdafx.h"
#include <string>
#include <vector>

#include "GlobalMessage.h" // constructMessage()

#include "TNOFileDialog.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

namespace {
int g_checks[] = {IDC_CHECK_WARNING, IDC_CHECK_ERROR, IDC_CHECK_INFO, -1};

std::string g_log_level_s[] = {"WARNING", "ERROR", "INFO"};

std::vector<std::string> split(const char *str, char c = '\n') {
  std::vector<std::string> result;
  do {
    const char *begin = str;
    while (*str != c && *str)
      str++;
    result.push_back(std::string(begin, str));
  } while (0 != *str++);

  return result;
}
} // namespace

bool CMesherDlg::is_active(eLogLevel level) {
  CButton *check = (CButton *)GetDlgItem(g_checks[level]); // FIXME
  return check->GetCheck() == BST_CHECKED;
}

std::string CMesherDlg::name(eLogLevel level) { return g_log_level_s[level]; }

////////////////////////////////////////////////////////////////////////////
// CMesherDlg dialog

// CMesherDlg::CMesherDlg(const std::vector <std::string>& msg_v, CWnd* pParent /*=NULL*/)
//: CDialog(CMesherDlg::IDD, pParent), m_msg_v( msg_v )
CMesherDlg::CMesherDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CMesherDlg::IDD, pParent), m_msg_v(std::vector<std::string>{}) {
  //{{AFX_DATA_INIT(CMesherDlg)
  //}}AFX_DATA_INIT
}

void CMesherDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CMesherDlg)
  DDX_Control(pDX, IDC_LIST, m_List);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMesherDlg, CDialog)
//{{AFX_MSG_MAP(CMesherDlg)
ON_BN_CLICKED(IDC_BUT_EXPORT, OnExport)
ON_BN_CLICKED(IDC_BUT_EXPORT_POINTSET, OnExportPoints)
ON_BN_CLICKED(IDC_CHECK_WARNING, OnCheckBoxClicked)
ON_BN_CLICKED(IDC_CHECK_ERROR, OnCheckBoxClicked)
ON_BN_CLICKED(IDC_CHECK_INFO, OnCheckBoxClicked)
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMesherDlg message handlers

void CMesherDlg::OnCheckBoxClicked() { update_logs(); }

HBRUSH CMesherDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor) {
  int id = pWnd->GetDlgCtrlID();

  if (id == IDC_LIST ||

      id == IDC_CHECK_WARNING || id == IDC_CHECK_ERROR || id == IDC_CHECK_INFO)

    pDC->SetBkColor(m_colorOutput_Box);

  return (HBRUSH)m_brushOutput_Box;
}

std::vector<std::string> CMesherDlg::filter_logs(const std::vector<std::string> &msg_v_in) {
  std::vector<std::string> msg_v;

  // CHECK BOXES

  bool checks_on[Num];

  int i = 0;
  while (i < Num) {
    CButton *check = (CButton *)GetDlgItem(g_checks[i]);
    checks_on[i] = check->GetCheck() == BST_CHECKED;
    i++;
  }

  // ALL CHECHED ?
  i = 0;
  bool all_checked = true;
  while (i < Num) {
    if (!checks_on[i]) {
      all_checked = false;
      break;
    }
    i++;
  }

  if (all_checked) {
    msg_v = msg_v_in;
  } else {
    for (i = 0; i < msg_v_in.size(); i++) {
      int j = 0;
      while (j < Num) {
        if (msg_v_in[i].rfind(g_log_level_s[j], 0) == 0 && checks_on[j])
          msg_v.push_back(msg_v_in[i]);
        j++;
      }
    }
  }
  return msg_v;
}

void CMesherDlg::update_logs() {
  std::string msg = IGlobalMessage::constructMessage(filter_logs(m_msg_v));

  // Coordinates coordinates;
  CMesherDlgHelper helper;

  /*std::vector <QString> coordinates_v = helper.reorientateCoordinatesNED2END( msg );
  if (coordinates_v.size() > 40)
    msg = helper.removeCoordinates( msg_v );*/

  std::vector<std::string> lines_v = split(msg.c_str());

  m_List.ResetContent();

  for (size_t j = 0; j < lines_v.size(); j++)
    m_List.AddString(lines_v[j].c_str());
}

BOOL CMesherDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_colorOutput_Box = RGB(240, 240, 240);
  m_brushOutput_Box.CreateSolidBrush(m_colorOutput_Box);

  int i = 0;
  while (g_checks[i] != -1) {
    ((CButton *)GetDlgItem(g_checks[i]))->SetCheck(true);
    i++;
  }

  update_logs();

  //----------------- Horizontal scroll
  CString str;
  CSize sz;
  int dx = 0;
  CDC *pDC = m_List.GetDC();
  for (int j = 0; j < m_List.GetCount(); j++) {
    m_List.GetText(j, str);
    sz = pDC->GetTextExtent(str);
    if (sz.cx > dx)
      dx = sz.cx;
  }
  m_List.ReleaseDC(pDC);
  if (m_List.GetHorizontalExtent() < dx) {
    m_List.SetHorizontalExtent(dx);
    ASSERT(m_List.GetHorizontalExtent() == dx);
  }
  //--------------------

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CMesherDlg::OnExport() {
  CTnoFileDialog tnoFileDialog(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                               _T("Text Files (*.txt)|*.txt||"));
  if (tnoFileDialog.DoModal() == IDOK) {
    CMesherDlgHelper helper;
    helper.save((LPCTSTR)tnoFileDialog.GetPathName(), filter_logs(m_msg_v));
  }
}

void CMesherDlg::OnExportPoints() {
  CTnoFileDialog tnoFileDialog(FALSE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                               _T("Text Files (*.txt)|*.txt||"));
  if (tnoFileDialog.DoModal() == IDOK) {
    CMesherDlgHelper helper;
    std::string new_msg = IGlobalMessage::constructMessage(filter_logs(m_msg_v));
    std::vector<QString> coordinates = helper.reorientateCoordinatesNED2END(new_msg);
    helper.reformat_coordinates(coordinates);
    helper.save((LPCTSTR)tnoFileDialog.GetPathName(), coordinates);
  }
}
