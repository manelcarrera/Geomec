#include "stdafx.h"

#include "WellCasingInternalPressure_Delegate.h"
#include "WellCasingInternalPressure.h"
#include "AttriWellCasingInternalLoad.h"
#include "FemAppMainWindow.h"

CWellCasingInternalPressure_Delegate::CWellCasingInternalPressure_Delegate(
  CWellCasingInternalPressure* wellCasingInternalPressure)
: IWellCasingInternalLoad_Delegate <TPressure> (wellCasingInternalPressure)
, m_wellCasingInternalPressure(wellCasingInternalPressure)
{
}

bool CWellCasingInternalPressure_Delegate::Attributes()
{
  CAttriWellCasingInternalLoadDlg <CWellCasingInternalPressure,
  CPressureGradientQuantity> dlg(*m_wellCasingInternalPressure,
      FemAppGetMainWnd());
  bool bRet = (dlg.DoModal() == IDOK);

  if (bRet)
  {
  m_wellCasingInternalPressure->Modified();
  }

  return bRet;
}
