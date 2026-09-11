// componentconvertdlg.cpp : implementation file
//

#include "componentconvertdlg.h"
#include "IPointSet.h"
#include "geomec.h"
#include "ielementset.h"
#include "stdafx.h"
#include "valuetype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CComponentConvertDlg dialog

CComponentConvertDlg::CComponentConvertDlg(const CValueType &value_type, IPointSet &target, CWnd *pParent /*=NULL*/)
    : CDialog(CComponentConvertDlg::IDD, pParent), m_value_type(value_type), m_target(target) {
  //{{AFX_DATA_INIT(CComponentConvertDlg)
  m_nType = 0;
  //}}AFX_DATA_INIT
}

CString CComponentConvertDlg::DlgText() const {
  CString sTarget = "point set";
  CString sSource = "point set";
  if (dynamic_cast<const IElementSet *>(&m_value_type.PointSet()))
    sSource = "element set";
  if (dynamic_cast<const IElementSet *>(&m_target))
    sTarget = "element set";

  CString sMessage;
  sMessage.Format("Convert component %s of %s %s to %s %s.", m_value_type.Name(), sSource,
                  m_value_type.PointSet().Name(), sTarget, m_target.Name());
  return sMessage;
}

geo::CValueMapper::TWeightingType CComponentConvertDlg::Type() const {
  switch (m_nType) {
  case 0:
    return geo::CValueMapper::WT_NONE;
  case 1:
    return geo::CValueMapper::WT_VOIGHT;
  case 2:
    return geo::CValueMapper::WT_REUSS;
  default:
    assert(false);
    break;
  }

  return geo::CValueMapper::WT_NONE;
}

void CComponentConvertDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  BOOL bEnable =
      dynamic_cast<const IElementSet *>(&m_value_type.PointSet()) && dynamic_cast<const IElementSet *>(&m_target);

  GetDlgItem(IDC_ST_TEXT)->SetWindowText(DlgText());
  GetDlgItem(IDC_RB_UNWEIGTHED)->EnableWindow(bEnable);
  GetDlgItem(IDC_RB_RUESS)->EnableWindow(bEnable);
  GetDlgItem(IDC_RB_VOIGHT)->EnableWindow(bEnable);

  //{{AFX_DATA_MAP(CComponentConvertDlg)
  DDX_Radio(pDX, IDC_RB_UNWEIGTHED, m_nType);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CComponentConvertDlg, CDialog)
//{{AFX_MSG_MAP(CComponentConvertDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComponentConvertDlg message handlers
