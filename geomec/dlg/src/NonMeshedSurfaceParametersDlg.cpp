#include "StdAfx.h"

#include "NonMeshedSurfaceParametersDlg.h"

CNonMeshedSurfaceParametersDlg::CNonMeshedSurfaceParametersDlg(CNonMeshedSurfaceParametersNode& parameters, CDialog* pParent)
: CAttributesTemplate<CNonMeshedSurfaceParametersNode>(IDD_NONMESHEDSURFACEPARAMETERS, parameters, pParent)
{
}

void CNonMeshedSurfaceParametersDlg::DoDataExchange(CDataExchange* pDX)
{
  double dCohesion;
  double dFrictionAngle;

  if(!pDX->m_bSaveAndValidate)
  {
    dCohesion = Copy().Cohesion().Value(UnitNode().Unit());
    dFrictionAngle = Copy().FrictionAngle().Value(UnitNode().Unit());

    CString strUnitCohesion = Copy().Cohesion().UnitName(UnitNode().Unit()).c_str();
    CString strUnitFrictionAngle = Copy().FrictionAngle().UnitName(UnitNode().Unit()).c_str();

    DDX_Text(pDX, IDC_UNIT_COHESION, strUnitCohesion);
    DDX_Text(pDX, IDC_UNIT_FRICTIONANGLE, strUnitFrictionAngle);
  }

  DDX_Text(pDX, IDC_EDIT_COHESION, dCohesion);
  DDX_Text(pDX, IDC_EDIT_FRICTIONANGLE, dFrictionAngle);

  if(pDX->m_bSaveAndValidate)
  {
    if(dCohesion < 0)
    {
      AfxMessageBox("Cohesion must be greater than or equal to 0");
      pDX->PrepareEditCtrl(IDC_EDIT_COHESION);
      pDX->Fail();
    }

    if(dFrictionAngle < 0 || dFrictionAngle >= 90)
    {
      AfxMessageBox("Friction Angle must be greater than or equal to 0 and less than 90");
      pDX->PrepareEditCtrl(IDC_EDIT_FRICTIONANGLE);
      pDX->Fail();
    }

    Copy().Cohesion().Value(dCohesion, UnitNode().Unit());
    Copy().FrictionAngle().Value(dFrictionAngle, UnitNode().Unit());
  }
}
