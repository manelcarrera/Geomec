#include "attrielementvalueset.h"
#include "ElementValueSet.h"
#include "geomec.h"
#include "stdafx.h"
#include "valuecomponent.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriElementValueSet dialog

CAttriElementValueSet::CAttriElementValueSet(CElementValueSet &value_set, CWnd *pParent /*=NULL*/)
    : CDialog(CAttriElementValueSet::IDD, pParent), m_value_set(value_set) {
  //{{AFX_DATA_INIT(CAttriElementValueSet)
  //}}AFX_DATA_INIT
}

void CAttriElementValueSet::DoDataExchange(CDataExchange *pDX) {
  CString sName;

  // Let the base class do it's job
  CDialog::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate)
    sName = m_value_set.Name().toStdString().c_str();

  //{{AFX_DATA_MAP(CAttriElementValueSet)
  //}}AFX_DATA_MAP

  DDX_Text(pDX, IDC_EB_NAME, sName);

  if (pDX->m_bSaveAndValidate)
    m_value_set.Name((LPCSTR)sName);
}

BEGIN_MESSAGE_MAP(CAttriElementValueSet, CDialog)
//{{AFX_MSG_MAP(CAttriElementValueSet)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriValueSet message handlers

BOOL CAttriElementValueSet::OnInitDialog() {
  CStatic *pMonth = (CStatic *)(GetDlgItem(IDC_ST_MONTH));
  pMonth->SetWindowText(m_value_set.Time().Month().toStdString().c_str());

  CString year;
  year.Format("%d", m_value_set.Time().GetYear());

  CStatic *pYear = (CStatic *)(GetDlgItem(IDC_ST_YEAR));
  pYear->SetWindowText(year);

  return CDialog::OnInitDialog();
}
