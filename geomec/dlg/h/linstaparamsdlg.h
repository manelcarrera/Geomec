#if !defined(AFX_LINSTAPARAMSDLG_H__99A472F4_557F_4973_83BD_F46B49910743__INCLUDED_)
#define AFX_LINSTAPARAMSDLG_H__99A472F4_557F_4973_83BD_F46B49910743__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// linstaparamsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinstaParamsDlg dialog

class CModelBase;

class CLinstaParamsDlg : public CDialog
{
// Construction
public:
	CLinstaParamsDlg(CModelBase& model, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinstaParamsDlg)
	enum { IDD = IDD_LINSTA_PARAMS };
	int		m_iMaxIter;
	double	m_dTolcon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinstaParamsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinstaParamsDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CModelBase& m_model;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINSTAPARAMSDLG_H__99A472F4_557F_4973_83BD_F46B49910743__INCLUDED_)
