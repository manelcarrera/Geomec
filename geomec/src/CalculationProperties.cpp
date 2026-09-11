// CalculationProperties.cpp : implementation file
//

#include "CalculationProperties.h"
#include "geomec.h"
#include "stdafx.h"

#include "AdvancedConvergenceOptionsDlg.h"
#include "BranchState.h"
#include "GeomecUtils.h"
#include "GlobalMessage.h"
#include "KeyFile.h"
#include "ResultRegister.h"
#include "TempPath.h"
#include "resource.h"
#include "resourceIDP.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalculationProperties dialog

CCalculationProperties::CCalculationProperties(CModelBase &model, CTempPath &temppath, CWnd *pParent /*=NULL*/)
    : CDialog(CCalculationProperties::IDD, pParent), m_Model(model), m_OriginalTempPath(temppath), m_pTempPathCopy(0) {
  //{{AFX_DATA_INIT(CCalculationProperties)
  m_bUseMethod = FALSE;
  //}}AFX_DATA_INIT

  if (m_Model.SaltInitRelax() || !m_Model.CanDisableSaltInitRelax())
    m_bSaltCheck = TRUE;
  else
    m_bSaltCheck = FALSE;

  m_bEnableResultCache = (m_Model.getEnableResultCache() != false);
  m_iResultCacheSize = m_Model.getResultCacheSize();
  m_pTempPathCopy = new CTempPath(temppath);

  m_bUseParallelSolver = m_Model.UseParallelSolver() != false;
  m_nSolverThreads = m_Model.NumSolverThreads();

  m_iUseDecompactionParams = (model.UseDecompactionParameters() ? 1 : 0);
  m_iGenerateStreamFile = (model.WriteOutputStreamFile() ? 1 : 0);
}

void CCalculationProperties::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CCalculationProperties)
  DDX_Check(pDX, IDC_USEMETHOD, m_bUseMethod);
  DDX_Check(pDX, IDC_CH_SALT_STRESS, m_bSaltCheck);
  DDX_Check(pDX, IDC_ENABLE_RESULT_CACHE, m_bEnableResultCache);
  DDX_Text(pDX, IDC_EDIT_RESULT_CACHE_SIZE, m_iResultCacheSize);
  //}}AFX_DATA_MAP
  DDX_Radio(pDX, IDC_ORDERLINEAR, m_iElementOrder);
  DDX_Check(pDX, IDC_CH_DECOMP, m_iUseDecompactionParams);
  DDX_Check(pDX, IDC_CHK_STRFILE, m_iGenerateStreamFile);

  if (m_bUseParallelSolver) {
    DDX_Text(pDX, IDC_ED_NUM_THREADS, m_nSolverThreads);
    DDV_MinMaxInt(pDX, m_nSolverThreads, 1, m_Model.MaxNumSolverThreads());
  } else if (!pDX->m_bSaveAndValidate) {
    CString strEmpty;
    DDX_Text(pDX, IDC_ED_NUM_THREADS, strEmpty);
  }
}

BEGIN_MESSAGE_MAP(CCalculationProperties, CDialog)
//{{AFX_MSG_MAP(CCalculationProperties)
ON_BN_CLICKED(IDC_BROWSE_USER, OnBrowseUser)
ON_BN_CLICKED(IDC_BROWSE_CALC, OnBrowseCalc)
ON_BN_CLICKED(IDC_ENV, OnEnv)
ON_BN_CLICKED(IDC_USER, OnUser)
ON_BN_CLICKED(IDC_ORDERLINEAR, OnOrderlinear)
ON_BN_CLICKED(IDC_ORDERQUADRATIC, OnOrderquadratic)
ON_BN_CLICKED(IDC_USEMETHOD, OnUsemethod)
ON_BN_CLICKED(IDC_CHECK_PARALLEL, OnParallel)
ON_BN_CLICKED(IDC_ENABLE_RESULT_CACHE, OnEnableResultCache)
ON_BN_CLICKED(IDC_BUTTON_CONVERGENCE, OnConvergenceOptions)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalculationProperties message handlers

