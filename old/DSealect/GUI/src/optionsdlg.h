#if !defined(AFX_OPTIONSDLG_H__D8AFE5FA_675A_41B7_83EB_26F804A129D8__INCLUDED_)
#define AFX_OPTIONSDLG_H__D8AFE5FA_675A_41B7_83EB_26F804A129D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// optionsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

//##ModelId=3DDA0C5002B3
class COptionsDlg : public CDialog
{
// Construction
public:
	//##ModelId=3DDA0C5002C3
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_Options };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	//##ModelId=3DDA0C5002C5
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	//##ModelId=3DDA0C5002D2
	virtual BOOL OnInitDialog();
	//##ModelId=3DDA0C5002D4
	afx_msg void OnbtnBrowseCasLib();
	//##ModelId=3DDA0C5002D6
	afx_msg void OnbtnBrowseCemLib();
	//##ModelId=3DDA0C5002D8
	afx_msg void OnbtnBrowseFormLib();
	//##ModelId=3DDA0C5002DA
	afx_msg void OnbtnBrowseTempDir();
	//##ModelId=3DDA0C5002E1
	virtual void OnOK();
	//##ModelId=3DDA0C5002E3
	afx_msg void OnbtnBrowseWorkDirOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__D8AFE5FA_675A_41B7_83EB_26F804A129D8__INCLUDED_)
