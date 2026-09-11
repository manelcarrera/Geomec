#include "stdafx.h"

#include "afxdialogex.h"

#include "Geomec.h"
#include "EditFaultSettings.h"
#include "FaultInitSettingsDlg.h"
#include "GlobalUnitNode.h"
#include "FaultParametersNode.h"
#include "HandleDefaultFaultParameters.h"
#include "SetGlobalFaultBehaviourDialog.h"


// CEditFaultSettings dialog

IMPLEMENT_DYNAMIC(CEditFaultSettings, CDialog)

CEditFaultSettings::CEditFaultSettings(CFemAppModel* femAppModel, CWnd* pParent)
	: CDialog(CEditFaultSettings::IDD, pParent)
  , m_femAppModel(femAppModel)
  , m_faultFractureList(m_femAppModel, Unit())
  , m_faultFractureComboBox()
  , m_handleFaultFractureBehaviour(this)
  , m_applyBehaviourToAllFaults()
  , m_applyInitD0ToAllFaults()
  , m_initialSelection(0)
{
}

CEditFaultSettings::CEditFaultSettings(CFemAppModel* femAppModel,
  const CHorizonBase* horizonBase, CWnd* pParent)
	: CDialog(CEditFaultSettings::IDD, pParent)
  , m_femAppModel(femAppModel)
  , m_faultFractureList(m_femAppModel, Unit())
  , m_faultFractureComboBox()
  , m_handleFaultFractureBehaviour(this)
  , m_applyBehaviourToAllFaults()
  , m_applyInitD0ToAllFaults()
  , m_initialSelection(
      determineInitialSelection(m_faultFractureList, horizonBase))
{
}

CEditFaultSettings::~CEditFaultSettings()
{
}

CDoubleQuantity::UNIT CEditFaultSettings::Unit() const
{
  CGlobalUnitNode globalUnitNode;
  return globalUnitNode.unitNode().Unit();
}

bool CEditFaultSettings::hasFaultFractures(const CFemAppModel* femAppModel)
{
  return CFaultFractureList::hasFaultFractures(femAppModel);
}

BOOL CEditFaultSettings::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here

  m_faultFractureComboBox.ResetContent();

  for (size_t index = 0; index < m_faultFractureList.size(); ++index)
  {
    m_faultFractureComboBox.
      AddString(m_faultFractureList[index].name().toStdString().c_str());
  }

  m_faultFractureComboBox.SetCurSel(m_initialSelection);

  // administrate the fault/fracture behaviour

  OnCbnSelchangeFaultFractureComboBox();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

// protected

void CEditFaultSettings::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_APPLY_BEHAVIOUR_TO_ALL_FAULTS, m_applyBehaviourToAllFaults);
  DDX_Control(pDX, IDC_APPLY_INIT_D0_TO_ALL_FAULTS, m_applyInitD0ToAllFaults);

  int currentSelection = 0;

  handleFaultFractureBehaviour(pDX, currentSelection);
  handleDefaultFaultParameters(pDX, currentSelection);
}

void CEditFaultSettings::OnOK()
{
  int currentSelection = m_faultFractureComboBox.GetCurSel();

  if (currentSelection == -1)
  {
    currentSelection = 0;
  }

  if (m_applyBehaviourToAllFaults.GetCheck() == BST_CHECKED)
  {
    for (int i = 0; i < m_faultFractureList.size(); ++i)
    {
      m_faultFractureList[i].FinalizeBehaviour();
    }
  }
  else
  {
    m_faultFractureList[currentSelection].FinalizeBehaviour();
  }

  if (m_applyInitD0ToAllFaults.GetCheck() == BST_CHECKED)
  {
    for (int i = 0; i < m_faultFractureList.size(); ++i)
    {
      m_faultFractureList[i].FinalizeInitD0();
    }
  }
  else
  {
    m_faultFractureList[currentSelection].FinalizeInitD0();
  }

  CDialog::OnOK();
}


BEGIN_MESSAGE_MAP(CEditFaultSettings, CDialog)
  ON_CBN_SELCHANGE(IDC_FAULT_FRACTURE_COMBO_BOX, &CEditFaultSettings::OnCbnSelchangeFaultFractureComboBox)
  ON_BN_CLICKED(IDC_FAULT_INITIALIZATION_METHOD, &CEditFaultSettings::OnBnClickedFaultInitializationMethod)
  ON_BN_CLICKED(IDC_GLOBAL_FAULT_BEHAVIOUR, &CEditFaultSettings::OnBnClickedGlobalFaultBehaviour)
  ON_EN_CHANGE(IDC_ED_COHESION, OnEnChangeDefaultFaultParameters)
  ON_EN_CHANGE(IDC_ED_FRICTION, OnEnChangeDefaultFaultParameters)
  ON_BN_CLICKED(IDC_APPLY_BEHAVIOUR_TO_ALL_FAULTS, &CEditFaultSettings::OnCheckApplyToAllFaults)
  ON_BN_CLICKED(IDC_APPLY_INIT_D0_TO_ALL_FAULTS, &CEditFaultSettings::OnCheckApplyToAllFaults)
