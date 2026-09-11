// D:\users\wedx\svn_trunk\geomec3\SRC\FaultInitSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "FaultInitSettingsDlg.h"

#include "ModelBase.h"


// CFaultInitSettingsDlg dialog

CFaultInitSettingsDlg::CFaultInitSettingsDlg(CModelBase& model, CWnd* pParent /*=NULL*/)
: CDialog(IDD_FAULTINITIALIZATIONSETTINGS, pParent),
  m_model(model)
{
  switch(model.FaultInit())
  {
  case CModelBase::FI_FULLSLIP:
  m_nMethod = 0;
  break;
  case CModelBase::FI_MOD_COHESION:
  m_nMethod = 1;
  break;
  case CModelBase::FI_MOD_FLUIDPRESSURE:
  m_nMethod = 2;
  break;
  default:
  assert(FALSE);
  }
}

void CFaultInitSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO_FULLSLIP, m_nMethod);
}

void CFaultInitSettingsDlg::OnOK()
{
  UpdateData(TRUE);

  switch(m_nMethod)
  {
  case 0:
  m_model.FaultInit(CModelBase::FI_FULLSLIP);
  break;
  case 1:
  m_model.FaultInit(CModelBase::FI_MOD_COHESION);
  break;
  case 2:
  m_model.FaultInit(CModelBase::FI_MOD_FLUIDPRESSURE);
  break;
  default:
  assert(FALSE);
  }

  CDialog::OnOK();
}
