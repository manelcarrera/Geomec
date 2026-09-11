// NonlinParams.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "NonlinParams.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "KeyFile.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "resourceIDP.h"

//#include "HtmlHelp.h"
//#include "..\Help\Geomec.hm"

/////////////////////////////////////////////////////////////////////////////
// CNonlinParams dialog


CNonlinParams::CNonlinParams(CModelBase& model, CWnd* pParent /*=NULL*/)
: CDialog(CNonlinParams::IDD, pParent),
  m_model(model),
  m_bApplyAll(FALSE)
{
  //{{AFX_DATA_INIT(CNonlinParams)
  m_bApplyAll = FALSE;
  m_bLargeDeform = FALSE;
  m_bAllowLinearCalc = FALSE;
  //}}AFX_DATA_INIT

  // Copy depletion stages
  CDepletionStage *pStage = &m_model.InitialDepletionStage();
  while(pStage)
  {
    CDepletionStage *pCopy = new CDepletionStage(*pStage);
    m_vcDepletionStage.push_back(TDepletionStagePair(pStage, pCopy));

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

CNonlinParams::~CNonlinParams()
{
  for(size_t i = 0; i < m_vcDepletionStage.size(); i++)
    delete m_vcDepletionStage[i].second;
}

void CNonlinParams::DoDataExchange(CDataExchange* pDX)
{
  int		 nNrOfSteps;
  int		 nMaxNumIter;
  int		 nConvergenceType;
  int		 nIterationScheme;
  double dConvergenceNormFactor;
  double dStepScalingFactor;
  int    nManualStepSizes;
  double dMaxAutoStepSize;

  CDialog::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate && IsWindow(m_cbDepletionStage.m_hWnd))
  {
    m_iPreviousSelection = m_cbDepletionStage.GetCurSel();
    CDepletionStage *pStage = m_vcDepletionStage[m_iPreviousSelection].second;

  nManualStepSizes = (pStage->AutomaticStepSizes() ? 0 : 1);
  dMaxAutoStepSize = pStage->MaxAutomaticStepSize();
    nNrOfSteps = pStage->NLSteps();
    dConvergenceNormFactor = pStage->NLConvNormVal();
    nMaxNumIter = pStage->NLIterationSteps();
  dStepScalingFactor = pStage->NLStepScalingFactor();

    switch(pStage->NLConvergenceType())
    {
    case CDepletionStage::FORCE:
      nConvergenceType = 0;
      break;
    case CDepletionStage::DISPLACEMENT:
      nConvergenceType = 1;
      break;
    case CDepletionStage::ENERGY:
      nConvergenceType = 2;
      break;
    default:
      assert(FALSE);
      break;
    }

    switch(pStage->NLIterationScheme())
    {
    case CDepletionStage::CONSTANT:
      nIterationScheme = 0;
      break;
    case CDepletionStage::LINEAR:
      nIterationScheme = 1;
      break;
    case CDepletionStage::REGULAR:
      nIterationScheme = 2;
      break;
    case CDepletionStage::MODIFIED:
      nIterationScheme = 3;
      break;
  case CDepletionStage::SECANT:
      nIterationScheme = 4;
      break;
    default:
      assert(FALSE);
      break;
    }

  }

  //{{AFX_DATA_MAP(CNonlinParams)
  DDX_Control(pDX, IDC_DEPLETIONSTAGE, m_cbDepletionStage);
  DDX_Check(pDX, IDC_CHK_APLY_ALL, m_bApplyAll);
  DDX_Text(pDX, IDC_NROFSTEPS, nNrOfSteps);
  DDX_Text(pDX, IDC_EDIT_STEP_SCALING_FACTOR, dStepScalingFactor);
  DDX_Text(pDX, IDC_MAX_ITERATIONS, nMaxNumIter);
  DDX_Radio(pDX, IDC_FORCE, nConvergenceType);
  DDX_Radio(pDX, IDC_CONSTANT, nIterationScheme);
  DDX_Text(pDX, IDC_CONVERGENCE_NORM_FACTOR, dConvergenceNormFactor);
  DDX_Check(pDX, IDC_LARGE_DEFORM, m_bLargeDeform);
  DDX_Check(pDX, IDC_ALLOWLINEARCALC, m_bAllowLinearCalc);
  DDX_Radio(pDX, IDC_RADIO_AUTOSTEP, nManualStepSizes);
  DDX_Text(pDX, IDC_MAXSIZ, dMaxAutoStepSize);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    CDepletionStage::eConvergenceType ct;
    switch(nConvergenceType)
    {
    case 0: 
      ct = CDepletionStage::FORCE;
      break;
    case 1:
      ct = CDepletionStage::DISPLACEMENT;
      break;
    case 2:
      ct = CDepletionStage::ENERGY;
      break;
    default:
      assert(FALSE);
      break;
    }

    CDepletionStage::eIterationScheme is;
    switch(nIterationScheme)
    {
    case 0:
      is = CDepletionStage::CONSTANT;
      break;
    case 1:
      is = CDepletionStage::LINEAR;
      break;
    case 2:
      is = CDepletionStage::REGULAR;
      break;
    case 3:
      is = CDepletionStage::MODIFIED;
      break;
  case 4:
      is = CDepletionStage::SECANT;
      break;
    default:
      assert(FALSE);
      break;
    }

  if (m_model.parentModel() && is == CDepletionStage::SECANT)
  {
      CDepletionStage* pCurrentStage = m_vcDepletionStage[m_cbDepletionStage.GetCurSel()].second;
      if (m_bApplyAll || pCurrentStage->Initial())
      {
    _m()->msg("The iteration scheme can't be Secant for initial depletion stage in zoom-in model");
    pDX->Fail();
      }
  }

  if(dStepScalingFactor < 0.01 || dStepScalingFactor > 1.0)
  {
      pDX->PrepareEditCtrl(IDC_EDIT_STEP_SCALING_FACTOR);
      _m()->msg("The step scaling factor must be greater than or equal to 0.01 and less than or equal to 1");
      pDX->Fail();
  }

  if(dMaxAutoStepSize <= 0 || dMaxAutoStepSize > 1)
  {
      pDX->PrepareEditCtrl(IDC_MAXSIZ);
      _m()->msg("The Maximum relative stepsize must be greater than 0 and less than or equal to 1");
      pDX->Fail();
  }

    //wjrx mantis 2557 begin
    if ( nNrOfSteps < 1 )
    {
      _m()->msg("Number of steps >= 1");
      pDX->Fail();
    }
    else
    //wjrx mantis 2557 end
    {
    
      CDepletionStage* pStage = m_vcDepletionStage[m_iPreviousSelection].second;
      pStage->SetNonLinParams((nManualStepSizes == 0), dMaxAutoStepSize, nNrOfSteps, dStepScalingFactor, dConvergenceNormFactor, nMaxNumIter, ct, is);
    }
  }
}


