#include "stdafx.h"

#include "FormationPlane_Delegate.h"
#include "FormationPlane.h"
#include "AttriFormPlane.h"

CFormationPlane_Delegate::CFormationPlane_Delegate(
  CFormationPlane* formationPlane)
: CColorNode_Delegate(formationPlane)
, m_formationPlane(formationPlane)
{
}

bool CFormationPlane_Delegate::Attributes()
{
  CAttriFormPlane dlg(*m_formationPlane);

  return dlg.DoModal() == IDOK;
}

bool CFormationPlane_Delegate::CanDestroy() const
{
  return m_formationPlane->CanDestroy();
}