BOOL CCalculationProperties::OnInitDialog() {
  CDialog::OnInitDialog();

  m_bUseMethod = !m_Model.NoCSHE();

  const CGeomecDoc *pDoc = GetGeomecDoc();
  assert(pDoc != 0);

#ifdef QUADRATIC_KEYFILE
  if (pDoc->ExperimentalUnlocked()) {
#endif
    switch (m_Model.ElementOrder()) {
    case CElementOrder::EO_LINEAR:
      m_iElementOrder = 0;
      break;
    case CElementOrder::EO_QUADRATIC:
      m_iElementOrder = 1;
      assert(m_Model.NoCSHE());
      break;
    case CElementOrder::EO_INIQUAD:
      m_iElementOrder = 2;
      break;
    default:
      assert(FALSE);
    }
#ifdef QUADRATIC_KEYFILE
  } else {
    assert(m_Model.ElementOrder() == CElementOrder::EO_LINEAR);
    m_iElementOrder = 0;
    GetDlgItem(IDC_ORDERQUADRATIC)->EnableWindow(FALSE);
  }
#endif

  UpdateMethodCheckBox();

  CSpinButtonCtrl *pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_NUM_THREADS);
  pSpin->SetRange(1, m_Model.MaxNumSolverThreads());
  UpdateParallelSolverControls();
  UpdateData(FALSE);

  ((CButton *)GetDlgItem(IDC_CHECK_PARALLEL))->SetCheck(m_bUseParallelSolver);

  if (m_Model.BranchState().IsBranch()) {
    GetDlgItem(IDC_LBL_ORDER)->EnableWindow(FALSE);
    GetDlgItem(IDC_ORDERLINEAR)->EnableWindow(FALSE);
    GetDlgItem(IDC_ORDERQUADRATIC)->EnableWindow(FALSE);

    GetDlgItem(IDC_LBL_INISTATE)->EnableWindow(FALSE);
    GetDlgItem(IDC_CH_SALT_STRESS)->EnableWindow(FALSE);

    GetDlgItem(IDC_USEMETHOD)->EnableWindow(FALSE);
  }

  if (!m_Model.CanDisableSaltInitRelax())
    GetDlgItem(IDC_CH_SALT_STRESS)->EnableWindow(FALSE);

  CButton *pEnv = (CButton *)GetDlgItem(IDC_ENV);
  CButton *pUser = (CButton *)GetDlgItem(IDC_USER);
  CWnd *pPathUser = GetDlgItem(IDC_EDITUSERPATH);
  CWnd *pButtonUser = GetDlgItem(IDC_BROWSE_USER);
  CWnd *pPathCalc = GetDlgItem(IDC_EDITCALCPATH);
  CWnd *pButtonCalc = GetDlgItem(IDC_BROWSE_CALC);

  if (!TempPath().EnvPathValid()) {
    _m()->msg(IDP_TEMPVARINVALID);

    pEnv->SetCheck(FALSE);
    pUser->SetCheck(TRUE);
    pEnv->EnableWindow(FALSE);

    pPathUser->EnableWindow(TRUE);
    pButtonUser->EnableWindow(TRUE);
    pPathCalc->EnableWindow(TRUE);
    pButtonCalc->EnableWindow(TRUE);

    pPathUser->SetWindowText(TempPath().Path(CTempPath::TEMP_GENERAL).toStdString().c_str());
    pPathCalc->SetWindowText(TempPath().Path(CTempPath::TEMP_CALCULATION).toStdString().c_str());
  } else {
    pEnv->SetCheck(!TempPath().UserPath());
    pUser->SetCheck(TempPath().UserPath());
    pPathUser->EnableWindow(TempPath().UserPath());
    pButtonUser->EnableWindow(TempPath().UserPath());
    pPathCalc->EnableWindow(TempPath().UserPath());
    pButtonCalc->EnableWindow(TempPath().UserPath());

    if (TempPath().UserPath()) {
      pPathUser->SetWindowText(TempPath().Path(CTempPath::TEMP_GENERAL).toStdString().c_str());
      pPathCalc->SetWindowText(TempPath().Path(CTempPath::TEMP_CALCULATION).toStdString().c_str());
    }
  }

  OnEnableResultCache();

