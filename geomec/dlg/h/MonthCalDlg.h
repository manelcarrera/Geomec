#if !defined(AFX_MONTHCALDLG_H__81E57FEC_CB97_4D46_BA16_A2D3DBD2B69F__INCLUDED_)
#define AFX_MONTHCALDLG_H__81E57FEC_CB97_4D46_BA16_A2D3DBD2B69F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonthCalDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonthCalDlg dialog

class CMonthCalDlg : public CDialog
{
// Construction
public:
	CMonthCalDlg(const COleDateTime &time, const CPoint &point, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonthCalDlg)
	enum { IDD = IDD_MONTHCALDLG };
	//}}AFX_DATA

	const COleDateTime &DateTime() const;

private:
	COleDateTime m_DateTime;
	CPoint m_pt;
	BOOL m_bExiting;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonthCalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMonthCalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
	afx_msg void OnSelectMonthcalendar(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg BOOL OnNcActivate(BOOL bActive);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONTHCALDLG_H__81E57FEC_CB97_4D46_BA16_A2D3DBD2B69F__INCLUDED_)
