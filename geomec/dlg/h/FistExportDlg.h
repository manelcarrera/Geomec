#if !defined(AFX_FISTEXPORTDLG_H__A1D8C4A3_83D4_4A3F_93F6_77B76BE7FBC1__INCLUDED_)
#define AFX_FISTEXPORTDLG_H__A1D8C4A3_83D4_4A3F_93F6_77B76BE7FBC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FistExportDlg.h : header file
//

#include "wellpathbase.h"

#include <list>

/////////////////////////////////////////////////////////////////////////////
// CFistExportDlg dialog
class CNewWellPath;
class CModelBase;

class CFistExportDlg : public CDialog {
  const CNewWellPath *m_pNewWellPath; // wjrx mantis 3564
  bool m_bNewWellPaths;               // wjrx mantis 3564; used when m_pWellPath and m_bNewWellPath are 0
  const CModelBase *m_pModel;
  std::list<const QDate *> m_NewDateList;        // wjrx mantis 3564
  std::list<const CNewWellPath *> m_NewWellList; // wjrx mantis 3564

  void UpdateDateList();
  void UpdatePointList();

  void AssignDatesToWellPath() const; // wjrx mantis 3564

  // Construction
public:
  CFistExportDlg(CWnd *pParent = NULL); // standard constructor
  CFistExportDlg(const CNewWellPath *pWellPath, const CModelBase *pModel, CWnd *pParent = NULL);
  CFistExportDlg(bool bNewWellPaths, const CModelBase *pModel, CWnd *pParent = NULL);

  // Dialog Data
  //{{AFX_DATA(CFistExportDlg)
  enum { IDD = IDD_FIST_EXPORT_DLG };
  CButton m_BtnRmvDate;
  CButton m_BtnExport;
  CButton m_BtnAddDate;
  CListCtrl m_ListDateCtrl;
  CListCtrl m_ListPtCtrl;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFistExportDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CFistExportDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnBtnAddDate();
  afx_msg void OnBtnIncStages();
  afx_msg void OnBtnRmvDate();
  virtual void OnCancel();
  afx_msg void OnItemchangedDateList(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnExport();
  afx_msg void OnClose();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FISTEXPORTDLG_H__A1D8C4A3_83D4_4A3F_93F6_77B76BE7FBC1__INCLUDED_)
