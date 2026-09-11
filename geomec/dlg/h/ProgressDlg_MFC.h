#pragma once

#include "resource.h"

#define WM_PROGRESS (WM_USER + 1)
#define SET_PROGRESS 0

// #define DEBUG_PROGRESS_DLG

class CProgressDlg_MFC : public CDialog {
  void DoMessagePump();

#ifdef DEBUG_PROGRESS_DLG
protected:
  int m_count;
#endif

protected:
  CButton m_btCancel;
  CProgressCtrl *m_pSecundary;
  CProgressCtrl *m_pPrimary;
  CString m_strPrimary;
  CString m_strSecundary;
  int m_nTotalJobs;
  int m_nCurJob;
  bool m_bCancel;
  bool m_bCancelEnable;
  BOOL m_bOriginalState;
  int m_nPreviousValue;

  void Init(int nJobs, bool bCancel);

public:
  // Constructor for single job dlg
  CProgressDlg_MFC(bool bCancel = true, CWnd *pParent = 0);
  // Constructor for multiple job dlg
  CProgressDlg_MFC(int nJobs, const CString &sPrimary, bool bCancel = true, CWnd *pParent = 0);
  CProgressDlg_MFC(int nJobs, unsigned int uPrimary, bool bCancel = true, CWnd *pParent = 0);
  virtual ~CProgressDlg_MFC();

  void SetPrimaryTitle(unsigned int uPrimary);
  void SetPrimaryTitle(const CString &sTitle);
  void SetTitle(unsigned int uTitle);
  void SetTitle(const CString &sTitle);
  void StartDialog();
  void NextJob(const CString &sTitle);
  void NextJob(unsigned int uTitle);

  bool SetProgress(int nTotalSteps, int nStep);
  bool SetProgress(int perc); // set progress in percentage (0 - 100)

  bool OnProgress(int p);

  // Dialog Data
  //{{AFX_DATA(CProgressDlg_MFC)
  enum { IDD = IDD_DOUBLE_PROGRESS };

  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProgressDlg_MFC)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CProgressDlg_MFC)
  virtual void OnCancel();
  afx_msg void OnDestroy();
  //	afx_msg BOOL OnInitDialog();
public:
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
