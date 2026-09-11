#if !defined(AFX_INFOFORM_H__4140B669_5E0E_4A0D_AD51_E6EBBC62F209__INCLUDED_)
#define AFX_INFOFORM_H__4140B669_5E0E_4A0D_AD51_E6EBBC62F209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"

//##ModelId=3C0DC15A0157
class CInfoForm : public CBaseFormView
{
protected:
	//##ModelId=3C0DC15A0159
	CInfoForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInfoForm)

// Form Data
public:
	//##ModelId=3C0DC15A015A
	virtual BOOL CheckEntry() const {return TRUE;}
	//{{AFX_DATA(CInfoForm)
	enum { IDD = IDD_Info };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoForm)
	protected:
	//##ModelId=3C0DC15A0168
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//##ModelId=3C0DC15A016B
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15A016E
	virtual ~CInfoForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15A0177
	virtual void AssertValid() const;
	//##ModelId=3C0DC15A0179
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoForm)
	//##ModelId=3DDA0C520236
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOFORM_H__4140B669_5E0E_4A0D_AD51_E6EBBC62F209__INCLUDED_)
