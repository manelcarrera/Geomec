#include "stdafx.h"

#include "ConsistencyGuardValidationPg.h"
#include "geomec.h"
#include "BaseEntryTypes.h"
#include "ModelValidationDlg.h"
#include "ModelBase.h"
#include "FormationBase.h"
#include "MaterialServer.h"
#include "TetraModel.h"
#include "depletionstage.h"
#include "TetraBoundary.h"
#include "pressuresupportnode.h"
#include "displacementsupportnode.h"
#include "TetraFormation.h"
#include "TetraEntryTypes.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "TetraHorizonBase.h"


CConsistencyGuardValidationPg::CConsistencyGuardValidationPg(CModelBase *pModel)
  : CPropertyPage(CConsistencyGuardValidationPg::IDD)
  , m_pModel(pModel)
  , m_ValidateModel(m_pModel)
  , m_bMesh(false)
  , m_bReservoir(false)
  , m_bDepletion(false)
  , m_bMaterial(false)
  , m_bValues(false)
  , m_bBoundCond(false)
  , m_bEmptForm(false)
  , m_bEmptFault(false)
{
  //{{AFX_DATA_INIT(CModelValidationDlg)
  //}}AFX_DATA_INIT
}


void CConsistencyGuardValidationPg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CModelValidationDlg)
  DDX_Control(pDX, IDC_EMPTY_FAULT_CHECK, m_EmptyFaultsCheck);
  DDX_Control(pDX, IDC_EMPTY_FORM_CHECK, m_EmptyFormationsCheck);
  DDX_Control(pDX, IDC_BOUND_CHECK, m_BoundConditionsCheck);
  DDX_Control(pDX, IDC_MAT_CHECK, m_MaterialCheckIcon);
  DDX_Control(pDX, IDC_VALUES_CHECK, m_ValuesCheckIcon);
  DDX_Control(pDX, IDC_DEPL_CHECK, m_DepletionCheckIcon);
  DDX_Control(pDX, IDC_RES_CHECK, m_ResCheckIcon);
  DDX_Control(pDX, IDC_MESH_CHECK, m_MeshCheckIcon);
  //}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
    m_bMesh = m_ValidateModel.checkMesh();
    m_bReservoir = m_ValidateModel.checkReservoir();
    m_bDepletion = m_ValidateModel.checkExistenceDepletionStage();
    m_bMaterial = m_ValidateModel.checkMaterial();
    m_bValues = m_ValidateModel.checkValues();

    CTetraModel *pModel = dynamic_cast<CTetraModel*>(m_pModel);
    if (!pModel)
    {
      CWnd *pWnd = GetDlgItem(IDC_TETRA_GROUP);
      pWnd->EnableWindow(FALSE);
      pWnd = GetDlgItem(IDC_INPUTSTAT_BOUND);
      pWnd->EnableWindow(FALSE);
      pWnd = GetDlgItem(IDC_INPUTSTAT_EMPT_FORM);
      pWnd->EnableWindow(FALSE);
      pWnd = GetDlgItem(IDC_INPUTSTAT_EMPT_FAULT);
      pWnd->EnableWindow(FALSE);
    }
    else
    {

      if (m_bMesh)
        m_bBoundCond = m_ValidateModel.checkBoundaryConditions();
      else
        m_bBoundCond = false;
      m_bEmptForm = m_ValidateModel.checkNoEmptyFormations();
      m_bEmptFault = m_ValidateModel.checkNoEmptyFaults();
    }
  }
}


BEGIN_MESSAGE_MAP(CConsistencyGuardValidationPg, CDialog)
  //{{AFX_MSG_MAP(CModelValidationDlg)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelValidationDlg message handlers

void CConsistencyGuardValidationPg::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  DrawCheck(dc, m_DepletionCheckIcon, m_bDepletion);
  DrawCheck(dc, m_MaterialCheckIcon, m_bMaterial);
  DrawCheck(dc, m_ValuesCheckIcon, m_bValues);
  DrawCheck(dc, m_MeshCheckIcon, m_bMesh);
  DrawCheck(dc, m_ResCheckIcon, m_bReservoir);

  CTetraModel *pMod = dynamic_cast<CTetraModel*>(m_pModel);
  if (pMod)
  {
    DrawCheck(dc, m_BoundConditionsCheck, m_bBoundCond);
    DrawCheck(dc, m_EmptyFormationsCheck, m_bEmptForm);
    DrawCheck(dc, m_EmptyFaultsCheck, m_bEmptFault);
  }

  // Do not call CDialog::OnPaint() for painting messages
}

void CConsistencyGuardValidationPg::DrawCheck(CPaintDC &dc, CStatic &IconCheck, bool bCheck)
{
  RECT rect;
  IconCheck.GetWindowRect(&rect);
  ScreenToClient(&rect);

  unsigned int CheckID;
  if (bCheck)
    CheckID = IDI_CHECK_RED;
  else
    CheckID = IDI_CROSS_RED;

  DrawIconEx(dc, rect.left, rect.top, LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(CheckID)), 16, 16, 0, 0, DI_NORMAL);
}
