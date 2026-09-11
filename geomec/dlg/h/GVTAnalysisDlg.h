#include "afxwin.h"
#if !defined(GVTANALYSISDLG_H_)
#define GVTANALYSISDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGVTSettings;

// CGVTAnalysisDlg dialog

class CGVTAnalysisDlg : public CDialog
{
	DECLARE_DYNAMIC(CGVTAnalysisDlg)
  CGVTSettings &m_GVTSettings;

  void SetStartState();
  void DrawCheck(CPaintDC &dc, CStatic &IconCheck, bool bCheck);
public:
	CGVTAnalysisDlg(CGVTSettings &settings, CWnd* pParent = NULL);   // standard constructor
	virtual ~CGVTAnalysisDlg();

// Dialog Data
  //{{AFX_DATA(CGVTAnalysisDlg)
	enum { IDD = IDD_GVT_ANALYSIS };
  CStatic m_GridCheck;
  CStatic m_VelModFileCheck;
  CStatic m_TimeLapseCheck;
  int m_CheckForcePointSets;
  //}}AFX_DATA

  //{{AFX_VIRTUAL(CGVTAnalysisDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL
  

  // Generated message map functions
	//{{AFX_MSG(CGVTAnalysisDlg)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedGvtSettButton();
  afx_msg void OnBnClickedGvtStartButton();
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, unsigned int nCtlColor);

private:
  
public:
  afx_msg void OnBnClickedCheckNewPointsets();
};
#endif