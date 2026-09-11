#if !defined(AFX_OUTPUTSELECTIONFORM_H__C5AD4A24_ADDA_483E_B662_245D97F37F95__INCLUDED_)
#define AFX_OUTPUTSELECTIONFORM_H__C5AD4A24_ADDA_483E_B662_245D97F37F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputSelectionForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputSelectionForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "OutputSelectionDraw.h"

//##ModelId=3C0DC15801C5
class COutputSelectionForm : public CBaseFormView
{
private:
protected:
	//##ModelId=3C0DC15801E9
	COutputSelectionForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COutputSelectionForm)

// Form Data
public:
	//##ModelId=3C0DC15801EA
	virtual BOOL CheckComplete() const;


	//{{AFX_DATA(COutputSelectionForm)
	enum { IDD = IDD_OutputSelection };
	//##ModelId=3C60D57201D5
	int		m_iOverviewLevel;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputSelectionForm)
	public:
	//##ModelId=3C60D57201E5
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15801F7
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC1580203
	virtual ~COutputSelectionForm();
#ifdef _DEBUG
	//##ModelId=3C0DC1580205
	virtual void AssertValid() const;
	//##ModelId=3C0DC1580207
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(COutputSelectionForm)
	//##ModelId=3C60D57201E7
	afx_msg void OnoptOverview1();
	//##ModelId=3C60D57201F5
	afx_msg void OnoptOverview2();
	//##ModelId=3C60D57201F7
	afx_msg void OnoptOverview3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTSELECTIONFORM_H__C5AD4A24_ADDA_483E_B662_245D97F37F95__INCLUDED_)
