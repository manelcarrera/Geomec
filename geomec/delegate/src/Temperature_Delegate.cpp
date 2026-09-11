#include "stdafx.h"

#include "AttriTemperatureDlg.h"
#include "Temperature.h"
#include "Temperature_Delegate.h"

CTemperature_Delegate::CTemperature_Delegate(CTemperature *temperature)
    : TTemperatureBase_Delegate(temperature), m_temperature(temperature) {}

bool CTemperature_Delegate::Attributes() {
  m_temperature->AssertValid();

  CAttriTemperatureDlg dlg(*m_temperature);

  if (dlg.DoModal() == IDOK) {
    m_temperature->AssertValid();

    return true;
  }

  return false;
}
