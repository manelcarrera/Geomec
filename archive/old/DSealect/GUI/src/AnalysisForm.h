#if !defined(AFX_ANALYSISFORM_H__EF624DC9_992F_4EE6_BF8A_1DD29843EAC3__INCLUDED_)
#define AFX_ANALYSISFORM_H__EF624DC9_992F_4EE6_BF8A_1DD29843EAC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnalysisForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalysisForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "AnalysisDraw.h"

//##ModelId=3C0DC161006D
class CAnalysisForm : public CBaseFormView
{

friend CAnalysisDraw;

protected:
	//##ModelId=3C0DC161008E
	CAnalysisForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAnalysisForm)

// Form Data
public:
	//##ModelId=3C0DC161008F
	virtual BOOL CheckComplete() const;
	//##ModelId=3C0DC1610091
	virtual BOOL CheckEntry() const;

	//{{AFX_DATA(CAnalysisForm)
	enum { IDD = IDD_Analysis };
	//##ModelId=3C3040BE0399
	CListCtrl	m_lstAnalysis;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C3040BE03B7
	void BuildAnalysisList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalysisForm)
	public:
	//##ModelId=3C3040BE03D6
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC161009F
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC16100AC
	virtual ~CAnalysisForm();
#ifdef _DEBUG
	//##ModelId=3C0DC16100AE
	virtual void AssertValid() const;
	//##ModelId=3C0DC16100B0
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CAnalysisForm)
	//##ModelId=3C8D9E2802EF
	afx_msg void OnbtnSaveProject();
	//##ModelId=3C8D9E280310
	afx_msg void OnbtnRunAnalysis();
	//##ModelId=3C8D9E28031F
	afx_msg void OnItemchangedlstAnalysis(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3DDA0C600003
	afx_msg void OnClicklstAnalysis(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYSISFORM_H__EF624DC9_992F_4EE6_BF8A_1DD29843EAC3__INCLUDED_)
