#if !defined(AFX_DRILLINGOPERATIONFORM_H__7B9BB6E2_BDF2_4EEF_A89E_90C0F64AF82B__INCLUDED_)
#define AFX_DRILLINGOPERATIONFORM_H__7B9BB6E2_BDF2_4EEF_A89E_90C0F64AF82B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrillingOperationForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrillingOperationForm form view

#include "DoubleEdit.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "DrillingOperationDraw.h"

//##ModelId=3C0DC15C03C8
class CDrillingOperationForm : public CBaseFormView
{
private:
	
protected:
	//##ModelId=3C0DC15D000F
	CDrillingOperationForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDrillingOperationForm)

// Form Data
public:
	//##ModelId=3C0DC15D0010
	virtual BOOL CheckComplete() const;

	//{{AFX_DATA(CDrillingOperationForm)
	enum { IDD = IDD_DrillingOperation };
	//##ModelId=3CAAA66C0090
	CStatic	m_lbUMudStressGradAtSurface;
	//##ModelId=3CAAA66C00A0
	CStatic	m_lbUMudDensityAtSurface;
	//##ModelId=3CAAA66C00B0
	CStatic	m_lbMudStressGradAtSurface;
	//##ModelId=3CAAA66C00CF
	CStatic	m_lbMudDensityAtSurface;
	//##ModelId=3CAAA66C00DF
//	CDoubleEdit	m_debWellInclination;
	//##ModelId=3CAAA66C00FE
	CDoubleEdit	m_debTMDAtOverburdenBottom;
	//##ModelId=3CAAA66C010D
	CDoubleEdit	m_debMudStressGradAtSurface;
	//##ModelId=3CAAA66C012C
	CDoubleEdit	m_debMudDensityAtSurface;
	//##ModelId=3CAAA66C013C
	CDoubleEdit	m_debMudCompressibility;
	//##ModelId=3CAAA66C014C
	CDoubleEdit	m_debBoreholeDiameter;
	//##ModelId=3CAAA66C016B
//	CDoubleEdit	m_debAzimuthWithNorth;
	//##ModelId=3C3040B80378
	int		m_intDensityDefinition;
  int m_iDeviated;
	//}}AFX_DATA

	//##ModelId=3CAAA66C018A
	CDoubleEdit	m_BoreholeDiameter;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrillingOperationForm)
	public:
	//##ModelId=3C0DC15D0138
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15D013A
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15D0157
	virtual ~CDrillingOperationForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15D0159
	virtual void AssertValid() const;
	//##ModelId=3C0DC15D0168
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDrillingOperationForm)
	//##ModelId=3C0DC15D0177
	afx_msg void OnNext();
	//##ModelId=3C0DC15D01C6
	afx_msg void OnoptDensityDefinition();
	//##ModelId=3C0DC15D01D4
	afx_msg void OnoptStressGradientDefinition();
	//}}AFX_MSG
	//##ModelId=3CA011A6034B
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
  afx_msg void OnOptVertical();
  afx_msg void OnOptDeviated();
  afx_msg void OnImport();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRILLINGOPERATIONFORM_H__7B9BB6E2_BDF2_4EEF_A89E_90C0F64AF82B__INCLUDED_)
