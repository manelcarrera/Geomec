// attrisurfacedlg.cpp : implementation file
//

#include "attrisurfacedlg.h"
#include "geomec.h"
#include "horizonbase.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriSurfaceDlg dialog

CAttriSurfaceDlg::CAttriSurfaceDlg(CSurfaceBase &surface, CWnd *pParent /*=NULL*/)
    : CAttributesTemplate<CSurfaceBase>(CAttriSurfaceDlg::IDD, surface, pParent) {
  //{{AFX_DATA_INIT(CAttriSurfaceDlg)

  //}}AFX_DATA_INIT
}

void CAttriSurfaceDlg::DoDataExchange(CDataExchange *pDX) {
  CAttributesTemplate<CSurfaceBase>::DoDataExchange(pDX);

  int nTriangles;
  CString strName;
  CString strState = _T("Unused");

  if (!pDX->m_bSaveAndValidate) {
    strName = Copy().Name().toStdString().c_str(); // Name ...
    nTriangles = Copy().Surface().FaceSize();      // Nr of triangles

    if (Copy().Used()) {
      strState = CString("Used by ") + Copy().Used()->Name().toStdString().c_str();
    }
  }

  //{{AFX_DATA_MAP(CAttriSurfaceDlg)
  DDX_Text(pDX, IDC_ST_NR_OF_TRI, nTriangles);
  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ST_STATUS, strState);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate) {
    CComboBox *pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_SURFUNIT));
    int sel = pCombo->GetCurSel();

    if (sel)
      Copy().Unit(CDoubleQuantity::FIELD_UNIT);
    else
      Copy().Unit(CDoubleQuantity::SI_UNIT);

    Copy().Name((LPCSTR)strName);
  }
}

BEGIN_MESSAGE_MAP(CAttriSurfaceDlg, CDialog)
//{{AFX_MSG_MAP(CAttriSurfaceDlg)
ON_BN_CLICKED(IDC_ST_COLOR, OnBtColor)
ON_WM_PAINT()
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriSurfaceDlg message handlers

void CAttriSurfaceDlg::OnBtColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

void CAttriSurfaceDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

BOOL CAttriSurfaceDlg::OnInitDialog() {
  CAttributesTemplate<CSurfaceBase>::OnInitDialog();

  CComboBox *pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_SURFUNIT));

  pCombo->InsertString(0, "SI");
  pCombo->InsertString(1, "FIELD");

  switch (Copy().Unit()) {
  case CDoubleQuantity::SI_UNIT:
    pCombo->SetCurSel(0);
    break;
  case CDoubleQuantity::FIELD_UNIT:
    pCombo->SetCurSel(1);
    break;
  default:
    assert(false);
  }

  if (Copy().Used()) {
    pCombo->EnableWindow(FALSE);
  }

  return TRUE;
}