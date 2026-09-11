#if !defined(AFX_WELLLOGDLG_H__4037C00F_7390_4B1E_A56F_AE67AAA98749__INCLUDED_)
#define AFX_WELLLOGDLG_H__4037C00F_7390_4B1E_A56F_AE67AAA98749__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellLogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellLogDlg dialog
//class well::CWellLog;
class AFX_EXT_CLASS CWellLogDlg : public CDialog
{
	const well::CWellLog *m_pWellLog;

// Construction
public:
	CWellLogDlg(const well::CWellLog *pLog, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWellLogDlg)
	enum { IDD = UT_IDD_WELLOG_DLG };
	CListCtrl	m_WellLogCtrl;
	CString	m_SndCut;
	CString	m_AvgComp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWellLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWellLogDlg)
		virtual BOOL OnInitDialog();
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLLOGDLG_H__4037C00F_7390_4B1E_A56F_AE67AAA98749__INCLUDED_)
