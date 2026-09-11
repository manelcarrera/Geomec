#include "stdafx.h"
#include "geomec.h"

#include "AttriFaultParameters.h"
#include "FaultParametersNode.h"
#include "BranchState.h"
#include "HorizonBase.h"
#include "FieldFactor.h"
#include "DepletionStage.h"
#include "GlobalMessage.h"

CAttriFaultParameters::CAttriFaultParameters(CFaultParametersNode& node, CWnd* pParent)
: CAttributesTemplate<CFaultParametersNode>(IDD_ATTRI_FAULT_PARAMETERS, node, pParent)
{
}

void CAttriFaultParameters::DoDataExchange(CDataExchange* pDX)
{
  int nCohesionExtrapolated;
  int nCohesionSameAsPrev;
  int nFrictionExtrapolated;
  int nFrictionSameAsPrev;
  int nDStiffSameAsPrev;

  CString strCohesionUnit;
  CString strFrictionUnit;
  CString strDStiffUnit;
  CString strEmpty;

  double dCohesionUnitFactor;
  double dDStiffUnitFactor;
  if(UnitNode().Unit() == CQuantity::SI_UNIT)
  {
  dCohesionUnitFactor = 1;
  strDStiffUnit = "MPa/m";
  dDStiffUnitFactor = 1;
  }
  else
  {
  dCohesionUnitFactor = FF_FACTOR_PRESSURE;
  strDStiffUnit = "psi/ft";
  dDStiffUnitFactor = FF_FACTOR_PRESSURE / FF_FACTOR_LENGTH;
  }

  double dCohesion;
  double dFriction;
  double dDStiffNormal;
  double dDStiffShear;

  if(!pDX->m_bSaveAndValidate)
  {
  strCohesionUnit = Copy().Cohesion().UnitName(UnitNode().Unit()).c_str();
  strFrictionUnit = Copy().FrictionAngle().UnitName(UnitNode().Unit()).c_str();

  dCohesion = Copy().Cohesion() * dCohesionUnitFactor;
  dFriction = Copy().FrictionAngle();
  dDStiffNormal = Copy().UserDStiff1() * dDStiffUnitFactor;
  dDStiffShear = Copy().UserDStiff2() * dDStiffUnitFactor;

  if(Copy().Fault().SlipType() != CHorizonBase::SLIP)
  {
      if(Copy().DistributedCohesion())
      {
    if(Copy().CohesionExtrapolated())
    {
          nCohesionExtrapolated = 0;
          nCohesionSameAsPrev = -1;
    }
    else
    {
          nCohesionExtrapolated = 1;
          if(Copy().CohesionSameAsPrevious())
      nCohesionSameAsPrev = 0;
          else
      nCohesionSameAsPrev = 1;
    }
      }
      else
      {
    nCohesionExtrapolated = -1;
    if(Copy().CohesionSameAsPrevious() && !Copy().Stage().Initial())
          nCohesionSameAsPrev = 0;
    else
          nCohesionSameAsPrev = 1;
      }
  }
  else
  {
      nCohesionExtrapolated = -1;
      nCohesionSameAsPrev = -1;
  }

  if(Copy().Fault().SlipType() != CHorizonBase::SLIP)
  {
      if(Copy().DistributedFriction())
      {
    if(Copy().FrictionExtrapolated())
    {
          nFrictionExtrapolated = 0;
          nFrictionSameAsPrev = -1;
    }
    else
    {
          nFrictionExtrapolated = 1;
          if(Copy().FrictionSameAsPrevious())
      nFrictionSameAsPrev = 0;
          else
      nFrictionSameAsPrev = 1;
    }
      }
      else
      {
    nFrictionExtrapolated = -1;
    if(Copy().FrictionSameAsPrevious() && !Copy().Stage().Initial())
          nFrictionSameAsPrev = 0;
    else
          nFrictionSameAsPrev = 1;
      }
  }
  else
  {
      nFrictionExtrapolated = -1;
      nFrictionSameAsPrev = -1;
  }

  if(Copy().Fault().SlipType() == CHorizonBase::USER || Copy().Fault().SlipType() == CHorizonBase::FRACTURE)
  {
      if(Copy().DStiffSameAsPrevious() && !Copy().Stage().Initial())
    nDStiffSameAsPrev = 0;
      else
    nDStiffSameAsPrev = 1;
  }
  else
  {
      nDStiffSameAsPrev = -1;
  }

  DDX_Text(pDX, IDC_UN_COHESION, strCohesionUnit);
  DDX_Text(pDX, IDC_UN_FRICTION, strFrictionUnit);
  DDX_Text(pDX, IDC_UN_DSTIFF_NORMAL, strDStiffUnit);
  DDX_Text(pDX, IDC_UN_DSTIFF_SHEAR, strDStiffUnit);
  }

  DDX_Radio(pDX, IDC_RADIO_COHESION_EXTRAPOLATED, nCohesionExtrapolated);
  DDX_Radio(pDX, IDC_RADIO_COHESION_SAME_AS_PREV, nCohesionSameAsPrev);
  DDX_Radio(pDX, IDC_RADIO_FRICTION_EXTRAPOLATED, nFrictionExtrapolated);
  DDX_Radio(pDX, IDC_RADIO_FRICTION_SAME_AS_PREV, nFrictionSameAsPrev);
  DDX_Radio(pDX, IDC_RADIO_DSTIFF_SAME_AS_PREV, nDStiffSameAsPrev);

  if(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Copy().CohesionSameAsPrevious())
  {
  DDX_Text(pDX, IDC_ED_COHESION, dCohesion);
    DDV_MinMaxDouble(pDX, 
                     dCohesion, 
                     Copy().Cohesion().MinValue(UnitNode().Unit()),
                     Copy().Cohesion().MaxValue(UnitNode().Unit()));
    // Prevent the cohesion from being less than zero
    if(dCohesion < 0)
    {
      _m()->msg("The cohesion should be at least 0");
      pDX->Fail();
    }
  }
  else if(!pDX->m_bSaveAndValidate)
  {
  DDX_Text(pDX, IDC_ED_COHESION, strEmpty);
  }

  if(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Copy().FrictionSameAsPrevious())
  {
  DDX_Text(pDX, IDC_ED_FRICTION, dFriction);
    DDV_MinMaxDouble(pDX, 
                     dFriction, 
                     Copy().FrictionAngle().MinValue(UnitNode().Unit()),
                     Copy().FrictionAngle().MaxValue(UnitNode().Unit()));

    // Prevent the friction angle from being less than or equal to zero
    if(dFriction <= 0)
    {
      _m()->msg("The friction angle should be greater than 0");
      pDX->Fail();
    }
  }
  else if(!pDX->m_bSaveAndValidate)
  {
  DDX_Text(pDX, IDC_ED_FRICTION, strEmpty);
  }

  if((Copy().Fault().SlipType() == CHorizonBase::USER || Copy().Fault().SlipType() == CHorizonBase::FRACTURE) && !Copy().DStiffSameAsPrevious())
  {
  DDX_Text(pDX, IDC_ED_DSTIFF_NORMAL, dDStiffNormal);
  if(dDStiffNormal < 0)
  {
      _m()->msg("The elastic normal stiffness should be at least 0");
      pDX->Fail();
  }

  DDX_Text(pDX, IDC_ED_DSTIFF_SHEAR, dDStiffShear);
  if(dDStiffShear <= 0)
  {
      _m()->msg("The elastic shear stiffness should be greater than 0");
      pDX->Fail();
  }
  }
  else if(!pDX->m_bSaveAndValidate)
  {
  DDX_Text(pDX, IDC_ED_DSTIFF_NORMAL, strEmpty);
  DDX_Text(pDX, IDC_ED_DSTIFF_SHEAR, strEmpty);
  }

  if(pDX->m_bSaveAndValidate)
  {
  if(Copy().Fault().SlipType() != CHorizonBase::SLIP)
  {
      if((!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Copy().CohesionSameAsPrevious())
    Copy().Cohesion(dCohesion / dCohesionUnitFactor);

      if((!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Copy().FrictionSameAsPrevious())
    Copy().FrictionAngle(dFriction);
  }

  if(Copy().Fault().SlipType() == CHorizonBase::USER || Copy().Fault().SlipType() == CHorizonBase::FRACTURE)
  {
      if(!Copy().DStiffSameAsPrevious())
      {
    Copy().UserDStiff1(dDStiffNormal / dDStiffUnitFactor);
    Copy().UserDStiff2(dDStiffShear / dDStiffUnitFactor);
      }
  }
  }
}

BOOL CAttriFaultParameters::OnInitDialog()
{
  CAttributesTemplate<CFaultParametersNode>::OnInitDialog();

  EnableControls();

  return FALSE;
}

BEGIN_MESSAGE_MAP(CAttriFaultParameters, CDialog)
  ON_BN_CLICKED(IDC_RADIO_COHESION_EXTRAPOLATED, OnCohesionExtrapolated)
  ON_BN_CLICKED(IDC_RADIO_COHESION_USE, OnCohesionUse)
  ON_BN_CLICKED(IDC_RADIO_COHESION_SAME_AS_PREV, OnCohesionSameAsPrev)
  ON_BN_CLICKED(IDC_RADIO_COHESION_VALUE, OnCohesionValue)
  ON_BN_CLICKED(IDC_RADIO_FRICTION_EXTRAPOLATED, OnFrictionExtrapolated)
  ON_BN_CLICKED(IDC_RADIO_FRICTION_USE, OnFrictionUse)
  ON_BN_CLICKED(IDC_RADIO_FRICTION_SAME_AS_PREV, OnFrictionSameAsPrev)
  ON_BN_CLICKED(IDC_RADIO_FRICTION_VALUE, OnFrictionValue)
  ON_BN_CLICKED(IDC_RADIO_DSTIFF_SAME_AS_PREV, OnDStiffSameAsPrev)
  ON_BN_CLICKED(IDC_RADIO_DSTIFF_VALUE, OnDStiffValue)
END_MESSAGE_MAP()

void CAttriFaultParameters::OnCohesionExtrapolated()
{
  UpdateData(TRUE);
  Copy().CohesionExtrapolated(true);
  EnableControls();
}

void CAttriFaultParameters::OnCohesionUse()
{
  UpdateData(TRUE);
  Copy().CohesionExtrapolated(false);
  EnableControls();
}

void CAttriFaultParameters::OnCohesionSameAsPrev()
{
  UpdateData(TRUE);
  Copy().CohesionSameAsPrevious(true);
  EnableControls();
}

void CAttriFaultParameters::OnCohesionValue()
{
  UpdateData(TRUE);
  Copy().CohesionSameAsPrevious(false);
  EnableControls();
}

void CAttriFaultParameters::OnFrictionExtrapolated()
{
  UpdateData(TRUE);
  Copy().FrictionExtrapolated(true);
  EnableControls();
}

void CAttriFaultParameters::OnFrictionUse()
{
  UpdateData(TRUE);
  Copy().FrictionExtrapolated(false);
  EnableControls();
}

void CAttriFaultParameters::OnFrictionSameAsPrev()
{
  UpdateData(TRUE);
  Copy().FrictionSameAsPrevious(true);
  EnableControls();
}

void CAttriFaultParameters::OnFrictionValue()
{
  UpdateData(TRUE);
  Copy().FrictionSameAsPrevious(false);
  EnableControls();
}

void CAttriFaultParameters::OnDStiffSameAsPrev()
{
  UpdateData(TRUE);
  Copy().DStiffSameAsPrevious(true);
  EnableControls();
}

void CAttriFaultParameters::OnDStiffValue()
{
  UpdateData(TRUE);
  Copy().DStiffSameAsPrevious(false);
  EnableControls();
}

void CAttriFaultParameters::EnableControls()
{
  GetDlgItem(IDC_RADIO_COHESION_EXTRAPOLATED)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && Copy().DistributedCohesion() && !Locked());
  GetDlgItem(IDC_RADIO_COHESION_USE)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && Copy().DistributedCohesion() && !Locked());
  GetDlgItem(IDC_RADIO_COHESION_SAME_AS_PREV)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && !Copy().Stage().Initial() && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Locked());
  GetDlgItem(IDC_RADIO_COHESION_VALUE)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Locked());
  GetDlgItem(IDC_LBL_COHESION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Copy().CohesionSameAsPrevious() && !Locked());
  GetDlgItem(IDC_ED_COHESION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Copy().CohesionSameAsPrevious() && !Locked());
  GetDlgItem(IDC_UN_COHESION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedCohesion() || !Copy().CohesionExtrapolated()) && !Copy().CohesionSameAsPrevious() && !Locked());

  GetDlgItem(IDC_RADIO_FRICTION_EXTRAPOLATED)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && Copy().DistributedFriction() && !Locked());
  GetDlgItem(IDC_RADIO_FRICTION_USE)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && Copy().DistributedFriction() && !Locked());
  GetDlgItem(IDC_RADIO_FRICTION_SAME_AS_PREV)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && !Copy().Stage().Initial() && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Locked());
  GetDlgItem(IDC_RADIO_FRICTION_VALUE)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Locked());
  GetDlgItem(IDC_LBL_FRICTION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Copy().FrictionSameAsPrevious() && !Locked());
  GetDlgItem(IDC_ED_FRICTION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Copy().FrictionSameAsPrevious() && !Locked());
  GetDlgItem(IDC_UN_FRICTION)->EnableWindow(Copy().Fault().SlipType() != CHorizonBase::SLIP && (!Copy().DistributedFriction() || !Copy().FrictionExtrapolated()) && !Copy().FrictionSameAsPrevious() && !Locked());

  bool bEnableDStiff = (Copy().Fault().SlipType() == CHorizonBase::USER || Copy().Fault().SlipType() == CHorizonBase::FRACTURE) && !Locked();
  GetDlgItem(IDC_FRAME_DSTIFF)->EnableWindow(bEnableDStiff);
  GetDlgItem(IDC_RADIO_DSTIFF_SAME_AS_PREV)->EnableWindow(bEnableDStiff && !Copy().Stage().Initial());
  GetDlgItem(IDC_RADIO_DSTIFF_VALUE)->EnableWindow(bEnableDStiff);
  GetDlgItem(IDC_LBL_DSTIFF_NORMAL)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());
  GetDlgItem(IDC_ED_DSTIFF_NORMAL)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());
  GetDlgItem(IDC_UN_DSTIFF_NORMAL)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());
  GetDlgItem(IDC_LBL_DSTIFF_SHEAR)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());
  GetDlgItem(IDC_ED_DSTIFF_SHEAR)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());
  GetDlgItem(IDC_UN_DSTIFF_SHEAR)->EnableWindow(bEnableDStiff && !Copy().DStiffSameAsPrevious());

  UpdateData(FALSE);
}

bool CAttriFaultParameters::Locked()
{
  const CModelBase& model = static_cast<const CModelBase&>(Copy().Model());
  return model.BranchState().IsFixedStage(Copy().Stage());
}
