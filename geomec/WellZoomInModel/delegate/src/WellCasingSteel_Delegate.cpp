#include "stdafx.h"

#include "WellCasingSteel_Delegate.h"
#include "WellCasingSteel.h"
#include "WellCasingSteelMaterial_Delegate.h"
#include "AttriNameColorDlg.h"

CWellCasingMaterialServer_Delegate::CWellCasingMaterialServer_Delegate(
  CWellCasingMaterialServer* wellCasingMaterialServer)
: IMaterialServerTempl_Delegate <CWellCasingSteelMaterial,
  CWellCasingSteelMaterial_Delegate,
  TWellCasingMaterialServerParent> (wellCasingMaterialServer)
, m_wellCasingMaterialServer(wellCasingMaterialServer)
{
}

CWellCasingSteel_Delegate::CWellCasingSteel_Delegate(
  CWellCasingSteel* wellCasingSteel)
: TWellCasingMaterialServerParent_Delegate(wellCasingSteel)
, m_wellCasingSteel(wellCasingSteel)
{
}

bool CWellCasingSteel_Delegate::Attributes()
{
  CAttriNameColorDlg <CWellCasingSteel> dlg(*m_wellCasingSteel);

  return dlg.DoModal() == IDOK;
}
