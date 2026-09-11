#include "stdafx.h"

#include "AttriPressureDlg.h"
#include "Pressure.h"
#include "Pressure_Delegate.h"

CPressure_Delegate::CPressure_Delegate(CPressure *pressure) : TPressureBase_Delegate(pressure), m_pressure(pressure) {}

bool CPressure_Delegate::Attributes() {
  m_pressure->AssertValid();

  CAttriPressureDlg dlg(*m_pressure);

  if (dlg.DoModal() == IDOK) {
    m_pressure->AssertValid();

    return true;
  }

  return false;
}
