#if !defined(AFX_ANALYSISLOGDLG_H__D28276D0_E9AC_402C_963D_0B84338C68E8__INCLUDED_)
#define AFX_ANALYSISLOGDLG_H__D28276D0_E9AC_402C_963D_0B84338C68E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// analysislogdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalysisLogDlg dialog

class CAnalysisLogger;

class CAnalysisLogDlg : public CDialog {
  void UpdateControls();

  // Construction
public:
  CAnalysisLogDlg(CAnalysisLogger &logger, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAnalysisLogDlg)
  enum { IDD = IDD_ANALYSISLOG };

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAnalysisLogDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAnalysisLogDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnExport();
  afx_msg void OnClear();
  afx_msg void OnVerbose();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  void FillControl(bool bFilterBadShapeWarnings = false, bool bVerbose = false);

  CListBox m_List;
  int m_nVerbose;
  //}}AFX_DATA
  CAnalysisLogger &m_logger;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYSISLOGDLG_H__D28276D0_E9AC_402C_963D_0B84338C68E8__INCLUDED_)
