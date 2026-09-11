#include "StdAfx.h"
#include "geomec.h"

#include "AttriWellCasingRTCI.h"
#include "GlobalMessage.h"

CAttriWellCasingRTCI::CAttriWellCasingRTCI(CWellCasingRTCI& rtci, CWnd* pParent)
: CAttributesTemplate<CWellCasingRTCI>(IDD_ATTRI_WELLCASINGRTCI, rtci, pParent)
{
}

void CAttriWellCasingRTCI::DoDataExchange(CDataExchange* pDX)
{
  CString strName;
  double dAngle;
  double dAzimuth;
  int nDirection;

  if(!pDX->m_bSaveAndValidate)
  {
    strName = Copy().Name().toStdString().c_str();
    dAngle = Copy().Angle();
    dAzimuth = Copy().StartAzimuth();
    nDirection = (Copy().WindingDirection() == CWellCasingRTCI::RD_CLOCKWISE ? 0 : 1);
  }

  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ED_ANGLE, dAngle);
  DDX_Text(pDX, IDC_ED_AZIMUTH, dAzimuth);
  DDX_Radio(pDX, IDC_RD_CLOCKWISE, nDirection);

  if(pDX->m_bSaveAndValidate)
  {
    if(strName.IsEmpty())
    {
      _m()->msg("Please provide a valid name");
      pDX->PrepareEditCtrl(IDC_ED_NAME);
      pDX->Fail();
    }

    if(dAngle < 5)
    {
      _m()->msg("Angle with horizontal plane must be at least 5");
      pDX->PrepareEditCtrl(IDC_ED_ANGLE);
      pDX->Fail();
    }

    if(dAngle >= 90)
    {
      _m()->msg("Angle with horizontal plane must be less than 90");
      pDX->PrepareEditCtrl(IDC_ED_ANGLE);
      pDX->Fail();
    }

    if(dAzimuth < 0)
    {
      _m()->msg("Start azimuth must be at least 0");
      pDX->PrepareEditCtrl(IDC_ED_AZIMUTH);
      pDX->Fail();
    }

    if(dAzimuth >= 360)
    {
      _m()->msg("Start azimuth must be less than 360");
      pDX->PrepareEditCtrl(IDC_ED_AZIMUTH);
      pDX->Fail();
    }

    Copy().Name((LPCSTR) strName);
    Copy().Angle(dAngle);
    Copy().StartAzimuth(dAzimuth);
    Copy().WindingDirection((nDirection == 0 ? CWellCasingRTCI::RD_CLOCKWISE : CWellCasingRTCI::RD_ANTICLOCKWISE));
  }
}
