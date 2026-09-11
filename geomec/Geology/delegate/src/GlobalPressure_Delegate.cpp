#include "stdafx.h"

#include "AttriGlobalPressureDlg.h"
#include "GlobalPressure.h"
#include "GlobalPressure_Delegate.h"

CGlobalPressure_Delegate::CGlobalPressure_Delegate(CGlobalPressure *globalPressure)
    : CGlobalFormationLoad_Delegate<TPressure, CComponentConstant<TPressure>>(globalPressure),
      m_globalPressure(globalPressure) {}

bool CGlobalPressure_Delegate::Attributes() {
  CAttriGlobalPressureDlg dlg(*m_globalPressure);

  return dlg.DoModal() == IDOK;
}
