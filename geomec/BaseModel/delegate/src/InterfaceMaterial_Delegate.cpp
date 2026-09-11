#include "stdafx.h"

#include "InterfaceMaterial_Delegate.h"
#include "InterfaceMaterial.h"
#include "AttriInterfaceMaterial.h"
#include "ContextMenuInvoker.h"
#include "FemAppMainWindow.h"

CInterfaceMaterial_Delegate::CInterfaceMaterial_Delegate(
  CInterfaceMaterial* interfaceMaterial)
: IMaterialBase_Delegate(interfaceMaterial)
, m_interfaceMaterial(interfaceMaterial)
{
}

bool CInterfaceMaterial_Delegate::Attributes()
{
  CAttriInterfaceMaterialDlg dlg(*m_interfaceMaterial, FemAppGetMainWnd());

  return dlg.DoModal() == IDOK;
}

CInterfaceMaterialEntry_Delegate::CInterfaceMaterialEntry_Delegate(
  CInterfaceMaterialEntry* interfaceMaterialEntry)
: CGraphEntryTemp_Delegate <CInterfaceMaterial> (interfaceMaterialEntry)
, m_interfaceMaterialEntry(interfaceMaterialEntry)
{
}

bool CInterfaceMaterialEntry_Delegate::Attributes()
{
  return true;
}

void CInterfaceMaterialEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  invoker.AddCommand("Insert New",
    *new CSingleCommandTemplate <CInterfaceMaterialEntry_Delegate> (*this,
      &CInterfaceMaterialEntry_Delegate::InsertNewWithProperties));
}

void CInterfaceMaterialEntry_Delegate::InsertNewWithProperties()
{
  CInterfaceMaterial* pMat = &m_interfaceMaterialEntry->InsertNew();
  CInterfaceMaterial_Delegate* pMat_Delegate =
    new CInterfaceMaterial_Delegate(pMat);

  if (!pMat_Delegate->Attributes())
  {
    delete pMat_Delegate;
    delete pMat;
  }
}
