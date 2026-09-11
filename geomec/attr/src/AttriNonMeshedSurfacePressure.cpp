#include "AttriNonMeshedSurfacePressure.h"
#include "StdAfx.h"

#include "resource.h"

CAttriNonMeshedSurfacePressure::CAttriNonMeshedSurfacePressure(CNonMeshedSurfacePressure &pressure, CWnd *pParent)
    : CAttributesTemplate<CNonMeshedSurfacePressure>(IDD_NONMESHEDSURFACEPRESSURE, pressure, pParent) {}

BOOL CAttriNonMeshedSurfacePressure::OnInitDialog() {
  CAttributesTemplate<CNonMeshedSurfacePressure>::OnInitDialog();
  UpdateControls();
  return TRUE;
}

void CAttriNonMeshedSurfacePressure::DoDataExchange(CDataExchange *pDX) {
  double dRefPressure;
  double dRefDepth;
  double dGradient;
  CString strEmpty;
  int nManual;

  if (!pDX->m_bSaveAndValidate) {
    dRefPressure = CSinglePressure(Copy().ReferencePressure()).Value(UnitNode().Unit());
    dRefDepth = CLengthQuantity(Copy().ReferenceDepth()).Value(UnitNode().Unit());
    dGradient = CPressureGradientQuantity(Copy().Gradient()).Value(UnitNode().Unit());

    CString strUnitPressure = CSinglePressure().UnitName(UnitNode().Unit()).c_str();
    CString strUnitDepth = CLengthQuantity().UnitName(UnitNode().Unit()).c_str();
    CString strUnitGradient = CPressureGradientQuantity().UnitName(UnitNode().Unit()).c_str();

    DDX_Text(pDX, IDC_UN_REFPRESSURE, strUnitPressure);
    DDX_Text(pDX, IDC_UN_REFDEPTH, strUnitDepth);
    DDX_Text(pDX, IDC_UN_GRADIENT, strUnitGradient);

    nManual = Copy().ManualValues() ? 1 : 0;
  }

  DDX_Radio(pDX, IDC_RADIO_FORMATIONS, nManual);

  if (Copy().ManualValues()) {
    DDX_Text(pDX, IDC_EDIT_REFPRESSURE, dRefPressure);
    DDX_Text(pDX, IDC_EDIT_REFDEPTH, dRefDepth);
    DDX_Text(pDX, IDC_EDIT_GRADIENT, dGradient);
  } else if (!pDX->m_bSaveAndValidate) {
    DDX_Text(pDX, IDC_EDIT_REFPRESSURE, strEmpty);
    DDX_Text(pDX, IDC_EDIT_REFDEPTH, strEmpty);
    DDX_Text(pDX, IDC_EDIT_GRADIENT, strEmpty);
  }

  if (pDX->m_bSaveAndValidate) {
    if (Copy().ManualValues()) {
      Copy().ReferencePressure(CSinglePressure(dRefPressure, UnitNode().Unit()).Value());
      Copy().ReferenceDepth(CLengthQuantity(dRefDepth, UnitNode().Unit()).Value());
      Copy().Gradient(CPressureGradientQuantity(dGradient, UnitNode().Unit()).Value());
    }
  }
}

BEGIN_MESSAGE_MAP(CAttriNonMeshedSurfacePressure, CDialog)
ON_BN_CLICKED(IDC_RADIO_FORMATIONS, OnFormationPressure)
ON_BN_CLICKED(IDC_RADIO_MANUAL, OnManualPressure)
END_MESSAGE_MAP()

void CAttriNonMeshedSurfacePressure::OnFormationPressure() {
  UpdateData(TRUE);
  Copy().ManualValues(false);
  UpdateControls();
}

void CAttriNonMeshedSurfacePressure::OnManualPressure() {
  UpdateData(TRUE);
  Copy().ManualValues(true);
  UpdateControls();
}

void CAttriNonMeshedSurfacePressure::UpdateControls() {
  GetDlgItem(IDC_LBL_REFPRESSURE)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_EDIT_REFPRESSURE)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_UN_REFPRESSURE)->EnableWindow(Copy().ManualValues());

  GetDlgItem(IDC_LBL_REFDEPTH)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_EDIT_REFDEPTH)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_UN_REFDEPTH)->EnableWindow(Copy().ManualValues());

  GetDlgItem(IDC_LBL_GRADIENT)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_EDIT_GRADIENT)->EnableWindow(Copy().ManualValues());
  GetDlgItem(IDC_UN_GRADIENT)->EnableWindow(Copy().ManualValues());

  UpdateData(FALSE);
}
