#include "stdafx.h"

#include "DerivedMatLibDlg.h"
#include "MaterialEntry_Delegate.h"

CDerivedMatLibDlg::CDerivedMatLibDlg(
  CMaterialEntry_Delegate* materialEntry_Delegate,
  ml::CMaterialLibrary& matlib, int modelfilter, CAnalysisLogger& logger,
  bool bImportMode, UINT nIDTemplate, CWnd* pParent)
: CMatLibDlg(matlib, modelfilter, logger, bImportMode, nIDTemplate, pParent)
, m_materialEntry_Delegate(materialEntry_Delegate)
{
}

BOOL CDerivedMatLibDlg::OnInitDialog()
{
  CMatLibDlg::OnInitDialog();

  GetDlgItem(IDC_EXPORT_MATERIAL)->EnableWindow(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
}

// private

BEGIN_MESSAGE_MAP(CDerivedMatLibDlg, CMatLibDlg)
  //{{AFX_MSG_MAP(CDerivedMatLibDlg)
  ON_BN_CLICKED(IDC_IMPORT_MATERIAL, OnImportMaterial)
  ON_BN_CLICKED(IDC_EXPORT_MATERIAL, OnExportMaterial)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTMATERIALS, OnItemchangedListmaterials)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDerivedMatLibDlg::OnImportMaterial()
{
  m_materialEntry_Delegate->ImportFromMatLib();
  OnInitDialog();
}

void CDerivedMatLibDlg::OnExportMaterial()
{
  m_materialEntry_Delegate->ExportToMatLib(CurrentSelection());
  OnInitDialog();
}

void CDerivedMatLibDlg::OnItemchangedListmaterials(NMHDR* pNMHDR,
  LRESULT* pResult)
{
  CMatLibDlg::OnItemchangedListmaterials(pNMHDR, pResult);
  GetDlgItem(IDC_EXPORT_MATERIAL)->EnableWindow(CurrentSelection().size() != 0);
}
