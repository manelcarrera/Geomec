#if !defined(AFX_ADDFORMATIONDLG_H__6103512E_BB8F_4265_A07C_371693843698__INCLUDED_)
#define AFX_ADDFORMATIONDLG_H__6103512E_BB8F_4265_A07C_371693843698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFormationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFormationDlg dialog

class CDoubleEdit;
class CFormationTopography;

//##ModelId=3C0DC1610138
class CAddFormationDlg : public CDialog
{
private:
	//##ModelId=3C0DC1610168
	BOOL m_bFormationAdded;
	//##ModelId=3CAAA6730081
	CFormationTopography* m_pFT;
	
// Construction
public:
	//##ModelId=3C0DC1610186
	BOOL IsFormationAdded();
	//CAddFormationDlg(CWnd* pParent = NULL);   // standard constructor
	//##ModelId=3C0DC1610187
	CAddFormationDlg(CFormationTopography* pFT,CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(CAddFormationDlg)
	enum { IDD = IDD_frmAddFormation };
	//##ModelId=3CAAA67300A0
	CDoubleEdit	m_debBottomTVD;
	//##ModelId=3C0DC16101A5
	CString	m_strFormationName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFormationDlg)
	protected:
	//##ModelId=3C0DC16101A6
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFormationDlg)
	//##ModelId=3C0DC16101B5
	afx_msg void OnKillfocusBottomTVD();
	//##ModelId=3C0DC16101B7
	virtual void OnOK();
	//##ModelId=3C2044080158
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFORMATIONDLG_H__6103512E_BB8F_4265_A07C_371693843698__INCLUDED_)
