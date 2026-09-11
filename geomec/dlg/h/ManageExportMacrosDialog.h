#pragma once

#include "afxwin.h"
#include "resource.h"

// CManageExportMacrosDialog dialog

class CManageExportMacrosDialog : public CDialog {
  DECLARE_DYNAMIC(CManageExportMacrosDialog)

public:
  CManageExportMacrosDialog(const CGraphEntry::TEntryNodeSet &entryNodeSet,
                            CWnd *pParent = NULL); // standard constructor
  virtual ~CManageExportMacrosDialog();

  // Dialog Data
  enum { IDD = IDD_MANAGE_EXPORT_MACROS };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

  DECLARE_MESSAGE_MAP()
private:
  CGraphEntry::TEntryNodeSet m_entryNodeSet;
  CListBox m_manageExportMacrosList;
  CEdit m_editBrowseFolder;
  CEdit m_editBrowseFile;
  CButton m_checkExport;

public:
  afx_msg void OnManageExportMacrosList();
  afx_msg void OnBrowseFolder();
  afx_msg void OnCheckExport();
  afx_msg void OnEditBrowseFolder();
  afx_msg void OnEditBrowseFile();
  virtual BOOL OnInitDialog();

private:
  void enableEditEntry(BOOL enable);

protected:
  virtual void OnOK();
};