BEGIN_MESSAGE_MAP(CNonlinParams, CDialog)
  //{{AFX_MSG_MAP(CNonlinParams)
  ON_CBN_SELCHANGE(IDC_DEPLETIONSTAGE, OnSelchangeDepletionstage)
  ON_BN_CLICKED(IDC_CHK_APLY_ALL, OnCheckApplyAll)
  ON_BN_CLICKED(IDC_DEFAULT, OnSetDefault)
  ON_BN_CLICKED(IDC_LARGE_DEFORM, OnLargeDeform)
  ON_BN_CLICKED(IDC_ALLOWLINEARCALC, OnAllowLinearCalc)
  ON_BN_CLICKED(IDC_HELP_BTN, OnHelpClicked)
  ON_BN_CLICKED(IDC_RADIO_AUTOSTEP, OnAutoStepSizes)
  ON_BN_CLICKED(IDC_RADIO_MANUALSTEP, OnManualStepSizes)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNonlinParams message handlers

BOOL CNonlinParams::OnInitDialog() 
{
  CDialog::OnInitDialog();
  for(int i = 0; i < m_vcDepletionStage.size(); i++)
  {
    CDepletionStage *pCopy = m_vcDepletionStage[i].second;
    VERIFY(i == m_cbDepletionStage.InsertString(i, pCopy->Name().toStdString().c_str()));
  }

  m_cbDepletionStage.SetCurSel(0);

  m_bLargeDeform = m_model.LargeDeformations();

  m_bAllowLinearCalc = m_model.AllowLinearCalc();

  const CGeomecApp *pApp = dynamic_cast<const CGeomecApp *> (AfxGetApp());
  assert(pApp != 0);

#ifdef LARGEDEF_KEYFILE
  if(!pApp->KeyFile().Unlocked())
  {
    assert(!m_bLargeDeform);
    GetDlgItem(IDC_LARGE_DEFORM)->EnableWindow(FALSE);
  }
#endif

  UpdateData(FALSE);

  if(m_model.BranchState().IsBranch())
  {
    GetDlgItem(IDC_FRM_DEFORM)->EnableWindow(FALSE);
    GetDlgItem(IDC_LARGE_DEFORM)->EnableWindow(FALSE);
  }

  UpdateStageControls();
  
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}


