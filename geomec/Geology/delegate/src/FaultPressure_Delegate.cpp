#include "stdafx.h"

#include "FaultPressure_Delegate.h"
#include "FaultPressure.h"
#include "AttriFaultPressureDlg.h"

CFaultPressure_Delegate::CFaultPressure_Delegate(CFaultPressure* faultPressure)
: IValueComposite_Delegate(faultPressure)
, m_faultPressure(faultPressure)
{
}

bool CFaultPressure_Delegate::Attributes()
{
  CAttriFaultPressureDlg dlg(*m_faultPressure);

  if (dlg.DoModal() == IDOK)
  {
  m_faultPressure->Component().Modified();

  return true;
  }

  return false;
}
