#include "stdafx.h"

#include "AttriWellCasingInternalLoad.h"
#include "FemAppMainWindow.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalPressure_Delegate.h"

CWellCasingInternalPressure_Delegate::CWellCasingInternalPressure_Delegate(
    CWellCasingInternalPressure *wellCasingInternalPressure)
    : IWellCasingInternalLoad_Delegate<TPressure>(wellCasingInternalPressure),
      m_wellCasingInternalPressure(wellCasingInternalPressure) {}

bool CWellCasingInternalPressure_Delegate::Attributes() {
  CAttriWellCasingInternalLoadDlg<CWellCasingInternalPressure, CPressureGradientQuantity> dlg(
      *m_wellCasingInternalPressure, FemAppGetMainWnd());
  bool bRet = (dlg.DoModal() == IDOK);

  if (bRet) {
    m_wellCasingInternalPressure->Modified();
  }

  return bRet;
}
