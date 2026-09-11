#include "stdafx.h"

#include "AttriStrainDlg.h"
#include "StrainLoad.h"
#include "StrainLoad_Delegate.h"

CStrainLoad_Delegate::CStrainLoad_Delegate(CStrainLoad *strainLoad)
    : TStrainLoadBase_Delegate(strainLoad), m_StrainLoad(strainLoad) {}

bool CStrainLoad_Delegate::Attributes() {
  CAttriStrainDlg dlg(*m_StrainLoad);

  return (dlg.DoModal() == IDOK);
}
