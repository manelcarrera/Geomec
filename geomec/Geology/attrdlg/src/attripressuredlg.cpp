// attripressuredlg.cpp : implementation file
//

#include "attripressuredlg.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "HexaFormation.h"
#include "MeshBase.h"
#include "PointSet.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriPressureDlg dialog

// ##ModelId=3BC55D650150
CAttriPressureDlg::CAttriPressureDlg(CPressure &prs, CWnd *pParent /*=NULL*/)
    : CAttriFormationLoadDlg<CPressure>(prs, IDD_ATTRI_PRESSURE, pParent), m_bOk(false) {}

// ##ModelId=3BC55D650153
void CAttriPressureDlg::DoDataExchange(CDataExchange *pDX) {
  double dContactPressure = 0;
  double dContactDepth = 0;
  double dUpperGradient = 0;
  double dLowerGradient = 0;
  double dTransPressure = 0;
  double dTransDepth = 0;
  int iTransFromPrevious = -1;
  CString strDepthUnit;
  CString strGradientUnit;
  CString strPressureUnit;
  int nType = 0;

  if (!pDX->m_bSaveAndValidate) {
    strPressureUnit = Copy().Component().UnitName(UnitNode().Unit()).toStdString().c_str();

    strDepthUnit = CString(Copy().GWC().ContactDepth().UnitName(UnitNode().Unit()).c_str());
    strGradientUnit = CString(Copy().GWC().UpperGradient().UnitName(UnitNode().Unit()).c_str());

    if (Copy().Type() == CPressure::GWC_MODE) {
      dContactPressure = Copy().GWC().ContactPressure().Value(UnitNode().Unit());
      dContactDepth = Copy().GWC().ContactDepth().Value(UnitNode().Unit());
      dUpperGradient = Copy().GWC().UpperGradient().Value(UnitNode().Unit());
      dLowerGradient = Copy().GWC().LowerGradient().Value(UnitNode().Unit());
      dTransPressure = Copy().GWC().TransitionPressure().Value(UnitNode().Unit());
      dTransDepth = Copy().GWC().TransitionDepth().Value(UnitNode().Unit());
      iTransFromPrevious = (Copy().GWC().TransitionFromPrevious()) ? 0 : 1;
      nType = 2;
    }
  }

  bool bGetGWCValues =
      (Copy().Type() == CPressure::GWC_MODE && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  if (bGetGWCValues) {
    DDX_Text(pDX, IDC_ED_CONTACT_DEPTH, dContactDepth);
    DDX_Text(pDX, IDC_ED_CONTACT_PRESSURE, dContactPressure);
    DDX_Text(pDX, IDC_ED_UPPER_GRADIENT, dUpperGradient);

    if (Copy().GWC().IsFirstGWC()) {
      DDX_Text(pDX, IDC_ED_LOWER_GRADIENT, dLowerGradient);
      DDV_MinMaxDouble(pDX, dLowerGradient, Copy().GWC().LowerGradient().MinValue(UnitNode().Unit()),
                       Copy().GWC().LowerGradient().MaxValue(UnitNode().Unit()));
    } else {
      // either set the first gradient, or keep that value
      dLowerGradient = Copy().GWC().FirstGWC().LowerGradient();
      double dDummy = dLowerGradient;
      DDX_Text(pDX, IDC_ED_LOWER_GRADIENT, dDummy);
    }

    DDV_MinMaxDouble(pDX, dContactDepth, Copy().GWC().ContactDepth().MinValue(UnitNode().Unit()),
                     Copy().GWC().ContactDepth().MaxValue(UnitNode().Unit()));
    DDV_MinMaxDouble(pDX, dContactPressure, Copy().GWC().ContactPressure().MinValue(UnitNode().Unit()),
                     Copy().GWC().ContactPressure().MaxValue(UnitNode().Unit()));
    DDV_MinMaxDouble(pDX, dUpperGradient, Copy().GWC().UpperGradient().MinValue(UnitNode().Unit()),
                     Copy().GWC().UpperGradient().MaxValue(UnitNode().Unit()));

    DDX_Radio(pDX, IDC_TRANS_PREV, iTransFromPrevious);

    if (!Copy().GWC().TransitionFromPrevious()) {
      DDX_Text(pDX, IDC_ED_TRANS_PRESSURE, dTransPressure);
      DDX_Text(pDX, IDC_ED_TRANS_DEPTH, dTransDepth);

      DDV_MinMaxDouble(pDX, dTransPressure, Copy().GWC().TransitionPressure().MinValue(UnitNode().Unit()),
                       Copy().GWC().TransitionPressure().MaxValue(UnitNode().Unit()));
      DDV_MinMaxDouble(pDX, dTransDepth, Copy().GWC().TransitionDepth().MinValue(UnitNode().Unit()),
                       Copy().GWC().TransitionDepth().MaxValue(UnitNode().Unit()));
    } else if (!pDX->m_bSaveAndValidate) {
      CString strEmpty;
      DDX_Text(pDX, IDC_ED_TRANS_PRESSURE, strEmpty);
      DDX_Text(pDX, IDC_ED_TRANS_DEPTH, strEmpty);
    }
  } else if (!pDX->m_bSaveAndValidate) {
    int iDummy = -1;
    DDX_Radio(pDX, IDC_TRANS_PREV, iDummy);

    CString strEmpty;
    DDX_Text(pDX, IDC_ED_CONTACT_DEPTH, strEmpty);
    DDX_Text(pDX, IDC_ED_CONTACT_PRESSURE, strEmpty);
    DDX_Text(pDX, IDC_ED_LOWER_GRADIENT, strEmpty);
    DDX_Text(pDX, IDC_ED_UPPER_GRADIENT, strEmpty);
    DDX_Text(pDX, IDC_ED_TRANS_PRESSURE, strEmpty);
    DDX_Text(pDX, IDC_ED_TRANS_DEPTH, strEmpty);
  }

  DDX_Text(pDX, IDC_ST_GRADIENT_UNIT_2, strGradientUnit);
  DDX_Text(pDX, IDC_ST_GRADIENT_UNIT_3, strGradientUnit);
  DDX_Text(pDX, IDC_PRESSURE_UNIT_2, strPressureUnit);
  DDX_Text(pDX, IDC_ST_DEPTH_UNIT_2, strDepthUnit);

  DDX_Text(pDX, IDC_UN_TRANS_PRESSURE, strPressureUnit);
  DDX_Text(pDX, IDC_UN_TRANS_DEPTH, strDepthUnit);

  CAttriFormationLoadDlg<CPressure>::DoDataExchange(pDX);

  if ((Copy().DistributedSize() == 0 || !Copy().DistributedOnly()) && nType == 2)
    DDX_Radio(pDX, IDC_RD_UNDEFINED, nType);

  if (pDX->m_bSaveAndValidate) {
    if (bGetGWCValues) {
      if (iTransFromPrevious == -1)
        iTransFromPrevious = (Copy().GWC().TransitionFromPrevious()) ? 0 : 1;

      if (m_bOk) // only check this stuff when user tries to end the dialog with the OK button
      {
        CLengthQuantity lqContactDepth(dContactDepth, UnitNode().Unit());
        if (!Copy().GWC().CheckContactDepth(lqContactDepth.Value(CDoubleQuantity::SI_UNIT))) {
          pDX->PrepareEditCtrl(IDC_ED_CONTACT_DEPTH); // to restore focus after messagebox
          _m()->msg(IDP_GWC_CHECK_CONTACT_DEPTH_FAIL);
          pDX->Fail();
        }

        CLengthQuantity lqTransDepth(dTransDepth, UnitNode().Unit());
        if (iTransFromPrevious && !Copy().GWC().CheckTransitionDepth(lqTransDepth.Value(CDoubleQuantity::SI_UNIT))) {
          pDX->PrepareEditCtrl(IDC_ED_TRANS_DEPTH); // to restore focus after messagebox
          _m()->msg(IDP_GWC_CHECK_TRANS_DEPTH_FAIL);
          pDX->Fail();
        }
      }

      assert(iTransFromPrevious == 0 || iTransFromPrevious == 1);

      Copy().GWC().Set(dContactPressure, dTransPressure, dContactDepth, dTransDepth, dUpperGradient, dLowerGradient,
                       (iTransFromPrevious == 0), UnitNode().Unit());
    }
  }
}

void CAttriPressureDlg::OnOK() {
  m_bOk = true;
  CAttriFormationLoadDlg<CPressure>::OnOK();
  m_bOk = false;
}

CString CAttriPressureDlg::UndefinedLabel() const { return _T("Global values"); }

BEGIN_MESSAGE_MAP(CAttriPressureDlg, CDialog)
// base class methods
ON_BN_CLICKED(IDC_RB_CONSTANT, OnConstant)
ON_BN_CLICKED(IDC_RD_UNDEFINED, OnUndefined)
ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnPreDefined)
ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtraPolate)

