#if !defined(AFX_MODELVALIDATIONDLG_H__0952370F_DB5C_4818_9D03_F07DD31E3D20__INCLUDED_)
#define AFX_MODELVALIDATIONDLG_H__0952370F_DB5C_4818_9D03_F07DD31E3D20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelValidationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelValidationDlg dialog

class CModelBase;

#include "ValidateModel.h"

class CModelValidationDlg : public CDialog
{
	CModelBase *m_pModel;
	CValidateModel m_ValidateModel;

	bool m_bMesh;
	bool m_bReservoir;
	bool m_bDepletion;
	bool m_bMaterial;
	bool m_bBoundCond;
	bool m_bEmptForm;
  bool m_bEmptFault;

	void DrawCheck(CPaintDC &dc, CStatic &IconCheck, bool bCheck);
	

public:
	CModelValidationDlg(CModelBase *pModel, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelValidationDlg)
	enum { IDD = IDD_MODEL_VALID_DLG };
  CStatic m_EmptyFaultsCheck;
	CStatic	m_EmptyFormationsCheck;
	CStatic	m_BoundConditionsCheck;
	CStatic	m_MaterialCheckIcon;
	CStatic	m_DepletionCheckIcon;
	CStatic	m_ResCheckIcon;
	CStatic	m_MeshCheckIcon;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelValidationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelValidationDlg)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELVALIDATIONDLG_H__0952370F_DB5C_4818_9D03_F07DD31E3D20__INCLUDED_)