#if !defined(_WIN64) && !defined(LINUX_X86_64)
  GetDlgItem(IDC_RESULT_CACHE_GROUPBOX)->EnableWindow(FALSE);
  GetDlgItem(IDC_ENABLE_RESULT_CACHE)->EnableWindow(FALSE);
  GetDlgItem(IDC_STATIC_RESULT_CACHE_SIZE)->EnableWindow(FALSE);
  GetDlgItem(IDC_EDIT_RESULT_CACHE_SIZE)->EnableWindow(FALSE);
  GetDlgItem(IDC_STATIC_MB)->EnableWindow(FALSE);

  GetDlgItem(IDC_RESULT_CACHE_GROUPBOX)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_ENABLE_RESULT_CACHE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_RESULT_CACHE_SIZE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_EDIT_RESULT_CACHE_SIZE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_STATIC_MB)->ShowWindow(SW_HIDE);
#endif // !defined(_WIN64) && !defined(LINUX_X86_64)

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalculationProperties::OnOK() {
  CEdit *pPathUser = (CEdit *)GetDlgItem(IDC_EDITUSERPATH);
  CEdit *pPathCalc = (CEdit *)GetDlgItem(IDC_EDITCALCPATH);

  if (TempPath().UserPath()) {
    CString str;
    pPathUser->GetWindowText(str);
    TempPath().Path(CTempPath::TEMP_GENERAL, (LPCSTR)str);
    pPathCalc->GetWindowTextA(str);
    TempPath().Path(CTempPath::TEMP_CALCULATION, (LPCSTR)str);
  }

  if (!DirExists(TempPath().Path(CTempPath::TEMP_GENERAL))) {
    _m()->msg(IDP_INVALIDTEMPPATH);
    pPathUser->SetSel(0, -1);
    pPathUser->SetFocus();
    return;
  }

  if (!DirExists(TempPath().Path(CTempPath::TEMP_CALCULATION))) {
    _m()->msg(IDP_INVALIDTEMPPATH);
    pPathCalc->SetSel(0, -1);
    pPathCalc->SetFocus();
    return;
  }

  m_OriginalTempPath = TempPath();

  if (!UpdateData(TRUE))
    return;

  m_Model.NoCSHE(!m_bUseMethod);

  switch (m_iElementOrder) {
  case 0:
    m_Model.ElementOrder(CElementOrder::EO_LINEAR);
    break;
  case 1:
    m_Model.ElementOrder(CElementOrder::EO_QUADRATIC);
    break;
  case 2:
    m_Model.ElementOrder(CElementOrder::EO_INIQUAD);
    break;
  default:
    assert(FALSE);
  }

  bool bSaltCheck = m_bSaltCheck ? true : false;
  if (m_Model.SaltInitRelax() != bSaltCheck)
    m_Model.ResultRegister().ClearAll();
  m_Model.SaltInitRelax(m_bSaltCheck);

  m_Model.NumSolverThreads(m_nSolverThreads);
  m_Model.UseParallelSolver(m_bUseParallelSolver);

  m_Model.UseDecompactionParameters(m_iUseDecompactionParams != 0);
  m_Model.WriteOutputStreamFile(m_iGenerateStreamFile != 0);

  m_Model.setResultCache(m_bEnableResultCache, m_iResultCacheSize);

  CDialog::OnOK();
}

void CCalculationProperties::OnBrowse(int nID, CTempPath::Category category) {
  CString strPath = BrowseFolder(GetSafeHwnd(), "Browse", TempPath().Path(category)).toStdString().c_str();
  if (!strPath.IsEmpty() && DirExists((LPCSTR)strPath)) {
    TempPath().Path(category, (LPCSTR)strPath);
    GetDlgItem(nID)->SetWindowText(TempPath().Path(category).toStdString().c_str());
  }
}

void CCalculationProperties::OnBrowseUser() { OnBrowse(IDC_EDITUSERPATH, CTempPath::TEMP_GENERAL); }

void CCalculationProperties::OnBrowseCalc() { OnBrowse(IDC_EDITCALCPATH, CTempPath::TEMP_CALCULATION); }

