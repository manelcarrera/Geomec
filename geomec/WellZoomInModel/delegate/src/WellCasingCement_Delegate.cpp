#include "stdafx.h"

#include "WellCasingCement_Delegate.h"
#include "WellCasingCement.h"
#include "AttriNameColorDlg.h"

CWellCasingCement_Delegate::CWellCasingCement_Delegate(
  CWellCasingCement* wellCasingCement)
: CFormationBase_Delegate(wellCasingCement)
, m_wellCasingCement(wellCasingCement)
{
}

bool CWellCasingCement_Delegate::Attributes()
{
  CAttriNameColorDlg <CWellCasingCement> dlg(*m_wellCasingCement);

  return dlg.DoModal() == IDOK;
}
