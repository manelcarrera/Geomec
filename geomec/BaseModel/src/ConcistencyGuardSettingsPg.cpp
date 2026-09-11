#include "stdafx.h"

#include "ConsistencyGuardSettingsPg.h"
#include "ModelBase.h"
#include "resource.h"

CConsistencyGuardSettingsPg::CConsistencyGuardSettingsPg(CModelBase *pModel)
    : CPropertyPage(IDD_CG_SETTINGS_PG), m_pModel(pModel) {}

BEGIN_MESSAGE_MAP(CConsistencyGuardSettingsPg, CDialog)
ON_BN_CLICKED(IDC_CG_RESULT_CLEARING_PROPAGATION, OnSetModified)
ON_BN_CLICKED(IDC_CG_WELLPATH_PROPAGATION, OnSetModified)
ON_BN_CLICKED(IDC_CG_MOVE_POINTSETS, OnSetModified)
ON_BN_CLICKED(IDC_CG_AUTO_RESULT_EXPORT, OnSetModified)
END_MESSAGE_MAP()

BOOL CConsistencyGuardSettingsPg::OnInitDialog() {
  m_options = m_pModel->GetConsistencyGuard()->Options();

  UpdateData(false);

  return TRUE;
}

void CConsistencyGuardSettingsPg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  DDX_Check(pDX, IDC_CG_RESULT_CLEARING_PROPAGATION, (int &)m_options.propagateResultClearing);
  DDX_Check(pDX, IDC_CG_WELLPATH_PROPAGATION, (int &)m_options.propagateWellpaths);
  DDX_Check(pDX, IDC_CG_MOVE_POINTSETS, (int &)m_options.movePointsets);
  DDX_Check(pDX, IDC_CG_AUTO_RESULT_EXPORT, (int &)m_options.autoResultExport);
}

void CConsistencyGuardSettingsPg::OnOK() { m_pModel->GetConsistencyGuard()->Options(m_options); }

void CConsistencyGuardSettingsPg::OnSetModified() {
  UpdateData(true);
  SetModified(m_options != m_pModel->GetConsistencyGuard()->Options());
}
