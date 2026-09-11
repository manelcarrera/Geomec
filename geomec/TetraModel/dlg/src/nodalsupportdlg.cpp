// nodalsupportdlg.cpp : implementation file
//

#include "geomec.h"
#include "stdafx.h"

#include "BranchState.h"
#include "DepletionStage.h"
#include "DisplacementSupportNode.h"
#include "FieldFactor.h"
#include "ModelBase.h"
#include "SupportDlgBase.h"
#include "nodalsupportdlg.h"
#include "resourceIDP.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CNodalSupportDlg dialog

CNodalSupportDlg::CNodalSupportDlg(CDisplacementSupportNode::CNodalSupportDef &supdef,
                                   const CDisplacementSupportNode &node, CWnd *pParent /*=NULL*/)
    : CSupportDlgBase(node, CNodalSupportDlg::IDD, pParent), m_SupportDef(supdef) {
  //{{AFX_DATA_INIT(CNodalSupportDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CNodalSupportDlg::DoDataExchange(CDataExchange *pDX) {
  double dN, dE, dD;

  if (!pDX->m_bSaveAndValidate) {
    EmptyEditBoxes();

    if (SupportType() == CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_LOCAL) {
      dN = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().X() * UnitFactor();
      dE = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Y() * UnitFactor();
      dD = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Z() * UnitFactor();
    }
  }

  CSupportDlgBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CNodalSupportDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  if (GetDlgItem(IDC_ED_N)->IsWindowEnabled()) {
    DDX_Text(pDX, IDC_ED_N, dN);
    DDX_Text(pDX, IDC_ED_E, dE);
    DDX_Text(pDX, IDC_ED_D, dD);
  }

  if (pDX->m_bSaveAndValidate) {
    if (SupportType() == CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_LOCAL) {
      m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().X(dN / UnitFactor());
      m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Y(dE / UnitFactor());
      m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Z(dD / UnitFactor());
    }
  }
}

void CNodalSupportDlg::FillDepletionStageCombo() {
  CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_STAGE);
  CSupportDlgBase::FillDepletionStageCombo(pCombo);
}

void CNodalSupportDlg::SetRadioButton() {
  CButton *pBut[3];
  pBut[0] = (CButton *)GetDlgItem(IDC_NONE);
  pBut[1] = (CButton *)GetDlgItem(IDC_LOCAL);
  pBut[2] = (CButton *)GetDlgItem(IDC_GLOBAL);

  for (int i = 0; i < 3; i++) {
    if ((int)SupportType() == i)
      pBut[i]->SetCheck(1);
    else
      pBut[i]->SetCheck(0);
  }
}

void CNodalSupportDlg::GetRadioButton() {
  CButton *pBut[3];
  pBut[0] = (CButton *)GetDlgItem(IDC_NONE);
  pBut[1] = (CButton *)GetDlgItem(IDC_LOCAL);
  pBut[2] = (CButton *)GetDlgItem(IDC_GLOBAL);

  for (int i = 0; i < 3; i++) {
    if (pBut[i]->GetCheck() == 1) {
      SupportType((TSupportType)i);
      break;
    }
  }
}

void CNodalSupportDlg::UpdateControls() {
  bool bLocal = (SupportType() == CDisplacementSupportNode::CNodalSupportDef::CSupportDef::ST_LOCAL);
  bool bBranch = (static_cast<const CModelBase &>(Node().Model())).BranchState().IsBranch();

  GetDlgItem(IDC_NONE)->EnableWindow(!bBranch);
  GetDlgItem(IDC_LOCAL)->EnableWindow(!bBranch);

  GetDlgItem(IDC_LB_N)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_ED_N)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_UN_N)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_LB_E)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_ED_E)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_UN_E)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_LB_D)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_ED_D)->EnableWindow(bLocal && !bBranch);
  GetDlgItem(IDC_UN_D)->EnableWindow(bLocal && !bBranch);

  bool bGlobalDefined = Node().GlobalTensorDefined(*m_vcDepletionStages[m_nDeplStage]);
  GetDlgItem(IDC_GLOBAL)->EnableWindow(bGlobalDefined && !bBranch);

  GetDlgItem(IDC_APPLYTOALL)->EnableWindow(!bBranch);
}

CNodalSupportDlg::TSupportType CNodalSupportDlg::SupportType() {
  return m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).SupportType();
}

void CNodalSupportDlg::SupportType(TSupportType nType) {
  m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).SupportType(nType);
}

void CNodalSupportDlg::FillUnitLabels() {
  CString strUnitName = _T("m");

  if (Unit() == IQuantityDouble::FIELD_UNIT)
    strUnitName = _T("ft");

  GetDlgItem(IDC_UN_N)->SetWindowText(strUnitName);
  GetDlgItem(IDC_UN_E)->SetWindowText(strUnitName);
  GetDlgItem(IDC_UN_D)->SetWindowText(strUnitName);
}

double CNodalSupportDlg::UnitFactor() {
  double dRet = 1;
  if (Unit() == IQuantityDouble::FIELD_UNIT)
    dRet = FF_FACTOR_LENGTH;

  return dRet;
}

void CNodalSupportDlg::EmptyEditBoxes() {
  GetDlgItem(IDC_ED_N)->SetWindowText(_T(""));
  GetDlgItem(IDC_ED_E)->SetWindowText(_T(""));
  GetDlgItem(IDC_ED_D)->SetWindowText(_T(""));
}

BEGIN_MESSAGE_MAP(CNodalSupportDlg, CDialog)
//{{AFX_MSG_MAP(CNodalSupportDlg)
ON_CBN_SELCHANGE(IDC_STAGE, OnSelchangeStage)
ON_BN_CLICKED(IDC_LOCAL, OnLocal)
ON_BN_CLICKED(IDC_GLOBAL, OnGlobal)
ON_BN_CLICKED(IDC_NONE, OnNone)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodalSupportDlg message handlers

void CNodalSupportDlg::OnSelchangeStage() {
  CComboBox *pCombo = (CComboBox *)GetDlgItem(IDC_STAGE);
  CSupportDlgBase::OnSelchangeStage(pCombo);
}

void CNodalSupportDlg::OnLocal() { OnRadioButton(); }

void CNodalSupportDlg::OnGlobal() { OnRadioButton(); }

void CNodalSupportDlg::OnNone() { OnRadioButton(); }

BOOL CNodalSupportDlg::OnInitDialog() {
  CSupportDlgBase::OnInitDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CNodalSupportDlg::OnOK() { CSupportDlgBase::OnOK(IDP_SUPPORT_NODE_APPLYTOALL); }
