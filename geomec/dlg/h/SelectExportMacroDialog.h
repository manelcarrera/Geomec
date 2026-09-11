#pragma once

class CExportResultData;

#include "resource.h"
#include "afxwin.h"

// CSelectExportMacroDialog dialog

class CSelectExportMacroDialog : public CDialog
{
  DECLARE_DYNAMIC(CSelectExportMacroDialog)

public:
  CSelectExportMacroDialog(CExportResultData** exportResultData,
  const CGraphEntry::TEntryNodeSet& entryNodeSet,
  CWnd* pParent = NULL);   // standard constructor
  virtual ~CSelectExportMacroDialog();

// Dialog Data
  enum { IDD = IDD_SELECT_EXPORT_MACRO_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
private:
  CExportResultData** m_exportResultData;
  const CGraphEntry::TEntryNodeSet m_entryNodeSet;
  CListBox m_selectExportMacrosList;
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnLbnDblclkSelectExportMacroList();
  afx_msg void OnLbnSelchangeSelectExportMacroList();
  afx_msg void OnLbnSelcancelSelectExportMacroList();
private:
  void retrieveExportResultData();
};
