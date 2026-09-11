#include "stdafx.h"

#include "BranchState.h"
#include "ContextMenuInvoker.h"
#include "IMaterial.h"
#include "IMaterial_Delegate.h"
#include "MatParamDlg.h"
#include "MaterialEntry.h"
#include "ModelBase.h"

IMaterial_Delegate::IMaterial_Delegate(IMaterial *material) : IMaterialBase_Delegate(material), m_material(material) {}

bool IMaterial_Delegate::Attributes() {
  CModelBase &model = static_cast<CModelBase &>(m_material->Model());
  CMaterialEntry *pEntry = dynamic_cast<CMaterialEntry *>(m_material->parent());
  assert(pEntry);

  CMatParamDlg dlg(m_material->LibraryMaterial(), pEntry->MaterialLibrary(), pEntry->MaterialModelFilter(),
                   model.Logger());
  dlg.DoModal();

  return true;
}

void IMaterial_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<IMaterial> TMaterialCommand;

  invoker.AddCommand(_T("&To Material Library"),
                     *(new TMaterialCommand(*m_material, &IMaterial::ExportToMatLib, &IMaterial::CanExportToMatLib)));
  // Append attributes ...

  invoker.AddCommand(_T("Delete"), *new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Destroy,
                                                                   &CGraphNode_Delegate::CanDestroy));
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Attributes)));
}

bool IMaterial_Delegate::CanDestroy() const { return m_material->CanDestroy(); }

bool IMaterial_Delegate::Destroy() { return m_material->Destroy(); }
