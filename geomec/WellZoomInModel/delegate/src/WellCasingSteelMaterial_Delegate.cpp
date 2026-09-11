#include "stdafx.h"

#include "WellCasingSteelMaterial_Delegate.h"
#include "WellCasingSteelMaterial.h"
#include "AttriWellCasingMaterial.h"
#include "ContextMenuInvoker.h"
#include "FemAppMainWindow.h"

CWellCasingSteelMaterial_Delegate::CWellCasingSteelMaterial_Delegate(
  CWellCasingSteelMaterial* wellCasingSteelMaterial)
: IMaterialBase_Delegate(wellCasingSteelMaterial)
, m_wellCasingSteelMaterial(wellCasingSteelMaterial)
{
}

bool CWellCasingSteelMaterial_Delegate::Attributes()
{
  CAttriWellCasingMaterialDlg dlg(*m_wellCasingSteelMaterial, FemAppGetMainWnd());

  return (dlg.DoModal() == IDOK);
}

CWellCasingSteelMaterialEntry_Delegate::CWellCasingSteelMaterialEntry_Delegate(
  CWellCasingSteelMaterialEntry* wellCasingSteelMaterialEntry)
: CGraphEntryTemp_Delegate <CWellCasingSteelMaterial> (
  wellCasingSteelMaterialEntry)
, m_wellCasingSteelMaterialEntry(wellCasingSteelMaterialEntry)
{
}

bool CWellCasingSteelMaterialEntry_Delegate::Attributes()
{
  return true;
}

void CWellCasingSteelMaterialEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  invoker.AddCommand("Insert New",
  *new CSingleCommandTemplate <CWellCasingSteelMaterialEntry_Delegate> (
      *this, &CWellCasingSteelMaterialEntry_Delegate::InsertNewAndProperties));
}

void CWellCasingSteelMaterialEntry_Delegate::InsertNewAndProperties()
{
  CWellCasingSteelMaterial* pMat =
  &m_wellCasingSteelMaterialEntry->InsertNew();
  CWellCasingSteelMaterial_Delegate* pMat_Delegate =
  new CWellCasingSteelMaterial_Delegate(pMat);

  if (!pMat_Delegate->Attributes())
  {
  delete pMat_Delegate;
  delete pMat;
  }
}
