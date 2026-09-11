#include "stdafx.h"
#include "geomec.h"

#include "AdvancedConvergenceOptionsDlg.h"

CAdvancedConvergenceOptionsDlg::CAdvancedConvergenceOptionsDlg(CModelBase& model, CWnd* pParent)
: CDialog(IDD_CONVERGENCE_OPTIONS, pParent),
  m_model(model)
{
  m_nDisable = (model.ConvergenceChecks() ? 1 : 0);
  m_nDisableMinAngle = (model.ConvergenceMinimumAngleCheck() ? 1 : 0);
  m_dMinAngle = model.ConvergenceMinimumAngleValue();
  m_nDisableMinRatio = (model.ConvergenceMinimumRatioCheck() ? 1 : 0);
  m_dMinRatio = model.ConvergenceMinimumRatioValue();
  m_nDisableMinVolume = (model.ConvergenceMinimumVolumeCheck() ? 1 : 0);
  m_dMinVolume = model.ConvergenceMinimumVolumeValue();
}

BEGIN_MESSAGE_MAP(CAdvancedConvergenceOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCalculationProperties)
	ON_BN_CLICKED(IDC_CHECK_CONVERGENCE, OnDisable)
	ON_BN_CLICKED(IDC_CHECK_ANGLE, OnDisableMinAngle)
	ON_BN_CLICKED(IDC_CHECK_RATIO, OnDisableMinRatio)
	ON_BN_CLICKED(IDC_CHECK_VOLUME, OnDisableMinVolume)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAdvancedConvergenceOptionsDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  UpdateControls();
  return TRUE;
}

void CAdvancedConvergenceOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
  CString strUnit;
  CUnitNode& unitnode = GetGeomecDoc()->UnitNode();
  CLengthQuantity lq;
  double dFactor = lq.Convert(1., unitnode.Unit(), CQuantity::SI_UNIT);
  double dMinVolume;

  if(!pDX->m_bSaveAndValidate)
  {
    strUnit = lq.UnitName(unitnode.Unit()).c_str();
    strUnit += "3";
    dMinVolume = m_dMinVolume * dFactor * dFactor * dFactor;
  }

  DDX_Check(pDX, IDC_CHECK_CONVERGENCE, m_nDisable);
  DDX_Check(pDX, IDC_CHECK_ANGLE, m_nDisableMinAngle);
  DDX_Text(pDX, IDC_EDIT_ANGLE, m_dMinAngle);
  DDX_Check(pDX, IDC_CHECK_RATIO, m_nDisableMinRatio);
  DDX_Text(pDX, IDC_EDIT_RATIO, m_dMinRatio);
  DDX_Check(pDX, IDC_CHECK_VOLUME, m_nDisableMinVolume);
  DDX_Text(pDX, IDC_EDIT_VOLUME, dMinVolume);
  DDX_Text(pDX, IDC_UNIT_VOLUME, strUnit);

  if(pDX->m_bSaveAndValidate)
  {
    if(m_dMinAngle <= 0)
    {
      AfxMessageBox("Minimum angle should be greater than 0");
      pDX->PrepareEditCtrl(IDC_EDIT_ANGLE);
      pDX->Fail();
    }

    if(m_dMinRatio <= 0)
    {
      AfxMessageBox("Minimum ratio should be greater than 0");
      pDX->PrepareEditCtrl(IDC_EDIT_RATIO);
      pDX->Fail();
    }

    if(m_dMinVolume <= 0)
    {
      AfxMessageBox("Minimum volume should be greater than 0");
      pDX->PrepareEditCtrl(IDC_EDIT_VOLUME);
      pDX->Fail();
    }

    m_dMinVolume = dMinVolume / (dFactor * dFactor * dFactor);
  }
}

void CAdvancedConvergenceOptionsDlg::OnOK()
{
  if(!UpdateData(TRUE))
    return;

  m_model.ConvergenceChecks(m_nDisable != 0);
  m_model.ConvergenceMinimumAngleCheck(m_nDisableMinAngle != 0);
  m_model.ConvergenceMinimumAngleValue(m_dMinAngle);
  m_model.ConvergenceMinimumRatioCheck(m_nDisableMinRatio != 0);
  m_model.ConvergenceMinimumRatioValue(m_dMinRatio);
  m_model.ConvergenceMinimumVolumeCheck(m_nDisableMinVolume != 0);
  m_model.ConvergenceMinimumVolumeValue(m_dMinVolume);

  CDialog::OnOK();
}

void CAdvancedConvergenceOptionsDlg::OnDisable()
{
  UpdateData();
  UpdateControls();
}

void CAdvancedConvergenceOptionsDlg::OnDisableMinAngle()
{
  UpdateData();
  UpdateControls();
}

void CAdvancedConvergenceOptionsDlg::OnDisableMinRatio()
{
  UpdateData();
  UpdateControls();
}

void CAdvancedConvergenceOptionsDlg::OnDisableMinVolume()
{
  UpdateData();
  UpdateControls();
}

void CAdvancedConvergenceOptionsDlg::UpdateControls()
{
  GetDlgItem(IDC_CHECK_ANGLE)->EnableWindow(m_nDisable != 0);
  GetDlgItem(IDC_EDIT_ANGLE)->EnableWindow(m_nDisable != 0 && m_nDisableMinAngle != 0);
  GetDlgItem(IDC_UNIT_ANGLE)->EnableWindow(m_nDisable != 0 && m_nDisableMinAngle != 0);
  GetDlgItem(IDC_CHECK_RATIO)->EnableWindow(m_nDisable != 0);
  GetDlgItem(IDC_EDIT_RATIO)->EnableWindow(m_nDisable != 0 && m_nDisableMinRatio != 0);
  GetDlgItem(IDC_CHECK_VOLUME)->EnableWindow(m_nDisable != 0);
  GetDlgItem(IDC_EDIT_VOLUME)->EnableWindow(m_nDisable != 0 && m_nDisableMinVolume != 0);
  GetDlgItem(IDC_UNIT_VOLUME)->EnableWindow(m_nDisable != 0 && m_nDisableMinVolume != 0);
  GetDlgItem(IDC_LBL_NOTE)->EnableWindow(m_nDisable != 0 && m_nDisableMinVolume != 0);
}
