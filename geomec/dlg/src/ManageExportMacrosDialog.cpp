// ManageExportMacrosDialog.cpp : implementation file
//

#include "ManageExportMacrosDialog.h"
#include "ExportResultData.h"
#include "Geomec.h"
#include "GeomecUtils.h"
#include "stdafx.h"

namespace {

typedef struct {
  QString m_folder;
  QString m_file;
  bool m_export;
  CExportResultData *m_exportResultData;
} TManageExportMacrosListEntry;

std::vector<TManageExportMacrosListEntry> exportMacrosListEntries;

} // anonymous namespace

// CManageExportMacrosDialog dialog

IMPLEMENT_DYNAMIC(CManageExportMacrosDialog, CDialog)

CManageExportMacrosDialog::CManageExportMacrosDialog(const CGraphEntry::TEntryNodeSet &entryNodeSet,
                                                     CWnd *pParent /*=NULL*/)
    : CDialog(CManageExportMacrosDialog::IDD, pParent), m_entryNodeSet(entryNodeSet) {}

CManageExportMacrosDialog::~CManageExportMacrosDialog() {}

void CManageExportMacrosDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_MANAGE_EXPORT_MACROS_LIST, m_manageExportMacrosList);
  DDX_Control(pDX, IDC_EDIT_BROWSE_FOLDER, m_editBrowseFolder);
  DDX_Control(pDX, IDC_EDIT_BROWSE_FILE, m_editBrowseFile);
  DDX_Control(pDX, IDC_CHECK_EXPORT, m_checkExport);
}

BEGIN_MESSAGE_MAP(CManageExportMacrosDialog, CDialog)
ON_LBN_SELCHANGE(IDC_MANAGE_EXPORT_MACROS_LIST, &CManageExportMacrosDialog::OnManageExportMacrosList)
ON_BN_CLICKED(IDC_BROWSE_FOLDER, &CManageExportMacrosDialog::OnBrowseFolder)
ON_BN_CLICKED(IDC_CHECK_EXPORT, &CManageExportMacrosDialog::OnCheckExport)
ON_EN_CHANGE(IDC_EDIT_BROWSE_FOLDER, &CManageExportMacrosDialog::OnEditBrowseFolder)
ON_EN_UPDATE(IDC_EDIT_BROWSE_FOLDER, &CManageExportMacrosDialog::OnEditBrowseFolder)
ON_EN_CHANGE(IDC_EDIT_BROWSE_FILE, &CManageExportMacrosDialog::OnEditBrowseFile)
ON_EN_UPDATE(IDC_EDIT_BROWSE_FILE, &CManageExportMacrosDialog::OnEditBrowseFile)
END_MESSAGE_MAP()

// CManageExportMacrosDialog message handlers

void CManageExportMacrosDialog::OnManageExportMacrosList() {
  int index = m_manageExportMacrosList.GetCurSel();

  if (index != LB_ERR) {
    size_t s = m_manageExportMacrosList.GetItemData(index);

    enableEditEntry(TRUE);

    m_editBrowseFolder.SetWindowText(exportMacrosListEntries[s].m_folder.toStdString().c_str());
    m_editBrowseFile.SetWindowText(exportMacrosListEntries[s].m_file.toStdString().c_str());
    m_checkExport.SetCheck(exportMacrosListEntries[s].m_export ? BST_CHECKED : BST_UNCHECKED);
  }
}

void CManageExportMacrosDialog::OnBrowseFolder() {
  CString oldPath;

  GetDlgItem(IDC_EDIT_BROWSE_FOLDER)->GetWindowText(oldPath);

  QString newPath = BrowseFolder(GetSafeHwnd(), "Browse", (LPCTSTR)oldPath);

  if (!newPath.isEmpty() && DirExists(newPath)) {
    GetDlgItem(IDC_EDIT_BROWSE_FOLDER)->SetWindowText(newPath.toStdString().c_str());
  }
}

void CManageExportMacrosDialog::OnCheckExport() {
  int index = m_manageExportMacrosList.GetCurSel();

  if (index != LB_ERR) {
    int state = ((CButton *)GetDlgItem(IDC_CHECK_EXPORT))->GetCheck();
    size_t s = m_manageExportMacrosList.GetItemData(index);

    exportMacrosListEntries[s].m_export = (state == BST_CHECKED);
  }
}

void CManageExportMacrosDialog::OnEditBrowseFolder() {
  int index = m_manageExportMacrosList.GetCurSel();

  if (index != LB_ERR) {
    CString folder;
    size_t s = m_manageExportMacrosList.GetItemData(index);

    GetDlgItem(IDC_EDIT_BROWSE_FOLDER)->GetWindowText(folder);
    exportMacrosListEntries[s].m_folder = (LPCTSTR)folder;
  }
}

void CManageExportMacrosDialog::OnEditBrowseFile() {
  int index = m_manageExportMacrosList.GetCurSel();

  if (index != LB_ERR) {
    CString file;
    size_t s = m_manageExportMacrosList.GetItemData(index);

    GetDlgItem(IDC_EDIT_BROWSE_FILE)->GetWindowText(file);
    exportMacrosListEntries[s].m_file = (LPCTSTR)file;
  }
}

BOOL CManageExportMacrosDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  for (CGraphEntry::TEntryNodeSet::iterator iterator = m_entryNodeSet.begin(); iterator != m_entryNodeSet.end();
       ++iterator) {
    TManageExportMacrosListEntry manageExportMacrosListEntry;
    CExportResultData *exportResultData = (CExportResultData *)*iterator;

    exportResultData->m_settings.bind(*exportResultData);

    manageExportMacrosListEntry.m_folder = exportResultData->m_folder;
    manageExportMacrosListEntry.m_file = exportResultData->m_file;
    manageExportMacrosListEntry.m_export = exportResultData->m_export;
    manageExportMacrosListEntry.m_exportResultData = exportResultData;

    int index = m_manageExportMacrosList.AddString(exportResultData->Name().toStdString().c_str());

    exportMacrosListEntries.push_back(manageExportMacrosListEntry);

    m_manageExportMacrosList.SetItemData(index, exportMacrosListEntries.size() - 1);
  }

  enableEditEntry(FALSE);

  return TRUE; // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

// protected

void CManageExportMacrosDialog::OnOK() {
  for (std::vector<TManageExportMacrosListEntry>::const_iterator manageExportMacrosListEntry =
           exportMacrosListEntries.begin();
       manageExportMacrosListEntry != exportMacrosListEntries.end(); ++manageExportMacrosListEntry) {
    (*manageExportMacrosListEntry).m_exportResultData->m_folder = (*manageExportMacrosListEntry).m_folder;
    (*manageExportMacrosListEntry).m_exportResultData->m_file = (*manageExportMacrosListEntry).m_file;
    (*manageExportMacrosListEntry).m_exportResultData->m_export = (*manageExportMacrosListEntry).m_export;

    (*manageExportMacrosListEntry)
        .m_exportResultData->m_settings.unbind(*(*manageExportMacrosListEntry).m_exportResultData, true);
  }

  CDialog::OnOK();
}

// private:

void CManageExportMacrosDialog::enableEditEntry(BOOL enable) {
  GetDlgItem(IDC_STATIC_BROWSE_FOLDER)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_BROWSE_FOLDER)->EnableWindow(enable);
  GetDlgItem(IDC_BROWSE_FOLDER)->EnableWindow(enable);
  GetDlgItem(IDC_STATIC_BROWSE_FILE)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_BROWSE_FILE)->EnableWindow(enable);
  GetDlgItem(IDC_CHECK_EXPORT)->EnableWindow(enable);
}
