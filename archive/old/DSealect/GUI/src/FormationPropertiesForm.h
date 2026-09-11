#if !defined(AFX_FORMATIONPROPERTIESFORM_H__54F82247_F345_4A70_94B1_B70D3F64B762__INCLUDED_)
#define AFX_FORMATIONPROPERTIESFORM_H__54F82247_F345_4A70_94B1_B70D3F64B762__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationPropertiesForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormationPropertiesForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "FormationMatListCtrl.h"
#include "BaseFormView.h"
#include "FormationPropertiesDraw.h"

class CFormation;

//##ModelId=3C0DC15A01A5
class CFormationPropertiesForm : public CBaseFormView
{

protected:
	//##ModelId=3C0DC15A01B8
	CFormationPropertiesForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFormationPropertiesForm)

// Form Data
public:

	//{{AFX_DATA(CFormationPropertiesForm)
	enum { IDD = IDD_FormationProperties };
	//##ModelId=3C2043FF00FB
	CFormationMatListCtrl	m_lstFormationMateriaParams;
	//##ModelId=3C2043FF012A
	CListCtrl	m_lstFormationMaterial;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15A01B9
	virtual BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormationPropertiesForm)
	public:
	//##ModelId=3C0DC15A01C7
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15A01C9
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15A01D5
	virtual ~CFormationPropertiesForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15A01E4
	virtual void AssertValid() const;
	//##ModelId=3C0DC15A01E6
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFormationPropertiesForm)
	//##ModelId=3C2043FF0149
	afx_msg void OnbtnFromLibrary();
	//##ModelId=3C2043FF0177
	afx_msg void OnClicklstFormationMaterial(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C2043FF01D5
	afx_msg void OnKeydownlstFormationMaterial(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C60D575005E
	afx_msg void OnEdit();
	//##ModelId=3C60D575006F
	afx_msg void OnNew();
	//##ModelId=3C60D5750071
	afx_msg void OnToLibrary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//##ModelId=3C2043FF0281
	void BuildFormationMaterialList();
	//##ModelId=3C2043FF02E0
	CFormation* m_pCurrentFormation;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONPROPERTIESFORM_H__54F82247_F345_4A70_94B1_B70D3F64B762__INCLUDED_)