void CNonlinParams::OnOK() 
{
  if(!UpdateData(TRUE))
    return;

  // Data is validated so start changing model
  if(m_bApplyAll)
  {
    int iCurrent = m_cbDepletionStage.GetCurSel();
    CDepletionStage* pCurrentStage = m_vcDepletionStage[iCurrent].second;
  bool bAutoStepSizes = pCurrentStage->AutomaticStepSizes();
  double dMaxAutoStepSize = pCurrentStage->MaxAutomaticStepSize();
    int nNrOfSteps = pCurrentStage->NLSteps();
  double dStepScalingFactor = pCurrentStage->NLStepScalingFactor();
    double dConvergenceNormFactor = pCurrentStage->NLConvNormVal();
    int nMaxNumIter = pCurrentStage->NLIterationSteps();
    CDepletionStage::eConvergenceType ct = pCurrentStage->NLConvergenceType();
    CDepletionStage::eIterationScheme is = pCurrentStage->NLIterationScheme();

    for(size_t i = 0; i < m_vcDepletionStage.size(); i++)
    {
      CDepletionStage *pStage = m_vcDepletionStage[i].second;
/*      if(pStage->Initial())
      {
    // only use convergence norm value and maximum number of iterations, rest is fixed
    bool bAutoStepSizes = pStage->AutomaticStepSizes();
    double dMaxAutoStepSize = pStage->MaxAutomaticStepSize();
    int nSteps = pStage->NLSteps();
    double dIniStepScalingFactor = pStage->NLStepScalingFactor();
    CDepletionStage::eConvergenceType ict = pStage->NLConvergenceType();
    CDepletionStage::eIterationScheme iis = pStage->NLIterationScheme();
    pStage->SetNonLinParams(bAutoStepSizes, dMaxAutoStepSize, nSteps, dIniStepScalingFactor, dConvergenceNormFactor, nMaxNumIter, ict, iis);
      }
      else
      {*/
        pStage->SetNonLinParams(bAutoStepSizes, dMaxAutoStepSize, nNrOfSteps, dStepScalingFactor, dConvergenceNormFactor, nMaxNumIter, ct, is);
//      }
    }
  }

  for(size_t i = 0; i < m_vcDepletionStage.size(); i++)
  {
    CDepletionStage& source = *(m_vcDepletionStage[i].first);
    const CBranchState& branchstate = m_model.BranchState();
    if(!branchstate.IsBranch() || !branchstate.IsFixedStage(source))
    {
      CDepletionStage& copy = *(m_vcDepletionStage[i].second);
      source = copy;
    }
  }

  m_model.LargeDeformations(m_bLargeDeform);
  m_model.AllowLinearCalc(m_bAllowLinearCalc);

  CDialog::OnOK();
}

void CNonlinParams::OnSelchangeDepletionstage() 
{
  UpdateData(TRUE);
  UpdateData(FALSE);

  UpdateStageControls();
}

void CNonlinParams::UpdateStageControls()
{
  const CDepletionStage& current = *m_vcDepletionStage[m_cbDepletionStage.GetCurSel()].first;
  bool bFixed = m_model.BranchState().IsBranch() && m_model.BranchState().IsFixedStage(current);
  //bool bInitial = current.Initial() != FALSE;
  bool bAutoStepSizes = m_vcDepletionStage[m_cbDepletionStage.GetCurSel()].second->AutomaticStepSizes();

  GetDlgItem(IDC_RADIO_AUTOSTEP)->EnableWindow(!bFixed);
  GetDlgItem(IDC_RADIO_MANUALSTEP)->EnableWindow(!bFixed);

  GetDlgItem(IDC_LBL_MAXSIZ)->EnableWindow(!bFixed && bAutoStepSizes);
  GetDlgItem(IDC_MAXSIZ)->EnableWindow(!bFixed && bAutoStepSizes);
  GetDlgItem(IDC_LBL_NUMSTEPS)->EnableWindow(!bFixed && !bAutoStepSizes);
  GetDlgItem(IDC_NROFSTEPS)->EnableWindow(!bFixed && !bAutoStepSizes);
  GetDlgItem(IDC_LBL_STEP_SCALING)->EnableWindow(!bFixed && !bAutoStepSizes);
  GetDlgItem(IDC_EDIT_STEP_SCALING_FACTOR)->EnableWindow(!bFixed && !bAutoStepSizes);

  // only these items are enabled for the initial stage
  GetDlgItem(IDC_LBL_CONVERGENCE_NORM_FACTOR)->EnableWindow(!bFixed);
  GetDlgItem(IDC_CONVERGENCE_NORM_FACTOR)->EnableWindow(!bFixed);
  GetDlgItem(IDC_LBL_MAX_ITERATIONS)->EnableWindow(!bFixed);
  GetDlgItem(IDC_MAX_ITERATIONS)->EnableWindow(!bFixed);

  GetDlgItem(IDC_LBL_ITER_SCHEME)->EnableWindow(!bFixed);
  GetDlgItem(IDC_CONSTANT)->EnableWindow(!bFixed);
  GetDlgItem(IDC_LINEAR)->EnableWindow(!bFixed);
  GetDlgItem(IDC_REGULAR)->EnableWindow(!bFixed);
  GetDlgItem(IDC_MODIFIED)->EnableWindow(!bFixed);
  GetDlgItem(IDC_SECANT)->EnableWindow(!bFixed);
  GetDlgItem(IDC_LBL_CONVNORM)->EnableWindow(!bFixed);
  GetDlgItem(IDC_FORCE)->EnableWindow(!bFixed);
  GetDlgItem(IDC_DISPLACEMENT)->EnableWindow(!bFixed);
  GetDlgItem(IDC_ENERGY)->EnableWindow(!bFixed);

  GetDlgItem(IDC_CHK_APLY_ALL)->EnableWindow(TRUE);
  GetDlgItem(IDC_DEFAULT)->EnableWindow(!bFixed);
}

