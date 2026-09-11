#if !defined(AFX_CASINGSTRESSFORM_H__88DDAC0A_CB63_4F9C_A62F_F4D851C27D38__INCLUDED_)
#define AFX_CASINGSTRESSFORM_H__88DDAC0A_CB63_4F9C_A62F_F4D851C27D38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CasingStressForm.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CCasingStressForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "CheckFrm.h"

class CCasingOperation;

//##ModelId=3C0DC15F02DE
class CCasingStressForm : public CBaseFormView
{
private:
	//##ModelId=3C3040BD0146
	CCasingOperation* GetCasingOperation() const;
	
protected:
	//##ModelId=3C0DC15F02FD
	CCasingStressForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCasingStressForm)

	//##ModelId=3CAAA66F038E
	CCheckFrame m_optAdvanced;
	//##ModelId=3CAAA66F039E
	CCheckFrame m_optNoWorkString;
	//##ModelId=3CAAA66F03AD
	CCheckFrame m_optWorkStringUsed;
	//##ModelId=3CAAA66F03BD
	CCheckFrame m_optNewFluid;


// Form Data
public:

	//{{AFX_DATA(CCasingStressForm)
	enum { IDD = IDD_CasingStresses };
	//##ModelId=3DDA0C5D00A0
	CDoubleEdit	m_debNewFluidDensity;
	//##ModelId=3DDA0C5D00B0
	CStatic	m_lbNewFluidStressGrad;
	//##ModelId=3DDA0C5D00BF
	CStatic	m_lbDispFluidStressGradientNWS;
	//##ModelId=3DDA0C5D00CF
	CStatic	m_lbDispFluidStressGrad;
	//##ModelId=3DDA0C5D00EE
	CDoubleEdit	m_debDispFluidDensity;
	//##ModelId=3DDA0C5D00FE
	CDoubleEdit	m_debDispFluidDensityNWS;
	//##ModelId=3CAAA66F03DC
	CDoubleEdit	m_debWorkStringPackersTMD;
	//##ModelId=3CAAA6700004
	CDoubleEdit	m_debNewFluidStressGrad;
	//##ModelId=3CAAA6700013
	CDoubleEdit	m_debNewFluidOverpressure;
	//##ModelId=3CAAA6700033
	CDoubleEdit	m_debNewFluidCompressibility;
	//##ModelId=3CAAA6700042
	CDoubleEdit	m_debHookLoad;
	//##ModelId=3CAAA6700062
	CDoubleEdit	m_debDispFluidStressGradientNWS;
	//##ModelId=3CAAA6700081
	CDoubleEdit	m_debDispFluidStressGrad;
	//##ModelId=3CAAA67000B0
	CDoubleEdit	m_debDispFluidCompressibilityNWS;
	//##ModelId=3CAAA67000CF
	CDoubleEdit	m_debDispFluidCompressibility;
	//##ModelId=3C0DC160004E
	int		m_intAutomatic;
	//##ModelId=3C0DC160005D
	int		m_intMud;
	//##ModelId=3C0DC160006D
	int		m_intNoWorkString;
	//##ModelId=3DDA0C5D011C
	int		m_intFluidDefinition;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15F034B
	virtual BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasingStressForm)
	public:
	//##ModelId=3C0DC160007D
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC160008C
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC16000AB
	virtual ~CCasingStressForm();
#ifdef _DEBUG
	//##ModelId=3C0DC16000AD
	virtual void AssertValid() const;
	//##ModelId=3C0DC16000AF
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCasingStressForm)
	//##ModelId=3C0DC16000FE
	afx_msg void OnoptAdvanced();
	//##ModelId=3C0DC1600100
	afx_msg void OnoptAutomatic();
	//##ModelId=3C0DC160010A
	afx_msg void OnoptMud();
	//##ModelId=3C0DC160010C
	afx_msg void OnoptNewFluid();
	//##ModelId=3C0DC160010E
	afx_msg void OnoptNoWorkString();
	//##ModelId=3C0DC160011A
	afx_msg void OnoptWorkStringUsed();
	//##ModelId=3DDA0C5D013B
	afx_msg void OnoptFluidDensityDefinition();
	//##ModelId=3DDA0C5D013D
	afx_msg void OnoptFluidPressureGradientDefinition();
	//}}AFX_MSG
	//##ModelId=3CA011AB01F3
	virtual afx_msg LRESULT OnValidateDoubleEdit(WPARAM id, LPARAM lparam); 
	DECLARE_MESSAGE_MAP()
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CASINGSTRESSFORM_H__88DDAC0A_CB63_4F9C_A62F_F4D851C27D38__INCLUDED_)