END_MESSAGE_MAP()

// public

// CEditFaultSettings message handlers

void CEditFaultSettings::OnCbnSelchangeFaultFractureComboBox()
{
  UpdateData(FALSE);
}

void CEditFaultSettings::OnBnClickedFaultInitializationMethod()
{
  CFaultInitSettingsDlg
    faultInitSettingsDlg(static_cast <CModelBase&> (*m_femAppModel));

  faultInitSettingsDlg.DoModal();
}

void CEditFaultSettings::OnBnClickedGlobalFaultBehaviour()
{
  CSetGlobalFaultBehaviourDialog
    setGlobalFaultBehaviourDialog(m_femAppModel, this);

  setGlobalFaultBehaviourDialog.DoModal();
}

void CEditFaultSettings::OnEnChangeDefaultFaultParameters()
{
  UpdateData(TRUE);
}

void CEditFaultSettings::OnCheckApplyToAllFaults()
{
  if (m_applyBehaviourToAllFaults.GetCheck() == BST_CHECKED || m_applyInitD0ToAllFaults.GetCheck() == BST_CHECKED)
  {
    UpdateData(TRUE);
  }
}

// private

BOOL CEditFaultSettings::OnCommand(WPARAM wParam, LPARAM lParam)
{
  int currentSelection = m_faultFractureComboBox.GetCurSel();

  if (currentSelection == -1)
  {
    currentSelection = 0;
  }

  if (m_applyBehaviourToAllFaults.GetCheck() == BST_CHECKED)
  {
    int slipType = m_handleFaultFractureBehaviour.getSlipType(m_faultFractureList[currentSelection]);
    WPARAM newWParam = wParam;
    switch (slipType)
    {
    case  CHorizonBase::SLIP:
      newWParam = IDC_RADIO_SLIP;
      break;
    case  CHorizonBase::STICK:
      newWParam = IDC_RADIO_STICK;
      break;
    case  CHorizonBase::USER:
      newWParam = IDC_RADIO_USER;
      break;
    case  CHorizonBase::FRACTURE:
      newWParam = IDC_RADIO_FRACTURE;
      break;
    }
    for (int i = 0; i < m_faultFractureList.size(); ++i)
    {
      m_handleFaultFractureBehaviour.onCommand(m_faultFractureList[i], newWParam, lParam);
    }
  }
  else
  {
    m_handleFaultFractureBehaviour.onCommand(m_faultFractureList[currentSelection], wParam, lParam);
  }

  return CDialog::OnCommand(wParam, lParam);
}

void CEditFaultSettings::handleFaultFractureBehaviour(CDataExchange* pDX,
  int& currentSelection)
{
  DDX_Control(pDX, IDC_FAULT_FRACTURE_COMBO_BOX, m_faultFractureComboBox);

  currentSelection = m_faultFractureComboBox.GetCurSel();

  if (currentSelection == -1)
  {
    currentSelection = 0;
  }

  m_handleFaultFractureBehaviour.enableSlipButtons(m_faultFractureList[currentSelection].Slip() && !m_faultFractureList[currentSelection].IsBranch());

  int slipType = m_handleFaultFractureBehaviour.getSlipType(m_faultFractureList[currentSelection]);

  m_handleFaultFractureBehaviour.setSlipType(pDX, slipType);
}

void CEditFaultSettings::handleDefaultFaultParameters(CDataExchange* pDX,
  int& currentSelection)
{
  if (m_applyInitD0ToAllFaults.GetCheck() == BST_CHECKED)
  {
    for (size_t f = 0; f < m_faultFractureList.size(); ++f)
    {
      CHandleDefaultFaultParameters handleDefaultFaultParameters(&m_faultFractureList[f]);

      handleDefaultFaultParameters.DoDataExchange(pDX);
    }
  }
  else
  {
    CHandleDefaultFaultParameters handleDefaultFaultParameters(&m_faultFractureList[currentSelection]);

    handleDefaultFaultParameters.DoDataExchange(pDX);
  }
}

// static

int CEditFaultSettings::determineInitialSelection(
  const CFaultFractureList& faultFractureList, const CHorizonBase* horizonBase)
{
  for (size_t f = 0; f < faultFractureList.size(); ++f)
  {
    if (faultFractureList[f].HasHorizon(horizonBase))
    {
      return f;
    }
  }

  return 0;
}
