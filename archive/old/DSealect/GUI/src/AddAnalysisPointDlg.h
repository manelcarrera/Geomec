#if !defined(AFX_ADDANALYSISPOINTDLG_H__3E635BD5_FEB2_4C8F_9094_2D27502F960D__INCLUDED_)
#define AFX_ADDANALYSISPOINTDLG_H__3E635BD5_FEB2_4C8F_9094_2D27502F960D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddAnalysisPointDlg.h : header file
//
#include "OXToolTipCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CAddAnalysisPointDlg dialog

class CAnalysis;

//##ModelId=3C2044080281
class CAddAnalysisPointDlg : public CDialog
{
private:
	//##ModelId=3C3040BF02CC
	BOOL m_bAnalysisPointAdded;
	//##ModelId=3CAAA673010D
	CAnalysis* m_pAnalysis;
	
	//##ModelId=3DDA0C60014C
	COXToolTipCtrl m_toolTip;

// Construction
public:
	//##ModelId=3C20440802CF
	CAddAnalysisPointDlg(CAnalysis* pAnalysis, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddAnalysisPointDlg)
	enum { IDD = IDD_AddAnalysisPoint };
	//##ModelId=3CAAA673013C
	CDoubleEdit	m_debTVD;
	//##ModelId=3CAAA673014C
	CDoubleEdit	m_debTMD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddAnalysisPointDlg)
	public:
	//##ModelId=3DDA0C600150
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3C20440802FE
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddAnalysisPointDlg)
	//##ModelId=3C2044080301
	virtual void OnOK();
	//##ModelId=3C204408031D
	virtual void OnCancel();
	//##ModelId=3C204408031F
	virtual BOOL OnInitDialog();
	//##ModelId=3C2044080321
	afx_msg void OnChangedebAddAnalysisPointTMD();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDANALYSISPOINTDLG_H__3E635BD5_FEB2_4C8F_9094_2D27502F960D__INCLUDED_)
