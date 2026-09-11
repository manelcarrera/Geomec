/*****************************************************************
 * FILENAME:
 *   $Workfile: RegisterDialog.h $
 *
 * PURPOSE:
 *   Contains the dialog used to show calculation progress
 *
 * COPYRIGHT 1999 Shell Internationale Research Mij. B.V.
 *
 * CONFIDENTIALITY:
 *   This is a CONFIDENTIAL document.
 *   Neither the  whole  or any  part of this document may
 *   be  reproduced,  stored in  any retrieval  system  or
 *   transmitted in any form or by any means  (electronic,
 *   mechanical,  reprographic,  recording  or  otherwise)
 *   without the  prior  consent of the  copyright  owner.
 *
 * VERSION INFORMATION:
 *   $Revision: 3 $
 *   $Author: Bilbyr $
 *   $Date: 7/01/03 13:58 $
 *
 * PART OF MODULE:
 *    License
 *
 * MODULE CONTROLLER:
 *    Logica
 *****************************************************************/
#if !defined(AFX_REGISTERDIALOG_H__B2B6A1CB_6D75_4F08_93E2_1FA08A91A749__INCLUDED_)
#define AFX_REGISTERDIALOG_H__B2B6A1CB_6D75_4F08_93E2_1FA08A91A749__INCLUDED_

#include "License.h"
#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RegisterDialog dialog

class AFX_EXT_CLASS_LICENSE RegisterDialog : public CDialog
{
// Construction
public:
	RegisterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(RegisterDialog)
	enum { IDD = IDD_REGISTRATIONBOX };
	CButton	m_cDemoButton;
	CButton	m_cRegisterText;
	CStatic	m_csOptionText;
	//}}AFX_DATA
	CString m_csAppName;
	BOOL GetShowDemo() { return m_bShowDemo; };
	void SetShowDemo(BOOL p_bShowDemo) { m_bShowDemo = p_bShowDemo; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RegisterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(RegisterDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void OnRegister();
	void OnDemoMode(); 
	void OnBrowseForLicense();
private:
	void OnInitialUpdate(); 
	BOOL m_bShowDemo;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDIALOG_H__B2B6A1CB_6D75_4F08_93E2_1FA08A91A749__INCLUDED_)
