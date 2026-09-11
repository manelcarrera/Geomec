#if !defined(AFX_MESHMESSAGEDLG_H__20BF3BA6_EAC6_491E_A4A1_EFE7C26AD852__INCLUDED_)
#define AFX_MESHMESSAGEDLG_H__20BF3BA6_EAC6_491E_A4A1_EFE7C26AD852__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeshMessageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshMessageDlg dialog

class CMeshMessageDlg : public CDialog
{
	const std::vector<std::string>& m_message;
// Construction
public:
	CMeshMessageDlg(const std::vector<std::string>& message, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMeshMessageDlg)
	enum { IDD = IDD_MESH_MESSAGES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshMessageDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeshMessageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHMESSAGEDLG_H__20BF3BA6_EAC6_491E_A4A1_EFE7C26AD852__INCLUDED_)
