#if !defined(AFX_SELECTWORKSHEETDLG_H__0B7468D3_574A_4C82_82B4_E5B0F8D09712__INCLUDED_)
#define AFX_SELECTWORKSHEETDLG_H__0B7468D3_574A_4C82_82B4_E5B0F8D09712__INCLUDED_

#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectWorksheetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectWorksheetDlg dialog

class CSelectWorksheetDlg : public CDialog
{
// Construction
public:
	CSelectWorksheetDlg(CWnd* pParent = NULL);   // standard constructor
	~CSelectWorksheetDlg();
  void SetStrings( const std::vector<CString>& vcStrings );
	void GetSelectedIndices( int **piIndices, int *nIndices );
	void SetTitle( CString strTitle ) {m_strTitle = strTitle;}

// Dialog Data
	//{{AFX_DATA(CSelectWorksheetDlg)
	enum { IDD = IDD_SELWORKSHEET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectWorksheetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  std::vector<CString> m_vcStrings;
	int *m_piIndices;
	int m_nIndices;
	CString m_strTitle;

	// Generated message map functions
	//{{AFX_MSG(CSelectWorksheetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTWORKSHEETDLG_H__0B7468D3_574A_4C82_82B4_E5B0F8D09712__INCLUDED_)
