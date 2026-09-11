#include "stdafx.h"

#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "ContextMenuInvoker.h"
#include "DerivedMatLibDlg.h"
#include "GeomecUtils.h"
#include "GlobalMessage.h"
#include "ISettings.h"
#include "LibraryMaterial.h"
#include "MaterialEntry.h"
#include "MaterialEntry_Delegate.h"
#include "MaterialExportDialog.h"
#include "MaterialImportDialog.h"
#include "ModelBase.h"
#include "TNOFileDialog.h"

CMaterialEntry_Delegate::CMaterialEntry_Delegate(CMaterialEntry *materialEntry)
    : CGraphEntryTemp_Delegate<IMaterial>(materialEntry), m_materialEntry(materialEntry) {}

bool CMaterialEntry_Delegate::Attributes() {
  CModelBase &model = static_cast<CModelBase &>(m_materialEntry->Model());

  CDerivedMatLibDlg dlg(this, m_materialEntry->MaterialLibrary(), m_materialEntry->MaterialModelFilter(),
                        model.Logger());

  dlg.DoModal();

  return true;
}

void CMaterialEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CMaterialEntry_Delegate> TCommand;

  invoker.AddCommand(_T("&Import from Material Library"),
                     *(new TCommand(*this, &CMaterialEntry_Delegate::ImportFromMatLib)));
  invoker.AddCommand(_T("&Create Material Library"), *(new TCommand(*this, &CMaterialEntry_Delegate::CreateMatLib)));
  invoker.AddSeparator();

  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Attributes)));
}

void CMaterialEntry_Delegate::ImportFromMatLib() {
  if (m_materialEntry->GetExternalMatLibFileName().isEmpty()) {
    SelectMatLib();

    if (m_materialEntry->GetExternalMatLibFileName().isEmpty()) {
      return;
    }
  }

  CModelBase &model = static_cast<CModelBase &>(m_materialEntry->Model());

  CMaterialImportDialog dlg(this, m_materialEntry, m_materialEntry->ExternalLibrary(),
                            m_materialEntry->MaterialModelFilter(), model.Logger(), true);

  if (dlg.DoModal() == IDOK) {
    std::vector<ml::CMaterial *> vcSelected = dlg.SelectedMaterials();
    bool bInvalidFound = false;
    for (size_t i = 0; i < vcSelected.size(); ++i) {
      CLibraryMaterial *pLibMat = dynamic_cast<CLibraryMaterial *>(vcSelected[i]);
      if (pLibMat)
        m_materialEntry->ImportFromMatLib(*pLibMat);
      else
        bInvalidFound = true;
    }

    if (bInvalidFound)
      _m()->msg("At least one selected material cannot be used by GEOMEC, it was not imported");
  }

  // material library contents might have changed (user can create, edit and delete in the dialog)
  m_materialEntry->UpdateExternalMatLib();
}

void CMaterialEntry_Delegate::SelectMatLib() {
  // let user browse for database

  CTnoFileDialog dlg(TRUE, "mlx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "XML Material Libraries (*.mlx)|*.mlx||");

  if (dlg.DoModal() != IDOK) {
    return;
  }

  ISettings::instance()->writeProfileString("Settings", "XMLMatLib", (LPCSTR)dlg.GetPathName());

  m_materialEntry->SetExternalMatLibFileName((LPCSTR)dlg.GetPathName(), true);
}

void CMaterialEntry_Delegate::ExportToMatLib(std::vector<ml::CMaterial *> selection) {
  if (m_materialEntry->GetExternalMatLibFileName().isEmpty()) {
    SelectMatLib();

    if (m_materialEntry->GetExternalMatLibFileName().isEmpty()) {
      return;
    }
  }

  CModelBase &model = static_cast<CModelBase &>(m_materialEntry->Model());

  CMaterialExportDialog dlg(this, m_materialEntry, m_materialEntry->ExternalLibrary(),
                            m_materialEntry->MaterialModelFilter(), model.Logger(), true);

  if (dlg.DoModal() == IDOK) {
    bool bInvalidFound = false;
    for (size_t i = 0; i < selection.size(); ++i) {
      CLibraryMaterial *pLibMat = dynamic_cast<CLibraryMaterial *>(selection[i]);
      if (pLibMat)
        m_materialEntry->ExportToMatLib(*pLibMat);
      else
        bInvalidFound = true;
    }

    if (bInvalidFound)
      _m()->msg("At least one selected material cannot be used by GEOMEC, it was not exported");
  }

  // material library contents might have changed (user can create, edit and delete in the dialog)
  m_materialEntry->UpdateExternalMatLib();
}

void CMaterialEntry_Delegate::CreateMatLib() {
  CTnoFileDialog dlg(FALSE, "mlx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "XML Material Libraries (*.mlx)|*.mlx||");
  if (dlg.DoModal() != IDOK)
    return;

  ISettings::instance()->writeProfileString("Settings", "XMLMatLib", (LPCSTR)dlg.GetPathName());

  m_materialEntry->SetExternalMatLibFileName((LPCSTR)dlg.GetPathName(), false);
}
