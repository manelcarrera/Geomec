#if !defined(AFX_POINTSETCREATEDLG_H__674E5E70_3D9E_4FD5_92C4_0F401A823537__INCLUDED_)
#define AFX_POINTSETCREATEDLG_H__674E5E70_3D9E_4FD5_92C4_0F401A823537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pointsetcreatedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPointSetCreateDlg dialog
class CModelBase;
class CUnitNode;
class CPointSet;

#include "PointSetCoordinates.h"

class CPointSetCreateDlg : public CDialog
{
	CModelBase& m_model;
	CPointSetCoordinates m_pointSetCoordinates;
	BOOL m_bValidateDensityOnly;
	const CUnitNode& UnitNode();
	void CreatePointSet();
public:
	CPointSetCreateDlg(CModelBase& model, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPointSetCreateDlg)
	enum { IDD = IDD_CREATE_POINTSET };
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointSetCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPointSetCreateDlg)
	afx_msg void OnDimensionChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTSETCREATEDLG_H__674E5E70_3D9E_4FD5_92C4_0F401A823537__INCLUDED_)
