#if !defined(AFX_SURFACEMERGEDEFDLG_H__48A8479F_3744_4512_BFFF_3A2CD98073C1__INCLUDED_)
#define AFX_SURFACEMERGEDEFDLG_H__48A8479F_3744_4512_BFFF_3A2CD98073C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// surfacemergedefdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSurfaceMergeDefDlg dialog

class CSurfaceMergeDefDlg : public CDialog
{
public:
	enum MERGE_TYPE { UNDEFINED = 0, FACES, POINTS};

private:
	MERGE_TYPE &m_type;
public:
// Construction
	CSurfaceMergeDefDlg(MERGE_TYPE &type, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSurfaceMergeDefDlg)
	enum { IDD = IDD_MERGE_SURFACES_DLG };
	int		m_checked;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSurfaceMergeDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSurfaceMergeDefDlg)
	afx_msg void OnConnectRadio();
	afx_msg void OnRemeshRadio();


	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SURFACEMERGEDEFDLG_H__48A8479F_3744_4512_BFFF_3A2CD98073C1__INCLUDED_)