void CCalculationProperties::OnEnv() {
  CEdit *pPathUser = (CEdit *)GetDlgItem(IDC_EDITUSERPATH);
  CWnd *pButtonUser = GetDlgItem(IDC_BROWSE_USER);
  CEdit *pPathCalc = (CEdit *)GetDlgItem(IDC_EDITCALCPATH);
  CWnd *pButtonCalc = GetDlgItem(IDC_BROWSE_CALC);

  TempPath().UserPath(FALSE);

  pButtonUser->EnableWindow(FALSE);
  pPathUser->EnableWindow(FALSE);
  pPathUser->SetWindowText("");
  pButtonCalc->EnableWindow(FALSE);
  pPathCalc->EnableWindow(FALSE);
  pPathCalc->SetWindowText("");
}

void CCalculationProperties::OnUser() {
  CEdit *pPathUser = (CEdit *)GetDlgItem(IDC_EDITUSERPATH);
  CWnd *pButtonUser = GetDlgItem(IDC_BROWSE_USER);
  CEdit *pPathCalc = (CEdit *)GetDlgItem(IDC_EDITCALCPATH);
  CWnd *pButtonCalc = GetDlgItem(IDC_BROWSE_CALC);

  TempPath().UserPath(TRUE);

  pButtonUser->EnableWindow(TRUE);
  pPathUser->EnableWindow(TRUE);
  pPathUser->SetWindowText(TempPath().Path(CTempPath::TEMP_GENERAL).toStdString().c_str());
  pButtonCalc->EnableWindow(TRUE);
  pPathCalc->EnableWindow(TRUE);
  pPathCalc->SetWindowText(TempPath().Path(CTempPath::TEMP_CALCULATION).toStdString().c_str());
}

CTempPath &CCalculationProperties::TempPath() {
  assert(m_pTempPathCopy != 0);
  return *m_pTempPathCopy;
}

void CCalculationProperties::OnOrderlinear() {
  UpdateData(TRUE);
  UpdateMethodCheckBox();
}

void CCalculationProperties::OnOrderquadratic() {
  UpdateData(TRUE);
  UpdateMethodCheckBox();
}

void CCalculationProperties::UpdateMethodCheckBox() {
  BOOL bEnable = (m_iElementOrder == 0);
  GetDlgItem(IDC_USEMETHOD)->EnableWindow(bEnable);
  if (!bEnable)
    m_bUseMethod = FALSE;
  UpdateData(FALSE);
}

void CCalculationProperties::OnUsemethod() {
  UpdateData(TRUE);
  if (m_Model.LargeDeformations() && m_bUseMethod) {
    // warn user that this combination is not advised
    _m()->msg(IDP_NL_LARGEDEFORMWITHCSHE);
  }
}

void CCalculationProperties::OnParallel() {
  if (!UpdateData(TRUE))
    ((CButton *)GetDlgItem(IDC_CHECK_PARALLEL))->SetCheck(m_bUseParallelSolver ? 1 : 0);

  m_bUseParallelSolver = ((CButton *)GetDlgItem(IDC_CHECK_PARALLEL))->GetCheck() != 0;
  UpdateParallelSolverControls();
  UpdateData(FALSE);
}

void CCalculationProperties::OnEnableResultCache() {
  UpdateData(TRUE);

  if (!m_bEnableResultCache) {
    m_iResultCacheSize = 0;
  }

  GetDlgItem(IDC_EDIT_RESULT_CACHE_SIZE)->EnableWindow(m_bEnableResultCache);
}

void CCalculationProperties::UpdateParallelSolverControls() {
  GetDlgItem(IDC_LBL_NUM_THREADS)->EnableWindow(m_bUseParallelSolver);
  GetDlgItem(IDC_ED_NUM_THREADS)->EnableWindow(m_bUseParallelSolver);
  GetDlgItem(IDC_SPIN_NUM_THREADS)->EnableWindow(m_bUseParallelSolver);
}

void CCalculationProperties::OnConvergenceOptions() {
  CAdvancedConvergenceOptionsDlg dlg(m_Model, this);
  dlg.DoModal();
}
