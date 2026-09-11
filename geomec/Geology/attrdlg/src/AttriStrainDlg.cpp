#include "geomec.h"
#include "stdafx.h"

#include "AttriStrainDlg.h"

CAttriStrainDlg::CAttriStrainDlg(CStrainLoad &strain, CWnd *pParent)
    : CAttriFormationLoadDlg<CStrainLoad>(strain, IDD_ATTRI_STRAIN_DLG, pParent), m_changingMode(false), m_dVolume(0),
      m_dNormal(0), m_dLateral(0), m_dDepth(0), m_dGradient(0) {}

BOOL CAttriStrainDlg::OnInitDialog() {
  BOOL bRet = CAttriFormationLoadDlg<CStrainLoad>::OnInitDialog();

  if (Copy().Type() == CStrainLoad::REPEAT_MODE) {
    Copy().Constant().UseVolumetric(false);
    Copy().Constant().UseInPlane(false);
  }

  return bRet;
}

void CAttriStrainDlg::UpdateControls() {
  CAttriFormationLoadDlg<CStrainLoad>::UpdateControls();

  UpdateData(TRUE);

  const CBranchState &branchstate = (static_cast<const CModelBase &>(Copy().Model())).BranchState();
  bool bDisableAll = (branchstate.IsBranch() && branchstate.IsFixedStage(Copy().DepletionStage()));

  // GetDlgItem(IDC_RD_EXTRA_POLATE)->EnableWindow(!bDisableAll);
  // GetDlgItem(IDC_RD_PRE_DEFINED)->EnableWindow(!bDisableAll);

  GetDlgItem(IDC_CB_PREDEFINED_VOLUMETRIC_STRAIN)->EnableWindow(!bDisableAll);
  GetDlgItem(IDC_CB_PREDEFINED_INPLANE_STRAIN)->EnableWindow(!bDisableAll);
  GetDlgItem(IDC_ED_PREDEFINED_VOLUMETRIC_STRAIN)->EnableWindow(!bDisableAll);
  GetDlgItem(IDC_ED_PREDEFINED_NORMAL_STRAIN)->EnableWindow(!bDisableAll);
  GetDlgItem(IDC_ED_PREDEFINED_LATERAL_STRAIN)->EnableWindow(!bDisableAll);

  UpdateData(FALSE);
}

void CAttriStrainDlg::DoDataExchange(CDataExchange *pDX) {
  CAttriFormationLoadDlg<CStrainLoad>::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_CB_PREDEFINED_VOLUMETRIC_STRAIN, m_chVolumetric);
  DDX_Control(pDX, IDC_CB_PREDEFINED_INPLANE_STRAIN, m_chInPlane);

  int nDistributed = -1;

  if (Copy().DistributedSize() != 0)
    nDistributed = Copy().DistributedOnly() ? 0 : 1;

  if (!pDX->m_bSaveAndValidate) {
    m_dVolume = Copy().Constant().ReferenceValue().Value(UnitNode().Unit());
    m_dNormal = Copy().Constant().ReferenceValueNormal().Value(UnitNode().Unit());
    m_dLateral = Copy().Constant().ReferenceValueLateral().Value(UnitNode().Unit());
    m_dDepth = Copy().Constant().ReferenceDepth().Value(UnitNode().Unit());
    m_dGradient = Copy().Constant().Gradient().Value(UnitNode().Unit());

    m_chVolumetric.SetCheck(Copy().Constant().UseVolumetric() ? 1 : 0);
    m_chInPlane.SetCheck(Copy().Constant().UseInPlane() ? 1 : 0);
  }

  DDX_Text(pDX, IDC_ED_PREDEFINED_VOLUMETRIC_STRAIN, m_dVolume);
  DDX_Text(pDX, IDC_ED_PREDEFINED_NORMAL_STRAIN, m_dNormal);
  DDX_Text(pDX, IDC_ED_PREDEFINED_LATERAL_STRAIN, m_dLateral);

  DDX_Radio(pDX, IDC_RD_EXTRA_POLATE, nDistributed);

  if (pDX->m_bSaveAndValidate) {
    Copy().Constant().Set(m_dVolume, m_dDepth, m_dGradient, UnitNode().Unit());
    Copy().Constant().ReferenceValueNormal(m_dNormal, UnitNode().Unit());
    Copy().Constant().ReferenceValueLateral(m_dLateral, UnitNode().Unit());
    if (m_changingMode &&
        Copy().Type() == CStrainLoad::CONSTANT_MODE) { // we're going to REPEAT_MODE, so set the checkboxes to false
      Copy().Constant().UseVolumetric(false);
      Copy().Constant().UseInPlane(false);
    } else // to whatever was clicked / already on
    {
      Copy().Constant().UseVolumetric(m_chVolumetric.GetCheck());
      Copy().Constant().UseInPlane(m_chInPlane.GetCheck());
    }
  }
}

CString CAttriStrainDlg::UndefinedLabel() const { return _T("Unused (0)"); }

void CAttriStrainDlg::OnUndefinedRadio() {
  m_changingMode = Copy().Type() == CStrainLoad::CONSTANT_MODE;
  UpdateData(TRUE);
  m_changingMode = false;
  Copy().Type(CStrainLoad::REPEAT_MODE);
  UpdateControls();
}

void CAttriStrainDlg::OnPreDefinedConstant() {
  UpdateData(TRUE);
  Copy().DistributedOnly(false);
  Copy().Type(CStrainLoad::CONSTANT_MODE); // We're doing this in DoDataExchange now, to always allow default values
                                           // when DistributedOnly is false
  UpdateControls();
}

void CAttriStrainDlg::OnPreDefinedConstantCheckbox() {
  if (Copy().Type() == CStrainLoad::REPEAT_MODE) {
    m_changingMode = true;
    UpdateData(TRUE);
    m_changingMode = false;
    Copy().DistributedOnly(false);
    Copy().Type(CStrainLoad::CONSTANT_MODE);
    UpdateControls();
  } else if (!m_chVolumetric.GetCheck() && !m_chInPlane.GetCheck()) {
    OnUndefinedRadio();
  } else {
    UpdateData(TRUE);
    UpdateControls();
  }
}

BEGIN_MESSAGE_MAP(CAttriStrainDlg, CAttriFormationLoadDlg<CStrainLoad>)
// base class methods
// ON_BN_CLICKED(IDC_RB_CONSTANT, OnConstant)
ON_BN_CLICKED(IDC_RD_UNDEFINED, OnUndefinedRadio)
ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnPreDefinedConstant)
ON_BN_CLICKED(IDC_CB_PREDEFINED_VOLUMETRIC_STRAIN, OnPreDefinedConstantCheckbox)
ON_BN_CLICKED(IDC_CB_PREDEFINED_INPLANE_STRAIN, OnPreDefinedConstantCheckbox)
ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtraPolate)
END_MESSAGE_MAP()
