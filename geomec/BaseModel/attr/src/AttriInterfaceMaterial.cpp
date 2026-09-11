#include "stdafx.h"
#include "geomec.h"

#include "AttriInterfaceMaterial.h"

#include "InterfaceMaterial.h"
#include "FieldFactor.h"
#include "GlobalMessage.h"

CAttriInterfaceMaterialDlg::CAttriInterfaceMaterialDlg(CInterfaceMaterial& mat, CWnd* pParent)
: CAttributesTemplate<CInterfaceMaterial>(IDD_ATTRI_INTERFACEMATERIAL, mat, pParent)
{
}

void CAttriInterfaceMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
  CString strName;

  double dCohesion;
  double dFriction;
  double dNormalStiff;
  double dShearStiff;

  CString strCohesionUnit;
  CString strFrictionUnit;
  CString strNormalStiffUnit;
  CString strShearStiffUnit;

  if(!pDX->m_bSaveAndValidate)
  {
  strName = Copy().Name().toStdString().c_str();

  switch(UnitNode().Unit())
  {
  case CDoubleQuantity::SI_UNIT:
      dCohesion = Copy().Cohesion();
      dFriction = Copy().Friction();
      dNormalStiff = Copy().NormalStiffness();
      dShearStiff = Copy().ShearStiffness();
      strCohesionUnit = "MPa";
      strFrictionUnit = "deg";
      strNormalStiffUnit = "MPa/m";
      strShearStiffUnit = "MPa/m";
      break;
  case CDoubleQuantity::FIELD_UNIT:
      dCohesion = Copy().Cohesion() * FF_FACTOR_STRESS;
      dFriction = Copy().Friction();
      dNormalStiff = Copy().NormalStiffness() * FF_FACTOR_STRESSGRADIENT;
      dShearStiff = Copy().ShearStiffness() * FF_FACTOR_STRESSGRADIENT;
      strCohesionUnit = "psi";
      strFrictionUnit = "deg";
      strNormalStiffUnit = "psi/ft";
      strShearStiffUnit = "psi/ft";
      break;
  default:
      assert(FALSE);
  }
  }

  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ED_COHESION, dCohesion);
  DDX_Text(pDX, IDC_UN_COHESION, strCohesionUnit);
  DDX_Text(pDX, IDC_ED_FRICTION, dFriction);
  DDX_Text(pDX, IDC_UN_FRICTION, strFrictionUnit);
  DDX_Text(pDX, IDC_ED_NORMAL_STIFF, dNormalStiff);
  DDX_Text(pDX, IDC_UN_NORMAL_STIFF, strNormalStiffUnit);
  DDX_Text(pDX, IDC_ED_SHEAR_STIFF, dShearStiff);
  DDX_Text(pDX, IDC_UN_SHEAR_STIFF, strShearStiffUnit);

  if(pDX->m_bSaveAndValidate)
  {
  if(UnitNode().Unit() == CDoubleQuantity::FIELD_UNIT)
  {
      dCohesion /= FF_FACTOR_STRESS;
      dNormalStiff /= FF_FACTOR_STRESSGRADIENT;
      dShearStiff /= FF_FACTOR_STRESSGRADIENT;
  }

  if(dCohesion < 0)
  {
      _m()->msg("Cohesion must be greater than 0");
      pDX->PrepareEditCtrl(IDC_ED_COHESION);
      pDX->Fail();
  }

  if(dFriction < 0 || dFriction >= 90)
  {
      _m()->msg("Friction angle must be at least 0 and less than 90");
      pDX->PrepareEditCtrl(IDC_ED_FRICTION);
      pDX->Fail();
  }

  if(dNormalStiff <= 0)
  {
      _m()->msg("Normal stiffness must be greater than 0");
      pDX->PrepareEditCtrl(IDC_ED_NORMAL_STIFF);
      pDX->Fail();
  }

  if(dShearStiff <= 0)
  {
      _m()->msg("Shear stiffness must be greater than 0");
      pDX->PrepareEditCtrl(IDC_ED_SHEAR_STIFF);
      pDX->Fail();
  }

  Copy().Name((LPCSTR) strName);
  Copy().SetCohesion(dCohesion);
  Copy().SetFriction(dFriction);
  Copy().SetNormalStiffness(dNormalStiff);
  Copy().SetShearStiffness(dShearStiff);
  }
}
