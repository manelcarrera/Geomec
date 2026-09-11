#if !defined(AFX_EXTREMEOPERATIONSFORM_H__AC375DB1_9323_4D84_BB69_CFF5A285EE55__INCLUDED_)
#define AFX_EXTREMEOPERATIONSFORM_H__AC375DB1_9323_4D84_BB69_CFF5A285EE55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtremeOperationsForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExtremeOperationsForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "ExtremeOperationsDraw.h"
#include "AnalysisPointList.h"
#include "CheckFrm.h"


//##ModelId=3C0DC15B0128
class CExtremeOperationsForm : public CBaseFormView
{
private:
	//##ModelId=3DDA0C550070
	BOOL	m_bFracturing;
	//##ModelId=3DDA0C550090
	BOOL	m_bEvacuation;
	
protected:
	//##ModelId=3C0DC15B0148
	CExtremeOperationsForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExtremeOperationsForm)

	//##ModelId=3C3040B600BA
	CCheckFrame m_fraFracturing;
	//##ModelId=3C3040B600F8
	CCheckFrame m_fraEvacuation;

// Form Data
public:

	//{{AFX_DATA(CExtremeOperationsForm)
	enum { IDD = IDD_ExtremeOperations };
	//##ModelId=3C60D5780001
	CButton	m_fraAnalysisPoint;
	//##ModelId=3C60D5780021
	CEdit	m_ebChosenWellType;
	//##ModelId=3C60D5780040
	CAnalysisPointList	m_lstAnalysisPoints;
	//##ModelId=3CAAA6690217
	CDoubleEdit	m_debTreatingFluidTemperatureEU;
	//##ModelId=3CAAA6690236
	CDoubleEdit	m_debTreatingFluidTemperature;
	//##ModelId=3CAAA6690256
	CDoubleEdit	m_debTimeOfExposToTreatFluidEU;
	//##ModelId=3CAAA6690265
	CDoubleEdit	m_debTimeOfExposToTreatFluid;
	//##ModelId=3CAAA6690284
	CDoubleEdit	m_debMaximumInjectionPressure;
	//##ModelId=3CAAA66902A4
	CDoubleEdit	m_debBottomHoleTreatingPres;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15B0149
	BOOL CheckComplete() const;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtremeOperationsForm)
	public:
	//##ModelId=3C3040B601F2
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15B01C5
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15B01D4
	virtual ~CExtremeOperationsForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15B01D6
	virtual void AssertValid() const;
	//##ModelId=3C0DC15B01D8
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CExtremeOperationsForm)
	//##ModelId=3C3040B7009A
	afx_msg void OnchkEvacuationUnloading();
	//##ModelId=3C3040B700F8
	afx_msg void OnchkFracturingHighRateKilling();
	//##ModelId=3C3040B60369
	afx_msg void OnKeydownlstAnalysisPoint(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTREMEOPERATIONSFORM_H__AC375DB1_9323_4D84_BB69_CFF5A285EE55__INCLUDED_)
