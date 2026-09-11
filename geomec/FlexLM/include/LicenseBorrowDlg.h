/*****************************************************************
 * FILENAME:
 *   $Workfile: LicenseBorrowDlg.h $
 *
 * PURPOSE:
 *   This file handles the dialog for borrowing licenses
 *
 * COPYRIGHT 2004 Shell Internationale Research Mij. B.V.
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
 *   $Author: Baronm $
 *   $Date: 11/21/05 10:21a $
 *
 * PART OF MODULE:
 *   
 *
 * MODULE CONTROLLER:
 *    LogicaCMG
 *****************************************************************/

#if !defined(AFX_LICENSEBORROWDLG_H__0D4CCE59_27E9_465B_996A_F9CBA2FDAC54__INCLUDED_)
#define AFX_LICENSEBORROWDLG_H__0D4CCE59_27E9_465B_996A_F9CBA2FDAC54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LicenseBorrowDlg.h : header file
//
#include "..\LicenseResource\resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLicenseBorrowDlg dialog

class CLicenseBorrowDlg : public CDialog
{
// Construction
public:
	CLicenseBorrowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLicenseBorrowDlg)
	enum { IDD = IDD_LICENSE_BORROW_DLG };
	CDateTimeCtrl	m_cBorrowEnd;
    CEdit	m_cReturnLicenseInformationEdit;
    CEdit	m_cBorrowInformationEdit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseBorrowDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void Updatelist();
    void SetDateRange();

	// Generated message map functions
	//{{AFX_MSG(CLicenseBorrowDlg)
	afx_msg void OnBorrowLicense();
	virtual BOOL OnInitDialog();
	afx_msg void OnReturnBorrowedLicense();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEBORROWDLG_H__0D4CCE59_27E9_465B_996A_F9CBA2FDAC54__INCLUDED_)
