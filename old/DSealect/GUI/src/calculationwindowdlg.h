#if !defined(AFX_CALCULATIONWINDOWDLG_H__A6019E88_FB89_4283_A349_86B7BF64A718__INCLUDED_)
#define AFX_CALCULATIONWINDOWDLG_H__A6019E88_FB89_4283_A349_86B7BF64A718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// calculationwindowdlg.h : header file
//

#include "CheckFrm.h"
#include "OXToolTipCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CCalculationWindowDlg dialog

class CBatchTable;
class CBatchTableRow;

//##ModelId=3DDA0C5E0274
class CCalculationWindowDlg : public CDialog
{

private:
	//##ModelId=3DDA0C5E0284
	void BuildAnalysisList();
	//BOOL PreTranslateMessage(MSG* pMsg);
	//##ModelId=3DDA0C5E0286
	CBatchTable* m_pBatchTable;
	//##ModelId=3DDA0C5E0296
	CBatchTableRow* m_pCurrentBatchTableRow;

	//##ModelId=3DDA0C5E02A4
	CListCtrl m_cListCtrl;
	//##ModelId=3DDA0C5E02B3
	CImageList m_cImageList;
	//##ModelId=3DDA0C5E02B8
	COXToolTipCtrl m_toolTip;

	//##ModelId=3DDA0C5E02C4
	BOOL bInitState;

protected:
	//##ModelId=3DDA0C5E02D2
	CCheckFrame m_optDefaultSteppingScheme;
// Construction
public:
	//##ModelId=3DDA0C5E02D6
	CCalculationWindowDlg(CBatchTable* pBatchTable,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalculationWindowDlg)
	enum { IDD = IDD_CalculationWindow };
	//##ModelId=3DDA0C5E02E2
	CButton	m_btnStart;
	//##ModelId=3DDA0C5E02F2
	CListCtrl	m_lstAnalysis;
	//##ModelId=3DDA0C5E0301
	CButton	m_chkStopBatchRunIfDiv;
	//##ModelId=3DDA0C5E0311
	CButton	m_chkSaveDiagnostic;
	//##ModelId=3DDA0C5E0316
	CButton	m_chkAllowFurtherPostProc;
	//##ModelId=3DDA0C5E0323
	CComboBox	m_cmbIterationScheme;
	//##ModelId=3DDA0C5E0330
	CComboBox	m_cmbConvergenceNorm;
	//##ModelId=3DDA0C5E033F
	int		m_iMaxNumberOfIterations;
	//##ModelId=3DDA0C5E034E
	int		m_iNumberOfTimeSteps;
	//##ModelId=3DDA0C5E034F
	int		m_intDefaultSteppingScheme;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalculationWindowDlg)
	public:
	//##ModelId=3DDA0C5E035E
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3DDA0C5E036E
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalculationWindowDlg)
	//##ModelId=3DDA0C5E0371
	afx_msg void OnbttnCancel();
	//##ModelId=3DDA0C5E037E
	afx_msg void OnbttnStart();
	//##ModelId=3DDA0C5E0380
	afx_msg void OnchkAllowFurtherPostProcessing();
	//##ModelId=3DDA0C5E0382
	afx_msg void OnchkSaveDiagnostic();
	//##ModelId=3DDA0C5E0384
	afx_msg void OnchkStopBatchRunIfDivergence();
	//##ModelId=3DDA0C5E038D
	afx_msg void OnCloseupcmbConvergenceNorm();
	//##ModelId=3DDA0C5E038F
	afx_msg void OnCloseupcmbIterationScheme();
	//##ModelId=3DDA0C5E0391
	afx_msg void OnoptAdvancedOptionsForSteppingScheme();
	//##ModelId=3DDA0C5E0393
	afx_msg void OnoptDefaultSteppingScheme();
	//##ModelId=3DDA0C5E0395
	afx_msg void OnClicklstAnalysis(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3DDA0C5E039F
	afx_msg void OnKillfocusebNrOfTimeSteps();
	//##ModelId=3DDA0C5E03AD
	afx_msg void OnKillfocusebMaxNumberOfIterations();
	//##ModelId=3DDA0C5E03AF
	virtual BOOL OnInitDialog();
	//##ModelId=3DDA0C5E03B1
	afx_msg void OnItemchangedlstAnalysis(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALCULATIONWINDOWDLG_H__A6019E88_FB89_4283_A349_86B7BF64A718__INCLUDED_)
