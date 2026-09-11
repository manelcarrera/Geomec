// attriwellpathdlg.cpp : implementation file
//

#include "attrinewwellpathdlg.h"
#include "BoundaryBase.h"
#include "geomec.h"
#include "modelbase.h"
#include "stdafx.h"
#include <cmath>
#include <qobject.h>
// #include <qptrlist.h>
#include "DDXQuantity.h"
#include "DoubleEdit.h"
#include "FistExport.h"
#include "GlobalUnitNode.h"
#include "MonthCalDlg.h"
#include "WellDefinitionPointList.h"
#include "WellEditDlg.h"
#include "WellPoint.h"
#include "WellPointDrawSpec.h"
#include "WellPointList.h"
#include "WellSceneInterMed.h"
#include "WellSchemeUtils.h"
#include "WellSectionDrawSpec.h"
#include <qfile.h>

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriNewWellPathDlg dialog

CAttriNewWellPathDlg::CAttriNewWellPathDlg(CNewWellPathInput &well, CWnd *pParent /*=NULL*/)
    : CDialog(IDD_ATTRI_NEWWELLPATH, pParent), m_well(well), m_color(well.Color()) {
  //{{AFX_DATA_INIT(CAttriNewWellPathDlg)
  m_strName = _T("");
  //}}AFX_DATA_INIT

  m_dDerElev.SetValue(0.0);
  m_dEasting.SetValue(0.0);
  m_dNorthing.SetValue(0.0);

  m_birth_date.setDate(m_well.BirthDate().year(), m_well.BirthDate().month(), m_well.BirthDate().day());
}

void CAttriNewWellPathDlg::DoDataExchange(CDataExchange *pDX) {
  if (!pDX->m_bSaveAndValidate) {
    ((CEdit *)GetDlgItem(IDC_ED_NEWWELLPATH_WH_NORTHING))->SetReadOnly(!m_well.IsVertical());
    ((CEdit *)GetDlgItem(IDC_ED_NEWWELLPATH_WH_EASTING))->SetReadOnly(!m_well.IsVertical());
  }

  CString strBirthDate(m_birth_date.toString("dd MM yyyy").toStdString().c_str());

  DDX_Text(pDX, IDC_ED_NEWWELLPATH_BIRTH_DATE, strBirthDate);

  //{{AFX_DATA_MAP(CAttriNewWellPathDlg)
  DDX_Control(pDX, ID_NEWWELLPATH_CHANGE_DATE, m_btChangeDate);
  DDX_Text(pDX, IDC_ED_NEWWELLPATH_NAME, m_strName);
  //}}AFX_DATA_MAP

  CGlobalUnitNode globalUnitNode;

  DDV_Quantity(pDX, IDC_ED_NEWWELLPATH_WH_NORTHING, &m_dNorthing, globalUnitNode.unitNode().Unit(), "Northing");
  DDV_Quantity(pDX, IDC_ED_NEWWELLPATH_WH_EASTING, &m_dEasting, globalUnitNode.unitNode().Unit(), "Easting");
  DDV_Quantity(pDX, IDC_ED_NEWWELLPATH_DER_ELEV, &m_dDerElev, globalUnitNode.unitNode().Unit(), "Derrick elevation");
  DDX_Control(pDX, IDC_ED_NEWWELLPATH_WH_NORTHING, m_dEditNorthing);
  DDX_Control(pDX, IDC_ED_NEWWELLPATH_WH_EASTING, m_dEditEasting);
  DDX_Control(pDX, IDC_ED_NEWWELLPATH_DER_ELEV, m_dEditDerElev);

  CString strUnit = CString(m_dDerElev.UnitName(globalUnitNode.unitNode().Unit()).c_str());
  DDX_Text(pDX, IDC_NEWWELLPATH_WH_NOR_UNIT, strUnit);
  DDX_Text(pDX, IDC_NEWWELLPATH_WH_EAS_UNIT, strUnit);
  DDX_Text(pDX, IDC_NEWWELLPATH_DER_ELEV_UNIT, strUnit);

  CModelBase *pModel = dynamic_cast<CModelBase *>(&(m_well.Model()));
}

