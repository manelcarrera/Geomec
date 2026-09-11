#include "stdafx.h"

#include "GlobalTemperature.h"
#include "GlobalTemperature_Delegate.h"
#include "InitialTemperatureDlg.h"

CGlobalTemperature_Delegate::CGlobalTemperature_Delegate(CGlobalTemperature *globalTemperature)
    : CGlobalFormationLoad_Delegate<TTemperature, CTemperatureGradientComponent>(globalTemperature),
      m_globalTemperature(globalTemperature) {}

bool CGlobalTemperature_Delegate::Attributes() {
  CInitialTemperatureDlg dlg(*m_globalTemperature);

  return (dlg.DoModal() == IDOK);
}
