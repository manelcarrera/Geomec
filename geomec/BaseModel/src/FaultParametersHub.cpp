#include "stdafx.h"

#include "FaultParametersHub.h"
#include "EditFaultSettings.h"

CFaultParametersHub::CFaultParametersHub(CHorizonBase& horizonBase)
: m_horizonBase(horizonBase)
{
}

void CFaultParametersHub::attributes()
{
  CEditFaultSettings
  editFaultSettings(&(m_horizonBase.Model()), &m_horizonBase);

  editFaultSettings.DoModal();
}

bool CFaultParametersHub::hasAttributes() const
{
  return ((&(m_horizonBase.Model()) != 0) &&
  CEditFaultSettings::hasFaultFractures(&(m_horizonBase.Model())));
}
