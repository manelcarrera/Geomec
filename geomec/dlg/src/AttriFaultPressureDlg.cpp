#include "geomec.h"
#include "stdafx.h"

#include "AttriFaultPressureDlg.h"
#include "BranchState.h"
#include "ConvertModeType.h"
#include "ModelBase.h"

CAttriFaultPressureDlg::CAttriFaultPressureDlg(CFaultPressure &faultpressure, CWnd *pParent)
    : CAttributesTemplate<CFaultPressure>(IDD_ATTRI_FAULT_PRESSURE, faultpressure, pParent) {}

void CAttriFaultPressureDlg::DoDataExchange(CDataExchange *pDX) {
  CString strDepthUnit;
  CString strPressureUnit;
  CString strGradientUnit;

  double dRefDepth;
  double dRefPressure;
  double dGradient;

  int nOutsideConvexHull;
  int nMode;

  CConvertModeType convertModeType;

  if (!pDX->m_bSaveAndValidate) {
    strDepthUnit = Copy().Gradient().ReferenceDepth().UnitName(UnitNode().Unit()).c_str();
    strPressureUnit = Copy().Gradient().ReferencePressure().UnitName(UnitNode().Unit()).c_str();
    strGradientUnit = Copy().Gradient().Gradient().UnitName(UnitNode().Unit()).c_str();

    if (Copy().Mode() == CFaultPressure::MT_GRADIENT) {
      dRefDepth = Copy().Gradient().ReferenceDepth().Value(UnitNode().Unit());
      dRefPressure = Copy().Gradient().ReferencePressure().Value(UnitNode().Unit());
      dGradient = Copy().Gradient().Gradient().Value(UnitNode().Unit());
    }

    if (Copy().DistributedSize() > 0)
      nOutsideConvexHull = Copy().DistributedOnly() ? 0 : 1;
    else
      nOutsideConvexHull = -1;

    nMode = convertModeType(Copy().Mode());
  }

  DDX_Radio(pDX, IDC_RADIO_EXTRAPOLATE, nOutsideConvexHull);

  if (nOutsideConvexHull == 0 && !pDX->m_bSaveAndValidate) {
    int iDummy = -1;
    DDX_Radio(pDX, IDC_RADIO_MAXIMUM, iDummy);
  } else {
    DDX_Radio(pDX, IDC_RADIO_MAXIMUM, nMode);
    if (nMode == -1)
      nMode = convertModeType(Copy().Mode());
  }

  DDX_Text(pDX, IDC_UN_DEPTH, strDepthUnit);
  DDX_Text(pDX, IDC_UN_PRESSURE, strPressureUnit);
  DDX_Text(pDX, IDC_UN_GRADIENT, strGradientUnit);

  if (Copy().Mode() == CFaultPressure::MT_GRADIENT && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0)) {
    DDX_Text(pDX, IDC_ED_DEPTH, dRefDepth);
    DDX_Text(pDX, IDC_ED_PRESSURE, dRefPressure);
    DDX_Text(pDX, IDC_ED_GRADIENT, dGradient);
  } else if (!pDX->m_bSaveAndValidate) {
    // empty edit boxes
    CString strEmpty;
    DDX_Text(pDX, IDC_ED_DEPTH, strEmpty);
    DDX_Text(pDX, IDC_ED_PRESSURE, strEmpty);
    DDX_Text(pDX, IDC_ED_GRADIENT, strEmpty);
  }

  if (pDX->m_bSaveAndValidate) {
    if ((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) && Copy().Mode() == CFaultPressure::MT_GRADIENT) {
      Copy().Gradient().ReferenceDepth(dRefDepth, UnitNode().Unit());
      Copy().Gradient().ReferencePressure(dRefPressure, UnitNode().Unit());
      Copy().Gradient().Gradient(dGradient, UnitNode().Unit());
    }

    if (!Copy().DistributedOnly() || Copy().DistributedSize() == 0) {
      assert(nMode >= 0);
      Copy().Mode(convertModeType(nMode));
    }

    if (Copy().DistributedSize() > 0)
      Copy().DistributedOnly(nOutsideConvexHull == 0);
  }
}

BOOL CAttriFaultPressureDlg::OnInitDialog() {
  CAttributesTemplate<CFaultPressure>::OnInitDialog();

  EnableControls();

  return FALSE;
}

BEGIN_MESSAGE_MAP(CAttriFaultPressureDlg, CDialog)
ON_BN_CLICKED(IDC_RADIO_EXTRAPOLATE, OnExtrapolated)
ON_BN_CLICKED(IDC_RADIO_INHULLONLY, OnInHullOnly)
ON_BN_CLICKED(IDC_RADIO_MAXIMUM, OnMaximum)
ON_BN_CLICKED(IDC_RADIO_MINIMUM, OnMinimum)
ON_BN_CLICKED(IDC_RADIO_GRADIENT, OnGradient)
ON_BN_CLICKED(IDC_RADIO_MAXGRADIENT, OnMaxGradient)
END_MESSAGE_MAP()

void CAttriFaultPressureDlg::OnExtrapolated() {
  UpdateData(TRUE);
  Copy().DistributedOnly(true);
  EnableControls();
}

void CAttriFaultPressureDlg::OnInHullOnly() {
  UpdateData(TRUE);
  Copy().DistributedOnly(false);
  EnableControls();
}

void CAttriFaultPressureDlg::OnMaximum() {
  UpdateData(TRUE);
  Copy().Mode(CFaultPressure::MT_MAXIMUM);
  EnableControls();
}

void CAttriFaultPressureDlg::OnMinimum() {
  UpdateData(TRUE);
  Copy().Mode(CFaultPressure::MT_MINIMUM);
  EnableControls();
}

void CAttriFaultPressureDlg::OnGradient() {
  UpdateData(TRUE);
  Copy().Mode(CFaultPressure::MT_GRADIENT);
  EnableControls();
}

void CAttriFaultPressureDlg::OnMaxGradient() {
  UpdateData(TRUE);
  Copy().Mode(CFaultPressure::MT_MAXGRADIENT);
  EnableControls();
}

void CAttriFaultPressureDlg::EnableControls() {
  GetDlgItem(IDC_RADIO_EXTRAPOLATE)->EnableWindow(Copy().DistributedSize() > 0 && !Locked());
  GetDlgItem(IDC_RADIO_INHULLONLY)->EnableWindow(Copy().DistributedSize() > 0 && !Locked());

  GetDlgItem(IDC_RADIO_MAXIMUM)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) && !Locked());
  GetDlgItem(IDC_RADIO_MINIMUM)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) && !Locked());
  GetDlgItem(IDC_RADIO_GRADIENT)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) && !Locked());

  GetDlgItem(IDC_LBL_DEPTH)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_ED_DEPTH)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_UN_DEPTH)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());

  GetDlgItem(IDC_LBL_PRESSURE)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_ED_PRESSURE)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_UN_PRESSURE)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());

  GetDlgItem(IDC_LBL_GRADIENT)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_ED_GRADIENT)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());
  GetDlgItem(IDC_UN_GRADIENT)
      ->EnableWindow((!Copy().DistributedOnly() || Copy().DistributedSize() == 0) &&
                     Copy().Mode() == CFaultPressure::MT_GRADIENT && !Locked());

  UpdateData(FALSE);
}

bool CAttriFaultPressureDlg::Locked() const {
  const CModelBase &model = static_cast<const CModelBase &>(Copy().Model());
  return model.BranchState().IsFixedStage(Copy().DepletionStage());
}
