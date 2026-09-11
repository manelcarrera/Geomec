#include "stdafx.h"

#include "Temperature_Delegate.h"
#include "Temperature.h"
#include "AttriTemperatureDlg.h"

CTemperature_Delegate::CTemperature_Delegate(CTemperature* temperature)
: TTemperatureBase_Delegate(temperature)
, m_temperature(temperature)
{
}

bool CTemperature_Delegate::Attributes()
{
  m_temperature->AssertValid();

  CAttriTemperatureDlg dlg(*m_temperature);

  if (dlg.DoModal() == IDOK)
  {
    m_temperature->AssertValid();

    return true;
  }

  return false;
}
