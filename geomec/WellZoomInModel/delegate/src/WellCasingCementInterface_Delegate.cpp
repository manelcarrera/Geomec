#include "stdafx.h"

#include "AttriWellCasingCementInterface.h"
#include "WellCasingCementInterface.h"
#include "WellCasingCementInterface_Delegate.h"

CWellCasingCementInterfaceMaterialServer_Delegate::CWellCasingCementInterfaceMaterialServer_Delegate(
    CWellCasingCementInterfaceMaterialServer *wellCasingCementInterfaceMaterialServer)
    : IMaterialServerTempl_Delegate<CInterfaceMaterial, CInterfaceMaterial_Delegate,
                                    TWellCasingCementInterfaceMaterialServerParent>(
          wellCasingCementInterfaceMaterialServer),
      m_wellCasingCementInterfaceMaterialServer(wellCasingCementInterfaceMaterialServer) {}

CWellCasingCementInterface_Delegate::CWellCasingCementInterface_Delegate(
    CWellCasingCementInterface *wellCasingCementInterface)
    : TWellCasingCementInterfaceMaterialServerParent_Delegate(wellCasingCementInterface),
      m_wellCasingCementInterface(wellCasingCementInterface) {}

bool CWellCasingCementInterface_Delegate::Attributes() {
  CAttriWellCasingCementInterface dlg(*m_wellCasingCementInterface);

  return dlg.DoModal() == IDOK;
}