// additional methods for this class
ON_BN_CLICKED(IDC_RB_GWC, OnGwc)
ON_BN_CLICKED(IDC_TRANS_PREV, OnTransPrevious)
ON_BN_CLICKED(IDC_TRANS_SPEC, OnTransSpecify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriPressureDlg message handlers

void CAttriPressureDlg::OnGwc() {
  UpdateData(TRUE);
  Copy().Type(CPressure::GWC_MODE);
  UpdateControls();
}

void CAttriPressureDlg::OnTransPrevious() {
  UpdateData(TRUE);
  Copy().GWC().TransitionFromPrevious(true);
  UpdateControls();
}

void CAttriPressureDlg::OnTransSpecify() {
  UpdateData(TRUE);
  Copy().GWC().TransitionFromPrevious(false);
  UpdateControls();
}

void CAttriPressureDlg::UpdateControls() {
  const CBranchState &branchstate = (static_cast<const CModelBase &>(Copy().Model())).BranchState();
  bool bDisableAll = (branchstate.IsBranch() && branchstate.IsFixedStage(Copy().DepletionStage()));

  // Enable GWC ...
  GetDlgItem(IDC_ED_CONTACT_PRESSURE)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_CONTACT_PRESSURE)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_PRESSURE_UNIT_2)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_ED_CONTACT_DEPTH)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_CONTACT_DEPTH)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ST_DEPTH_UNIT_2)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_ED_UPPER_GRADIENT)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_UPPER_GRADIENT)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ST_GRADIENT_UNIT_2)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_ED_LOWER_GRADIENT)
      ->EnableWindow(Copy().Type() == CPressure::GWC_MODE && Copy().GWC().IsFirstGWC() && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_LBL_LOWER_GRADIENT)
      ->EnableWindow(Copy().Type() == CPressure::GWC_MODE && Copy().GWC().IsFirstGWC() && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ST_GRADIENT_UNIT_3)
      ->EnableWindow(Copy().Type() == CPressure::GWC_MODE && Copy().GWC().IsFirstGWC() && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_FRAME_TRANS)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_TRANS_PREV)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_TRANS_SPEC)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_LBL_TRANS_PRESSURE)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ED_TRANS_PRESSURE)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_UN_TRANS_PRESSURE)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_LBL_TRANS_DEPTH)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_ED_TRANS_DEPTH)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));
  GetDlgItem(IDC_UN_TRANS_DEPTH)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && (!Copy().GWC().IsFirstGWC()) &&
                     (!Copy().GWC().TransitionFromPrevious()) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  GetDlgItem(IDC_RB_GWC)->EnableWindow(!bDisableAll && (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  bool bIsMesh = ((dynamic_cast<const CModelBase &>(Copy().Model())).Mesh().IsMesh());

  unsigned int uWarning = IDS_GWC_HAS_MESH;
  if (!bIsMesh)
    uWarning = IDS_GWC_NO_MESH;

  CString str;
  str = getStringTableEntry(uWarning);

  GetDlgItem(IDC_MESH_WARNING)->SetWindowText(str);
  GetDlgItem(IDC_MESH_WARNING)
      ->EnableWindow((Copy().Type() == CPressure::GWC_MODE) && !bDisableAll &&
                     (!Copy().DistributedOnly() || Copy().DistributedSize() == 0));

  CAttriFormationLoadDlg<CPressure>::UpdateControls(); // this will UpdateData(FALSE)
}
