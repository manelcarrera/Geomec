#include "stdafx.h"
#include "geomec.h"

#include "AttriWellZoomInModel.h"
#include "FormationBase.h"
#include "GlobalUnitNode.h"
#include "GlobalMessage.h"

static int IndexOf(const CFormationBase& formation, const std::vector<const CFormationBase*>& vcFormations)
{
  int i;
  for(i = 0; i < vcFormations.size(); ++i)
  {
  if(vcFormations[i] == &formation)
      return i;
  }

  return -1;
}

BEGIN_MESSAGE_MAP(CAttriWellZoomInModelDlg, CDialog)
  ON_BN_CLICKED(IDC_RADIO_START_DEFAULT, OnStartDefault)
  ON_BN_CLICKED(IDC_RADIO_START_AHD, OnStartAHD)
  ON_BN_CLICKED(IDC_RADIO_START_FORMATION, OnStartFormation)
  ON_CBN_SELCHANGE(IDC_COMBO_START_FORMATION, OnSelChangeStartFormation)
  ON_BN_CLICKED(IDC_RADIO_END_DEFAULT, OnEndDefault)
  ON_BN_CLICKED(IDC_RADIO_END_AHD, OnEndAHD)
  ON_BN_CLICKED(IDC_RADIO_END_FORMATION, OnEndFormation)
  ON_CBN_SELCHANGE(IDC_COMBO_END_FORMATION, OnSelChangeEndFormation)
  ON_EN_CHANGE(IDC_ED_START_AHD, OnChangeStartAHD)
  ON_EN_CHANGE(IDC_ED_END_AHD, OnChangeEndAHD)
END_MESSAGE_MAP()

CAttriWellZoomInModelDlg::CAttriWellZoomInModelDlg(IWellModel& model, const CString& strCaption, CWnd* pParent)
: CDialog(IDD_ATTRI_WELL_ZOOMIN_MODEL, pParent),
  m_model(model),
  m_strCaption(strCaption)
{
}

void CAttriWellZoomInModelDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  CString strUnit;
  double dRadius;
  CLengthQuantity qnLength;

  int nStartMethod;
  double dStartAHD =  -9999;
  int nStartFormation = -1;

  int nEndMethod;
  double dEndAHD = -9999;
  int nEndFormation = -1;

  CString strStartFormation;
  CString strEndFormation;

  std::vector<const CFormationBase*> vcFormations = m_model.ValidFormations();
  CGlobalUnitNode globalUnitNode;

  if(!pDX->m_bSaveAndValidate)
  {
  switch(globalUnitNode.unitNode().Unit())
  {
    case CQuantity::SI_UNIT:
      strUnit.Format("m");
      break;
    case CQuantity::FIELD_UNIT:
      strUnit.Format("ft");
      break;
  }

  dRadius = qnLength.Convert(m_dRadius, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);

  nStartMethod = int(m_pStartPosition->Method());
  dStartAHD = qnLength.Convert(m_pStartPosition->AHD(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  if(m_pStartPosition->InsideFormation())
  {
      nStartFormation = IndexOf(m_pStartPosition->Formation(), vcFormations);
      strStartFormation = m_pStartPosition->Formation().Name().toStdString().c_str();
  }
  else
  {
      nStartFormation = -1;
      strStartFormation = "<outside parent model>";
  }

  GetDlgItem(IDC_ED_START_AHD)->EnableWindow(nStartMethod == IWellModel::CPositionDef::PDM_AHD);
  GetDlgItem(IDC_UNIT_START_AHD)->EnableWindow(nStartMethod == IWellModel::CPositionDef::PDM_AHD);
  GetDlgItem(IDC_COMBO_START_FORMATION)->EnableWindow(nStartMethod == IWellModel::CPositionDef::PDM_FORMATION);

  nEndMethod = int(m_pEndPosition->Method());
  dEndAHD = qnLength.Convert(m_pEndPosition->AHD(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
  if(m_pEndPosition->InsideFormation())
  {
      nEndFormation = IndexOf(m_pEndPosition->Formation(), vcFormations);
      strEndFormation = m_pEndPosition->Formation().Name().toStdString().c_str();
  }
  else
  {
      nEndFormation = -1;
      strEndFormation = "<outside parent model>";
  }

  GetDlgItem(IDC_ED_END_AHD)->EnableWindow(nEndMethod == IWellModel::CPositionDef::PDM_AHD);
  GetDlgItem(IDC_UNIT_END_AHD)->EnableWindow(nEndMethod == IWellModel::CPositionDef::PDM_AHD);
  GetDlgItem(IDC_COMBO_END_FORMATION)->EnableWindow(nEndMethod == IWellModel::CPositionDef::PDM_FORMATION);

  bool bInsideModel = (m_pStartPosition->InsideFormation() && m_pEndPosition->InsideFormation());
  bool bStartToEnd = (m_pEndPosition->AHD() > m_pStartPosition->AHD());
  bool bValidAHD = bInsideModel && bStartToEnd;

  if(!bInsideModel)
      GetDlgItem(IDC_INVALID_AHD_WARNING)->SetWindowText("Both the start AHD and the end AHD must be inside the parent model");
  else if(!bStartToEnd)
      GetDlgItem(IDC_INVALID_AHD_WARNING)->SetWindowText("The End AHD must be greater than the Start AHD");
  else
      GetDlgItem(IDC_INVALID_AHD_WARNING)->SetWindowText("");

  GetDlgItem(IDOK)->EnableWindow(bValidAHD);
  }

  DDX_Text(pDX, IDC_UNIT_RADIUS, strUnit);
  DDX_Text(pDX, IDC_ED_RADIUS, dRadius);

  DDX_Radio(pDX, IDC_RADIO_START_DEFAULT, nStartMethod);

  CString str, empty1, empty2;
  GetDlgItem(IDC_ED_START_AHD)->GetWindowText(str);
  if(!pDX->m_bSaveAndValidate && (nStartMethod != IWellModel::CPositionDef::PDM_AHD))
  DDX_Text(pDX, IDC_ED_START_AHD, empty1);
  else if(!pDX->m_bSaveAndValidate || !str.IsEmpty())
  DDX_Text(pDX, IDC_ED_START_AHD, dStartAHD);
  else
  dStartAHD = qnLength.Convert(m_pStartPosition->AHD(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);

  DDX_Text(pDX, IDC_UNIT_START_AHD, strUnit);

  if(!pDX->m_bSaveAndValidate && (nStartMethod != IWellModel::CPositionDef::PDM_FORMATION))
  nStartFormation = -1;
  DDX_CBIndex(pDX, IDC_COMBO_START_FORMATION, nStartFormation);

  if(!pDX->m_bSaveAndValidate)
  {
  DDX_Text(pDX, IDC_LBL_START_AHD_RESULT, dStartAHD);
  DDX_Text(pDX, IDC_UNIT_START_AHD_RESULT, strUnit);
  DDX_Text(pDX, IDC_LBL_START_FORMATION_RESULT, strStartFormation);
  }

  DDX_Radio(pDX, IDC_RADIO_END_DEFAULT, nEndMethod);

  GetDlgItem(IDC_ED_END_AHD)->GetWindowText(str);
  if(!pDX->m_bSaveAndValidate && (nEndMethod != IWellModel::CPositionDef::PDM_AHD))
  DDX_Text(pDX, IDC_ED_END_AHD, empty2);
  else if(!pDX->m_bSaveAndValidate || !str.IsEmpty())
  DDX_Text(pDX, IDC_ED_END_AHD, dEndAHD);
  else
  dEndAHD = qnLength.Convert(m_pEndPosition->AHD(), globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);

  DDX_Text(pDX, IDC_UNIT_END_AHD, strUnit);

  if(!pDX->m_bSaveAndValidate && (nEndMethod != IWellModel::CPositionDef::PDM_FORMATION))
  nEndFormation = -1;
  DDX_CBIndex(pDX, IDC_COMBO_END_FORMATION, nEndFormation);

  if(!pDX->m_bSaveAndValidate)
  {
  DDX_Text(pDX, IDC_LBL_END_AHD_RESULT, dEndAHD);
  DDX_Text(pDX, IDC_UNIT_END_AHD_RESULT, strUnit);
  DDX_Text(pDX, IDC_LBL_END_FORMATION_RESULT, strEndFormation);
  }

  if(pDX->m_bSaveAndValidate)
  {
  if(dRadius <= 0)
  {
      pDX->PrepareEditCtrl(IDC_ED_RADIUS);
      _m()->msg("The radius must be greater than 0");
      pDX->Fail();
  }

  m_dRadius = qnLength.Convert(dRadius, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit());

  if(nStartMethod == IWellModel::CPositionDef::PDM_DEFAULT)
      m_pStartPosition->SetDefault();
  else if(nStartMethod == IWellModel::CPositionDef::PDM_AHD)
      m_pStartPosition->SetAHD(qnLength.Convert(dStartAHD, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit()));
  else
  {
      assert(nStartFormation == -1 || nStartFormation < vcFormations.size());
      if(nStartFormation >= 0)
    m_pStartPosition->SetFormation(*vcFormations[nStartFormation]);
  }

  if(nEndMethod == IWellModel::CPositionDef::PDM_DEFAULT)
      m_pEndPosition->SetDefault();
  else if(nEndMethod == IWellModel::CPositionDef::PDM_AHD)
      m_pEndPosition->SetAHD(qnLength.Convert(dEndAHD, CQuantity::SI_UNIT, globalUnitNode.unitNode().Unit()));
  else
  {
      assert(nEndFormation == -1 || nEndFormation < vcFormations.size());
      if(nEndFormation >= 0)
    m_pEndPosition->SetFormation(*vcFormations[nEndFormation]);
  }
  }
}

BOOL CAttriWellZoomInModelDlg::OnInitDialog()
{
  m_pStartPosition = new IWellModel::CPositionDef(m_model.StartPosition());
  m_pEndPosition = new IWellModel::CPositionDef(m_model.EndPosition());
  m_dRadius = m_model.Radius();

  CDialog::OnInitDialog();

  SetWindowText(m_strCaption);

  // fill the formations combo boxes
  std::vector<const CFormationBase*> vcFormations = m_model.ValidFormations();
  assert(!vcFormations.empty());
  CComboBox* pComboStart = (CComboBox*)GetDlgItem(IDC_COMBO_START_FORMATION);
  CComboBox* pComboEnd   = (CComboBox*)GetDlgItem(IDC_COMBO_END_FORMATION);
  for(size_t i = 0; i < vcFormations.size(); ++i)
  {
  pComboStart->AddString(vcFormations[i]->Name().toStdString().c_str());
  pComboEnd->AddString(vcFormations[i]->Name().toStdString().c_str());
  }

  UpdateData(FALSE);

  return TRUE;
}

void CAttriWellZoomInModelDlg::OnOK()
{
  if(UpdateData())
  {
  bool bSendModified = false;
  if(!(m_model.StartPosition() == *m_pStartPosition))
  {
      m_model.StartPosition() = *m_pStartPosition;
      bSendModified = true;
  }
  if(!(m_model.EndPosition() == *m_pEndPosition))
  {
      m_model.EndPosition() = *m_pEndPosition;
      bSendModified = true;
  }

  if(m_model.Radius() != m_dRadius)
  {
      m_model.Radius(m_dRadius);
      bSendModified = false; // already done by Radius function
  }

  if(bSendModified)
      m_model.Modified(GEOMETRY_CHANGED);

    EndDialog(IDOK);
  }
}

void CAttriWellZoomInModelDlg::PostNcDestroy()
{
  delete m_pStartPosition;
  delete m_pEndPosition;
}

void CAttriWellZoomInModelDlg::OnStartDefault()
{
  UpdateData(TRUE);
  m_pStartPosition->SetDefault();
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnStartAHD()
{
  UpdateData(TRUE);
  m_pStartPosition->SetAHD(m_pStartPosition->AHD());
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnStartFormation()
{
  UpdateData(TRUE);
  if(!m_pStartPosition->InsideFormation())
  m_pStartPosition->SetDefault();
  m_pStartPosition->SetFormation(m_pStartPosition->Formation());
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnSelChangeStartFormation()
{
  UpdateData(TRUE);
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnEndDefault()
{
  UpdateData(TRUE);
  m_pEndPosition->SetDefault();
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnEndAHD()
{
  UpdateData(TRUE);
  m_pEndPosition->SetAHD(m_pEndPosition->AHD());
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnEndFormation()
{
  UpdateData(TRUE);
  if(!m_pEndPosition->InsideFormation())
  m_pEndPosition->SetDefault();
  m_pEndPosition->SetFormation(m_pEndPosition->Formation());
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnSelChangeEndFormation()
{
  UpdateData(TRUE);
  UpdateData(FALSE);
}

void CAttriWellZoomInModelDlg::OnChangeStartAHD()
{
  static double previousValue = 0.0;
  CString sText;
  CWnd* pWnd = GetDlgItem(IDC_ED_START_AHD);
  assert(pWnd != 0);
  pWnd->GetWindowText(sText);
  double d;
  if((_sntscanf_s(sText, sText.GetLength(), _T("%lf"), &d) == 1) &&
  (d != previousValue))
  {
  previousValue = d;
  UpdateData(TRUE);
  UpdateData(FALSE);
  }
}

void CAttriWellZoomInModelDlg::OnChangeEndAHD()
{
  static double previousValue = 0.0;
  CString sText;
  CWnd* pWnd = GetDlgItem(IDC_ED_END_AHD);
  assert(pWnd != 0);
  pWnd->GetWindowText(sText);
  double d;
  if((_sntscanf_s(sText, sText.GetLength(), _T("%lf"), &d) == 1) &&
  (d != previousValue))
  {
  previousValue = d;
  UpdateData(TRUE);
  UpdateData(FALSE);
  }
}
