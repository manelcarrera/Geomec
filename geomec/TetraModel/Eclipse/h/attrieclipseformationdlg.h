#if !defined(AFX_ATTRIECLIPSEFORMATIONDLG_H__67683467_7BA5_4389_A9B2_2E5A798FA087__INCLUDED_)
#define AFX_ATTRIECLIPSEFORMATIONDLG_H__67683467_7BA5_4389_A9B2_2E5A798FA087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrieclipseformationdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseFormationDlg dialog
#include "EclipseFormation.h"
#include "AttributesTemplate.h"

class CAttriEclipseFormationDlg : public CAttributesTemplate<CEclipseFormation>
{
// Construction
public:
	CAttriEclipseFormationDlg(CEclipseFormation& formation, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttriEclipseFormationDlg)
	enum { IDD = IDD_ECL_FORMATION };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriEclipseFormationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttriEclipseFormationDlg)
	afx_msg void OnPaint();
	afx_msg void OnColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIECLIPSEFORMATIONDLG_H__67683467_7BA5_4389_A9B2_2E5A798FA087__INCLUDED_)
