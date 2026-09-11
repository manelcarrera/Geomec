#if !defined(AFX_CASINGOPERATIONFORM_H__C79CEF2C_98E1_4A78_B306_3FC7051F963B__INCLUDED_)
#define AFX_CASINGOPERATIONFORM_H__C79CEF2C_98E1_4A78_B306_3FC7051F963B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CasingOperationForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCasingOperationForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "CasingMatListCtrl.h"
#include "CasingOperationDraw.h"
#include "BaseFormView.h"

class CCasingOperation;

//##ModelId=3C0DC16001A5
class CCasingOperationForm : public CBaseFormView
{
private:
	//##ModelId=3C0DC16001B6
	CCasingOperation* GetCasingOperation();
	
	//##ModelId=3C6A14CC0202
	CCasingMatListCtrl m_lstCasingMaterialParams;

protected:
	//##ModelId=3C0DC16001D4
	CCasingOperationForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCasingOperationForm)

// Form Data
public:

	//{{AFX_DATA(CCasingOperationForm)
	enum { IDD = IDD_CasingOperation };
	//##ModelId=3CAAA67001F8
	CDoubleEdit	m_debPercentageEccentricity;
	//##ModelId=3CAAA6700207
	CDoubleEdit	m_debGuideShoeDepth;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC16001D5
	virtual BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasingOperationForm)
	public:
	//##ModelId=3C60D57E00EB
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC1600203
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC1600206
	virtual ~CCasingOperationForm();
#ifdef _DEBUG
	//##ModelId=3C0DC1600213
	virtual void AssertValid() const;
	//##ModelId=3C0DC1600215
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCasingOperationForm)
	//##ModelId=3C0DC1600222
	afx_msg void OnKillfocusPercentageEccentricity();
	//##ModelId=3C0DC1600224
	afx_msg void OnKillfocusGuideShoeDepth();
	//##ModelId=3C60D57E00FA
	afx_msg void OnbtnNew();
	//##ModelId=3C60D57E010A
	afx_msg void OnbtnEdit();
	//##ModelId=3C60D57E010C
	afx_msg void OnbtnFromLibrary();
	//##ModelId=3C60D57E010E
	afx_msg void OnbtnToLibrary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CASINGOPERATIONFORM_H__C79CEF2C_98E1_4A78_B306_3FC7051F963B__INCLUDED_)
