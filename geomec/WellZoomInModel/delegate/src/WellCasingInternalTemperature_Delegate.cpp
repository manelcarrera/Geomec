#include "stdafx.h"

#include "AttriWellCasingInternalLoad.h"
#include "FemAppMainWindow.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingInternalTemperature_Delegate.h"

CWellCasingInternalTemperature_Delegate::CWellCasingInternalTemperature_Delegate(
    CWellCasingInternalTemperature *wellCasingInternalTemperature)
    : IWellCasingInternalLoad_Delegate<TTemperature>(wellCasingInternalTemperature),
      m_wellCasingInternalTemperature(wellCasingInternalTemperature) {}

bool CWellCasingInternalTemperature_Delegate::Attributes() {
  CAttriWellCasingInternalLoadDlg<CWellCasingInternalTemperature, CTemperatureGradientQuantity> dlg(
      *m_wellCasingInternalTemperature, FemAppGetMainWnd());
  bool bRet = (dlg.DoModal() == IDOK);

  if (bRet) {
    m_wellCasingInternalTemperature->Modified();
  }

  return bRet;
}
