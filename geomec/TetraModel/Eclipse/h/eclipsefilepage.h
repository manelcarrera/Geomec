#if !defined(AFX_ECLIPSEFILEPAGE_H__2D270BA6_8529_415D_84CA_381FF9316CF9__INCLUDED_)
#define AFX_ECLIPSEFILEPAGE_H__2D270BA6_8529_415D_84CA_381FF9316CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WizzardPageBase.h"
#include "EclipseModel.h"

class CEclipseFilePage : public CWizzardPageBase
{
	DECLARE_DYNCREATE(CEclipseFilePage)

	CString	m_sPath;
// Construction
public:
	CEclipseFilePage();
	CEclipseFilePage(CEclipseModel& model);
	~CEclipseFilePage();

// Dialog Data
	//{{AFX_DATA(CEclipseFilePage)
	enum { IDD = IDD_ECLIPSE_FILE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEclipseFilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEclipseFilePage)
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECLIPSEFILEPAGE_H__2D270BA6_8529_415D_84CA_381FF9316CF9__INCLUDED_)
