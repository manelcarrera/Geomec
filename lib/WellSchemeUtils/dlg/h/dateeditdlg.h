#if !defined(AFX_DATEEDITDLG_H__A074AA29_6A4F_427F_91DD_AA1353CA035F__INCLUDED_)
#define AFX_DATEEDITDLG_H__A074AA29_6A4F_427F_91DD_AA1353CA035F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dateeditdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDateEditDlg dialog

class CDateEditDlg : public CDialog
{
// Construction
public:
	CDateEditDlg(const COleDateTime &time, const CPoint &point, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonthCalDlg)
	enum { IDD = IDD_DATEEDITDLG };
	//}}AFX_DATA

	const COleDateTime &DateTime() const;

private:
	COleDateTime m_DateTime;
	CPoint m_pt;
	BOOL m_bExiting;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDateEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
	afx_msg void OnSelectMonthcalendar(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg BOOL OnNcActivate(BOOL bActive);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATEEDITDLG_H__A074AA29_6A4F_427F_91DD_AA1353CA035F__INCLUDED_)
