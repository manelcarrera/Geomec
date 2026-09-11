#if !defined(AFX_EXPORTDLG_H__1247D49C_68D1_4F49_B194_42808274E8A5__INCLUDED_)
#define AFX_EXPORTDLG_H__1247D49C_68D1_4F49_B194_42808274E8A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExportFormat.h"

class CExportDlg : public CDialog
{
	IExportFormat::TAxisSystem m_axis_system;
public:
	CExportDlg(IExportFormat::TAxisSystem axis_system, CWnd* pParent = NULL);   // standard constructor
	IExportFormat::TAxisSystem AxisSystem() const;

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_EXPORT_POINTSET };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__1247D49C_68D1_4F49_B194_42808274E8A5__INCLUDED_)
