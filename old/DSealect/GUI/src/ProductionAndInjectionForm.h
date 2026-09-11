#if !defined(AFX_PRODUCTIONANDINJECTIONFORM_H__8A3F897A_0C1D_41D5_BBFC_DFB6E9678EE6__INCLUDED_)
#define AFX_PRODUCTIONANDINJECTIONFORM_H__8A3F897A_0C1D_41D5_BBFC_DFB6E9678EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProductionAndInjectionForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProductionAndInjectionForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "ProductionAndInjectionDraw.h"
#include "AnalysisPointList.h"
#include "CheckFrm.h"

//##ModelId=3C0DC157030D
class CProductionAndInjectionForm : public CBaseFormView
{
private:
	//##ModelId=3DDA0C5001C8
	int	m_iConversion;
	
protected:

	//##ModelId=3C0DC157032C
	CProductionAndInjectionForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CProductionAndInjectionForm)

	//##ModelId=3CAAA6630300
	CCheckFrame m_fraProduction;
	//##ModelId=3CAAA6630311
	CCheckFrame m_fraInjection;

// Form Data
public:


	//{{AFX_DATA(CProductionAndInjectionForm)
	enum { IDD = IDD_ProductionAndInjection };
	//##ModelId=3C60D57200FB
	CButton	m_fraAnalysisPoint;
	//##ModelId=3C3040B100E9
	CAnalysisPointList	m_lstAnalysisPoints;
	//##ModelId=3C3040B10137
	CEdit	m_ebChosenWellType;
	
	
	//##ModelId=3CAAA6630350
	CDoubleEdit	m_debTimeOfContinuousProd;
	//##ModelId=3CAAA663035F
	CDoubleEdit	m_debTimeOfContinuousInjection;
	//##ModelId=3CAAA663037E
	CDoubleEdit	m_debPorePressure;
	//##ModelId=3CAAA663039E
	CDoubleEdit	m_debMaximumInjectionPressure;
	//##ModelId=3CAAA66303AD
	CDoubleEdit	m_debInjectionTemperature;
	//##ModelId=3CAAA66303CD
	CDoubleEdit	m_debBottomHoleFlowingTemp;
	//##ModelId=3CAAA66303DC
	CDoubleEdit	m_debBottomHoleFlowingPress;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC1570350
	BOOL CheckComplete() const;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductionAndInjectionForm)
	public:
	//##ModelId=3C0DC15800FA
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15800FC
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC158010A
	virtual ~CProductionAndInjectionForm();
#ifdef _DEBUG
	//##ModelId=3C0DC1580119
	virtual void AssertValid() const;
	//##ModelId=3C0DC158011B
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CProductionAndInjectionForm)
	//##ModelId=3C0DC1580167
	afx_msg void OnchkConversion();
	//##ModelId=3C3040B102FB
	afx_msg void OnKeydownlstAnalysisPoint(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTIONANDINJECTIONFORM_H__8A3F897A_0C1D_41D5_BBFC_DFB6E9678EE6__INCLUDED_)
