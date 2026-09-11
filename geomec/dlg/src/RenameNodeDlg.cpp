// attrivaluetypedlg.cpp : implementation file
//

#include "RenameNodeDlg.h"
#include "Geomec.h"
#include "GlobalMessage.h"
#include "StdAfx.h"
#include "resourceIDS.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

CRenameNodeDlg::CRenameNodeDlg(CGraphNode &node, CWnd *pParent /*=NULL*/)
    : CDialog(CRenameNodeDlg::IDD, pParent), m_node(node) {
  m_strName = node.Name().toStdString().c_str();
}

void CRenameNodeDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EC_NAME, m_strName);
}

void CRenameNodeDlg::OnOK() {
  UpdateData(TRUE);
  if (m_strName.IsEmpty()) {
    _m()->msg(IDS_NAME_FIELD_EMPTY);
    return;
  }

  m_node.Name((LPCSTR)m_strName);

  CDialog::OnOK();
}