BEGIN_MESSAGE_MAP(CAttriNewWellPathDlg, CDialog)
//{{AFX_MSG_MAP(CAttriNewWellPathDlg)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_ST_NEWWELLPATH_COLOR, OnStColor)
ON_EN_KILLFOCUS(IDC_ED_NEWWELLPATH_NAME, OnKillfocusEdName)
ON_BN_CLICKED(ID_NEWWELLPATH_CHANGE_DATE, OnButtonChangeDate)
ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateEdit)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriNewWellPathDlg message handlers

LRESULT CAttriNewWellPathDlg::OnValidateEdit(WPARAM wparam, LPARAM lparam) {
  if (wparam == IDC_ED_NEWWELLPATH_DER_ELEV) {
    // mantis 2536 wjrx
    CString text;
    ((CEdit *)GetDlgItem(IDC_ED_NEWWELLPATH_DER_ELEV))->GetWindowText(text);
  }
  UpdateData(TRUE);
  return 0;
}

void CAttriNewWellPathDlg::OnStColor() {
  m_color = SelectColor(m_color);
  Invalidate();
}

void CAttriNewWellPathDlg::OnOK() {
  UpdateData();
  if (m_well.IsVertical()) {
    if (m_dNorthing != m_well.GlobalNorthing() || m_dEasting != m_well.GlobalEasting() ||
        m_dDerElev != m_well.GlobalTVD()) {
      m_well.Recalculate(m_dNorthing, m_dEasting, m_dDerElev);
    }
  } else {
    if (m_dDerElev != m_well.DerElev()) {
      // inclined wells can not change horizontal coordinates
      m_well.Recalculate(m_dDerElev - m_well.DerElev());
    }
  }

  m_well.Name((LPCSTR)m_strName);
  m_well.BirthDate(m_birth_date);
  m_well.Color(m_color);

  CDialog::OnOK();
}

BOOL CAttriNewWellPathDlg::OnInitDialog() {

  m_dNorthing.SetValue(m_well.GlobalNorthing());
  m_dEasting.SetValue(m_well.GlobalEasting());
  m_dDerElev.SetValue(m_well.DerElev());

  m_strName = m_well.Name().toStdString().c_str();

  return CDialog::OnInitDialog();
}

void CAttriNewWellPathDlg::OnButtonChangeDate() {
  COleDateTime time(m_well.BirthDate().year(), m_well.BirthDate().month(), m_well.BirthDate().day(), 0, 0, 0);

  CRect rect;
  m_btChangeDate.GetWindowRect(&rect);
  CPoint ptScreen(rect.CenterPoint());
  CMonthCalDlg dlg(time, ptScreen, this);

  if (dlg.DoModal() == IDOK) {
    // mantis 2548 wjrx
    m_birth_date.setDate(dlg.DateTime().GetYear(), dlg.DateTime().GetMonth(), dlg.DateTime().GetDay());
    CString strBirthDate(m_birth_date.toString("dd MM yyyy").toStdString().c_str());

    ((CEdit *)GetDlgItem(IDC_ED_NEWWELLPATH_BIRTH_DATE))->SetWindowText(strBirthDate);
  }
}

void CAttriNewWellPathDlg::OnPaint() {
  CPaintDC dc(this);

  FillFrame(IDC_ST_NEWWELLPATH_COLOR, m_color, dc);
}

void CAttriNewWellPathDlg::FillFrame(const UINT uFrameId, TColor color, CPaintDC &dc) {
  CWnd *pColor = GetDlgItem(uFrameId);
  CBrush Brush(RGB(qRed(color), qGreen(color), qBlue(color)));
  CRect rectClient;
  pColor->GetWindowRect(&rectClient);
  ScreenToClient(&rectClient);
  dc.FillRect(&rectClient, &Brush);
}

CAttriNewWellPathDlg::TColor CAttriNewWellPathDlg::SelectColor(TColor nDefaultColor) {
  CColorDialog dlg;

  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = RGB(qRed(nDefaultColor), qGreen(nDefaultColor), qBlue(nDefaultColor));
  if (dlg.DoModal() == IDOK) {
    int nColorRet = dlg.GetColor();
    return qRgb(GetRValue(nColorRet), GetGValue(nColorRet), GetBValue(nColorRet));
  }

  return nDefaultColor;
}

void CAttriNewWellPathDlg::OnKillfocusEdName() { UpdateData(TRUE); }