void CNonlinParams::OnCheckApplyAll() 
{
  UpdateData(TRUE);

  m_cbDepletionStage.EnableWindow(!m_bApplyAll);
/*
  m_bApplyAll = !m_bApplyAll;	

  CWnd *pEdit = dynamic_cast<CWnd*>(GetDlgItem(IDC_NROFSTEPS));
  CWnd *pConvNormFac = dynamic_cast<CWnd*>(GetDlgItem(IDC_CONVERGENCE_NORM_FACTOR));
  CWnd *pMaxIter= dynamic_cast<CWnd*>(GetDlgItem(IDC_MAX_ITERATIONS));
  CWnd *pConst = dynamic_cast<CWnd*>(GetDlgItem(IDC_CONSTANT));
  CWnd *pLin = dynamic_cast<CWnd*>(GetDlgItem(IDC_LINEAR));
  CWnd *pReg = dynamic_cast<CWnd*>(GetDlgItem(IDC_REGULAR));
  CWnd *pMod = dynamic_cast<CWnd*>(GetDlgItem(IDC_MODIFIED));
  CWnd *pForce = dynamic_cast<CWnd*>(GetDlgItem(IDC_FORCE));
  CWnd *pDispl = dynamic_cast<CWnd*>(GetDlgItem(IDC_DISPLACEMENT));
  CWnd *pEnerg = dynamic_cast<CWnd*>(GetDlgItem(IDC_ENERGY));

  pEdit->EnableWindow(!m_bApplyAll);
  pConvNormFac->EnableWindow(!m_bApplyAll);
  pMaxIter->EnableWindow(!m_bApplyAll);
  pConst->EnableWindow(!m_bApplyAll);
  pLin->EnableWindow(!m_bApplyAll);
  pReg->EnableWindow(!m_bApplyAll);
  pMod->EnableWindow(!m_bApplyAll);
  pForce->EnableWindow(!m_bApplyAll);
  pDispl->EnableWindow(!m_bApplyAll);
  pEnerg->EnableWindow(!m_bApplyAll);

  UpdateData(TRUE);
  UpdateData(FALSE);
*/
}

void CNonlinParams::OnSetDefault() 
{
  CDepletionStage *pStage = m_vcDepletionStage[m_cbDepletionStage.GetCurSel()].second;
  pStage->SetDefaultNonLinParams();

//	UpdateData(TRUE);
  UpdateData(FALSE);
}

void CNonlinParams::OnLargeDeform() 
{
  UpdateData(TRUE);
  if(m_bLargeDeform && !m_model.NoCSHE())
  {
    // warn user that this combination is not advised
    _m()->msg(IDP_NL_LARGEDEFORMWITHCSHE);
  }
}

void CNonlinParams::OnAllowLinearCalc() 
{
  UpdateData(TRUE);
}

void CNonlinParams::OnHelpClicked() 
{
//	HWND hWnd = HtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_HELP_CONTEXT, IDH_NONLIN_PARAMS);
//	if(!hWnd) _m()->msg(IDP_NOHELPAVAILABLE);
}

void CNonlinParams::OnAutoStepSizes()
{
  UpdateData(TRUE);
  UpdateData(FALSE);
  UpdateStageControls();
}

void CNonlinParams::OnManualStepSizes()
{
  UpdateData(TRUE);
  UpdateData(FALSE);
  UpdateStageControls();
}
