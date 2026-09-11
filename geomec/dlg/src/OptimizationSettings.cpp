#include "stdafx.h"

#include "GeomecDoc.h"
#include "Global.h"
#include "HexaMesh.h"
#include "OptimizationSettings.h"
#include "TetraMesh.h"
#include "ValueMapper.h"
#include "resource.h"

COptimizationSettingsDlg::COptimizationSettingsDlg(CWnd *pParent)
    : CDialog(IDD_OPTIMIZATION_DLG, pParent), m_pTetraMesh(0), m_pHexaMesh(0), m_nVisualMapping(0), m_nDianaMapping(0),
      m_nTetraMesher(0), m_nHexaMesher(0), m_nDSFStressSmoothening(0), m_nDSFInterSurfGeneration(0), m_dsa(0) {}

BOOL COptimizationSettingsDlg::OnInitDialog() {
  GetDlgItem(IDC_CHECK_DIANA_SEPARATED_PROCESS)->ShowWindow(_g->dsa_available() ? SW_SHOW : SW_HIDE);

  CValueMapper *vm = CValueMapper::instance();
  if (vm->UseLegacyCode())
    m_nVisualMapping = 0;
  else if (vm->AllowParallelMapping())
    m_nVisualMapping = 2;
  else
    m_nVisualMapping = 1;

  if (vm->DianaDistributedMapping())
    m_nDianaMapping = 1;
  else
    m_nDianaMapping = 0;

#ifdef HAVE_CM2_MESHER
  m_pTetraMesh = dynamic_cast<CTetraMesh *>(&static_cast<CModelBase *>(GetGeomecDoc()->Model())->Mesh());
  if (m_pTetraMesh && m_pTetraMesh->CurrentMesher() == "CM2")
    m_nTetraMesher = 1;
  else
#endif
    m_nTetraMesher = 0;

  m_pHexaMesh = dynamic_cast<CHexaMesh *>(&static_cast<CModelBase *>(GetGeomecDoc()->Model())->Mesh());
  if (m_pHexaMesh && m_pHexaMesh->CurrentMesher() == "NextGen")
    m_nHexaMesher = 1;
  else
    m_nHexaMesher = 0;

  m_nDSFStressSmoothening = static_cast<CModelBase *>(GetGeomecDoc()->Model())->UseStressSmootheningOnDSF();
  m_nDSFInterSurfGeneration = static_cast<CModelBase *>(GetGeomecDoc()->Model())->DSFIntermediateSurfaceType();
  m_dsa = _g->dsa();

  CDialog::OnInitDialog();

  return TRUE;
}

void COptimizationSettingsDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO_VIS_NONE, m_nVisualMapping);
  DDX_Radio(pDX, IDC_RADIO_DIA_NO, m_nDianaMapping);
  DDX_Radio(pDX, IDC_RADIO_MESH_FGV, m_nTetraMesher);
  DDX_Radio(pDX, IDC_RADIO_MESH_HEXAMESHER_OLD, m_nHexaMesher);
  DDX_Check(pDX, IDC_CHECK_DIANA_SEPARATED_PROCESS, m_dsa);
  DDX_Check(pDX, IDC_CHECK_DSF_STRESS_SMOOTHENING, m_nDSFStressSmoothening);
  DDX_Check(pDX, IDC_CHECK_DSF_INTERSURF_TYPE, m_nDSFInterSurfGeneration);

  if (pDX->m_bSaveAndValidate) {
    CModelBase *pModel = static_cast<CModelBase *>(GetGeomecDoc()->Model());

    bool bInvalidateResults = false;
    bool bInvalidateMesh = false;

    if (m_pTetraMesh) {
      switch (m_nTetraMesher) {
      case 0:
        if (m_pTetraMesh->CurrentMesher() == "CM2")
          bInvalidateMesh = true;
        break;
      case 1:
        if (m_pTetraMesh->CurrentMesher() == "")
          bInvalidateMesh = true;
        break;
      }
    }

    if (m_pHexaMesh) {
      switch (m_nHexaMesher) {
      case 0:
        if (m_pHexaMesh->CurrentMesher() == "NextGen")
          bInvalidateMesh = true;
        break;
      case 1:
        if (m_pHexaMesh->CurrentMesher() == "")
          bInvalidateMesh = true;
        break;
      }
    }

    if (pModel->UseStressSmootheningOnDSF() != (m_nDSFStressSmoothening == 1))
      bInvalidateResults = pModel->ResultRegister().ResultsAvailable();

    QString msg;
    if (bInvalidateMesh)
      msg = "The mesh will be invalidated. Proceed?";
    else if (bInvalidateResults)
      msg = "Results will be invalidated. Proceed?";

    if (!msg.isEmpty() && _m()->msg(msg, MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
      pDX->Fail();
  }
}

void COptimizationSettingsDlg::OnOK() {
  UpdateData(TRUE);

  CValueMapper *vm = CValueMapper::instance();
  switch (m_nVisualMapping) {
  case 0:
    vm->UseLegacyCode(true);
    break;
  case 1:
    vm->UseLegacyCode(false);
    break;
  case 2:
    vm->AllowParallelMapping(true);
    break;
  }
  switch (m_nDianaMapping) {
  case 0:
    vm->DianaDistributedMapping(false);
    break;
  case 1:
    vm->DianaDistributedMapping(true);
    break;
  }
  if (m_pTetraMesh) {
    switch (m_nTetraMesher) {
    case 0:
      m_pTetraMesh->SwitchMesher("");
      break;
    case 1:
      m_pTetraMesh->SwitchMesher("CM2");
      break;
    }
  }

  if (m_pHexaMesh) {
    switch (m_nHexaMesher) {
    case 0:
      m_pHexaMesh->SwitchMesher("");
      break;
    case 1:
      m_pHexaMesh->SwitchMesher("NextGen");
      break;
    }
  }

  static_cast<CModelBase *>(GetGeomecDoc()->Model())->UseStressSmootheningOnDSF(m_nDSFStressSmoothening == 1);
  static_cast<CModelBase *>(GetGeomecDoc()->Model())->DSFIntermediateSurfaceType(m_nDSFInterSurfGeneration);

  CDialog::OnOK();
}
