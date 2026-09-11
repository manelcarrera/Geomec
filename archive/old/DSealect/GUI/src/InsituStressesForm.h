#if !defined(AFX_INSITUSTRESSESFORM_H__31C48C52_0E7B_423E_A718_26383D9583E0__INCLUDED_)
#define AFX_INSITUSTRESSESFORM_H__31C48C52_0E7B_423E_A718_26383D9583E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsituStressesForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsituStressesForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "InsituStressesDraw.h"
#include "CheckFrm.h"

class CFormation;

//##ModelId=3C0DC15902CE
class CInsituStressesForm : public CBaseFormView
{
private:
	//##ModelId=3C2043FE00CD
	CFormation* m_pCurrentFormation;
	//##ModelId=3C2043FE011A
	void BuildFormationList();
	
protected:
	//##ModelId=3C0DC15902EE
	CInsituStressesForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInsituStressesForm)

	//##ModelId=3CAAA6660042
	CCheckFrame m_lstItemOverburden;
	//##ModelId=3CAAA6660052
	CCheckFrame	m_lstItemFormation;

// Form Data
public:
	//##ModelId=3C0DC159031C
	virtual BOOL CheckComplete() const;

	//{{AFX_DATA(CInsituStressesForm)
	enum { IDD = IDD_InsituStresses };
	//##ModelId=3CAAA6660071
	CListCtrl	m_lstFormationName;
	//##ModelId=3CAAA6660081
	CDoubleEdit	m_debPorePressFormationBottom;
	//##ModelId=3CAAA666009F
	CDoubleEdit	m_debPorePressAtFormationTop;
	//##ModelId=3CAAA66600B0
	CDoubleEdit	m_debAzimuthOfSHWithNorth;
	//##ModelId=3CAAA66600BF
	CDoubleEdit	m_debMinHorizontalStressRatio;
	//##ModelId=3CAAA66600DF
	CDoubleEdit	m_debMaxHorizontalStressRatio;
	//##ModelId=3CAAA666010D
	CDoubleEdit	m_debGeothermalGradientF;
	//##ModelId=3CAAA666012D
	CDoubleEdit	m_debAverageDensity;
	//##ModelId=3CAAA666013C
	CDoubleEdit	m_debGeothermalGradient;
	//##ModelId=3CAAA666015C
	CDoubleEdit	m_debSurfaceUndisturbedTemp;
	//int		m_optFullyUndrained;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C2043FE0168
	CFormation* CurrentFormation();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsituStressesForm)
	public:
	//##ModelId=3C0DC15A004E
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15A0050
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15A005D
	virtual ~CInsituStressesForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15A006D
	virtual void AssertValid() const;
	//##ModelId=3C0DC15A006F
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInsituStressesForm)
	//##ModelId=3C0DC15A00BE
	afx_msg void OnClicklstFormationName(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C2043FE01A6
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//##ModelId=3C2043FE02CF
	afx_msg void OnKeydownlstFormationName(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnoptFullyDrained();
	//afx_msg void OnoptFullyUndrained();
  afx_msg void OnImportPressure();
  afx_msg void OnRemovePressure();
  afx_msg void OnImportDensity();
  afx_msg void OnRemoveDensity();
  afx_msg void OnImportTemperature();
  afx_msg void OnRemoveTemperature();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSITUSTRESSESFORM_H__31C48C52_0E7B_423E_A718_26383D9583E0__INCLUDED_)
