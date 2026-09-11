#include "StdAfx.h"
#include "geomec.h"

#include "AttriWellCasingCementInterface.h"

#include "WellCasingCementInterface.h"

CAttriWellCasingCementInterface::CAttriWellCasingCementInterface(CWellCasingCementInterface &source, CWnd *pParent)
    : CDialog(IDD_ATTRI_WELLCASINGCEMENTINTERFACE, pParent), m_source(source), m_color(source.Color()),
      m_sliptype(int(source.SlipType())), m_strName(source.Name().toStdString().c_str()) {}

void CAttriWellCasingCementInterface::DoDataExchange(CDataExchange *pDX) {
  int sliptype;
  CString strName;

  if (!pDX->m_bSaveAndValidate) {
    switch (m_sliptype) {
    case CWellCasingCementInterface::SLIP:
      sliptype = 0;
      break;
    case CWellCasingCementInterface::STICK:
      sliptype = 1;
      break;
    case CWellCasingCementInterface::USER:
      sliptype = 2;
      break;
    default:
      assert(false);
    }

    strName = m_strName;
  }

  DDX_Radio(pDX, IDC_RADIO_SLIP, sliptype);
  DDX_Text(pDX, IDC_ED_NAME, strName);

  if (pDX->m_bSaveAndValidate) {
    if (strName.IsEmpty()) {
      AfxMessageBox("Please enter a valid name");
      pDX->PrepareEditCtrl(IDC_ED_NAME);
      pDX->Fail();
    }

    m_strName = strName;

    switch (sliptype) {
    case 0:
      m_sliptype = CWellCasingCementInterface::SLIP;
      break;
    case 1:
      m_sliptype = CWellCasingCementInterface::STICK;
      break;
    case 2:
      m_sliptype = CWellCasingCementInterface::USER;
      break;
    default:
      assert(false);
    }
  }
}

void CAttriWellCasingCementInterface::OnOK() {
  UpdateData(TRUE);

  m_source.Name((LPCSTR)m_strName);
  m_source.Color(m_color);
  m_source.SlipType(CWellCasingCementInterface::TSlipType(m_sliptype));

  CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CAttriWellCasingCementInterface, CDialog)
ON_BN_CLICKED(IDC_ST_COLOR, OnStColor)
ON_WM_PAINT()
END_MESSAGE_MAP()

void CAttriWellCasingCementInterface::OnStColor() {
  m_color = SelectColor(m_color);
  Invalidate();
}

void CAttriWellCasingCementInterface::OnPaint() {
  CPaintDC dc(this);
  FillFrame(IDC_ST_COLOR, m_color, dc);
}

CAttriWellCasingCementInterface::TColor CAttriWellCasingCementInterface::SelectColor(TColor nDefaultColor) {
  CColorDialog dlg;
  //	COLORREF aCustomColors[16];
  // GetCustomColors( aCustomColors );

  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = RGB(qRed(nDefaultColor), qGreen(nDefaultColor), qBlue(nDefaultColor));
  //	dlg.m_cc.lpCustColors = aCustomColors;

  CWnd::PostMessage(0x118);
  if (dlg.DoModal() == IDOK) {
    int nColorRet = dlg.GetColor();
    // SetCustomColors( aCustomColors );
    return qRgb(GetRValue(nColorRet), GetGValue(nColorRet), GetBValue(nColorRet));
  }

  return nDefaultColor;
}

void CAttriWellCasingCementInterface::FillFrame(const unsigned int uFrameId, TColor color, CPaintDC &dc) {
  CWnd *pColor = GetDlgItem(uFrameId);
  CBrush Brush(RGB(qRed(color), qGreen(color), qBlue(color)));
  CRect rectClient;
  pColor->GetWindowRect(&rectClient);
  ScreenToClient(&rectClient);
  dc.FillRect(&rectClient, &Brush);
}
