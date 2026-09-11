#include "stdafx.h"

#include "MaterialImportDialog.h"
#include "TNOFileDialog.h"
#include "MaterialEntry.h"
#include "MaterialEntry_Delegate.h"

CMaterialImportDialog::CMaterialImportDialog(
  CMaterialEntry_Delegate* materialEntry_Delegate,
  CMaterialEntry* materialEntry, ml::CMaterialLibrary& matlib, int modelfilter,
  CAnalysisLogger& logger, bool bImportMode, UINT nIDTemplate, CWnd* pParent)
: CMatLibDlg(matlib, modelfilter, logger, bImportMode, nIDTemplate, pParent)
, m_materialEntry(materialEntry)
, m_materialEntry_Delegate(materialEntry_Delegate)
{
}

BOOL CMaterialImportDialog::OnInitDialog()
{
  CMatLibDlg::OnInitDialog();

  GetDlgItem(IDC_EDIT_MATERIAL_LIBRARY)->SetWindowText(
    m_materialEntry->GetExternalMatLibFileName().toStdString().c_str());

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

// private

BEGIN_MESSAGE_MAP(CMaterialImportDialog, CMatLibDlg)
  //{{AFX_MSG_MAP(CMaterialImportDialog)
  ON_BN_CLICKED(IDC_BROWSE, OnBrowseMaterialLibrary)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMaterialImportDialog::OnBrowseMaterialLibrary()
{
  m_materialEntry_Delegate->SelectMatLib();
  OnInitDialog();
}
