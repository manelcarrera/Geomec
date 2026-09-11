#include "stdafx.h"

#include "MaterialEntry.h"
#include "MaterialExportDialog.h"

namespace {

const QString EXPORT_LABEL = "Export";
const QString EXPORT_TITLE = "Export To Material Library";

} // anonymous namespace

CMaterialExportDialog::CMaterialExportDialog(CMaterialEntry_Delegate *materialEntry_Delegate,
                                             CMaterialEntry *materialEntry, ml::CMaterialLibrary &matlib,
                                             int modelfilter, CAnalysisLogger &logger, bool bImportMode,
                                             UINT nIDTemplate, CWnd *pParent)
    : CMaterialImportDialog(materialEntry_Delegate, materialEntry, matlib, modelfilter, logger, bImportMode,
                            nIDTemplate, pParent) {}

BOOL CMaterialExportDialog::OnInitDialog() {
  CMatLibDlg::OnInitDialog();

  SetWindowText(EXPORT_TITLE.toStdString().c_str());

  GetDlgItem(IDC_EDIT_MATERIAL_LIBRARY)
      ->SetWindowText(m_materialEntry->GetExternalMatLibFileName().toStdString().c_str());
  GetDlgItem(IDOK)->SetWindowText(EXPORT_LABEL.toStdString().c_str());

  GetDlgItem(IDOK)->EnableWindow(TRUE);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

// private

BEGIN_MESSAGE_MAP(CMaterialExportDialog, CMatLibDlg)
//{{AFX_MSG_MAP(CMaterialExportDialog)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTMATERIALS, OnItemchangedListmaterials)
ON_BN_CLICKED(IDC_BUT_DELETE, OnButDelete)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMaterialExportDialog::OnItemchangedListmaterials(NMHDR *pNMHDR, LRESULT *pResult) {
  CMatLibDlg::OnItemchangedListmaterials(pNMHDR, pResult);
  GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CMaterialExportDialog::OnButDelete() {
  CMatLibDlg::OnButDelete();
  GetDlgItem(IDOK)->EnableWindow(TRUE);
}
