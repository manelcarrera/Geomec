#if !defined(AFX_CEMENTOPERATIONFORM_H__5B8F11DD_75AF_427E_A394_75B7FA18F73A__INCLUDED_)
#define AFX_CEMENTOPERATIONFORM_H__5B8F11DD_75AF_427E_A394_75B7FA18F73A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CementOperationForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCementOperationForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "CementOperationDraw.h"
#include "CheckFrm.h"

class CCementingOperation;

//##ModelId=3C0DC15F001F
class CCementOperationForm : public CBaseFormView
{
private:
	//##ModelId=3DDA0C5B015B
	BOOL bOptBtnIsAlreadyClicked;
	//##ModelId=3C0DC15F003F
	CCementingOperation* GetCementingOperation();
	//##ModelId=3DDA0C5B016A
	int m_intLeadTailCements;

protected:
	//##ModelId=3C0DC15F004E
	CCementOperationForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCementOperationForm)

	//##ModelId=3CAAA66E0062
	CCheckFrame m_optLeadTailCementsNo;
	//##ModelId=3CAAA66E0081
	CCheckFrame m_optLeadTailCementsYes;

// Form Data
public:
	
	//{{AFX_DATA(CCementOperationForm)
	enum { IDD = IDD_CementingOperation };
	//##ModelId=3DDA0C5B018A
	CStatic	m_lbUTailCementStressGrad;
	//##ModelId=3DDA0C5B019A
	CStatic	m_lbULeadCementStressGrad;
	//##ModelId=3DDA0C5B01AA
	CStatic	m_lbUCementStressGradient;
	//##ModelId=3DDA0C5B01B9
	CStatic	m_lbTailCementStressGrad;
	//##ModelId=3DDA0C5B01D9
	CStatic	m_lbLeadCementStressGrad;
	//##ModelId=3DDA0C5B01E8
	CStatic	m_lbCementStressGradient;
	//##ModelId=3DDA0C5B01F8
	CDoubleEdit	m_debLeadCementDensity;
	//##ModelId=3DDA0C5B0207
	CDoubleEdit	m_debTailCementDensity;
	//##ModelId=3DDA0C5B0227
	CDoubleEdit	m_debCementDensity;
	//##ModelId=3CAAA66E00A0
	CDoubleEdit	m_debTopOfTailCementTMD;
	//##ModelId=3CAAA66E00B0
	CDoubleEdit	m_debTopOfCementTVD;
	//##ModelId=3CAAA66E00CF
	CDoubleEdit	m_debTopOfCementsTMD;
	//##ModelId=3CAAA66E00DF
	CDoubleEdit	m_debTailCementStressGrad;
	//##ModelId=3CAAA66E00FE
	CDoubleEdit	m_debPressureFromFluidsAbove;
	//##ModelId=3CAAA66E010D
	CDoubleEdit	m_debLeadCementStressGrad;
	//##ModelId=3CAAA66E012D
	CDoubleEdit	m_debCementStressGradient;
	//##ModelId=3DDA0C5B0235
	int		m_intFluidDefinition;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15F006D
	BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCementOperationForm)
	public:
	//##ModelId=3C0DC15F0119
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15F011B
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15F0128
	virtual ~CCementOperationForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15F012A
	virtual void AssertValid() const;
	//##ModelId=3C0DC15F0138
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCementOperationForm)
	//##ModelId=3C0DC15F0177
	afx_msg void OnoptLeadTailCementsNo();
	//##ModelId=3C0DC15F0179
	afx_msg void OnoptLeadTailCementsYes();
	//##ModelId=3DDA0C5B0255
	afx_msg void OnoptFluidDensityDefinition();
	//##ModelId=3DDA0C5B0265
	afx_msg void OnoptFluidPressureGradientDefinition();
	//}}AFX_MSG
	//##ModelId=3CA011A803A9
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEMENTOPERATIONFORM_H__5B8F11DD_75AF_427E_A394_75B7FA18F73A__INCLUDED_)
