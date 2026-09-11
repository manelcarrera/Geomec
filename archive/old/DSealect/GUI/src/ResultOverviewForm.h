#if !defined(AFX_RESULTOVERVIEWFORM_H__690C1FE3_223B_418F_921E_A34DB9956D5F__INCLUDED_)
#define AFX_RESULTOVERVIEWFORM_H__690C1FE3_223B_418F_921E_A34DB9956D5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResultOverviewForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"

class CAnalysisPoint;

static const char* WellComponent[]=
{
	"sealant",
	"formation",
	"casing"
};

//##ModelId=3CA0119D030D
class CResultOverviewForm : public CBaseFormView
{
protected:
	//##ModelId=3CA0119D032C
	CResultOverviewForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResultOverviewForm)

// Form Data
public:
	//{{AFX_DATA(CResultOverviewForm)
	enum { IDD = IDD_ResultsOverview };
	//##ModelId=3DDA0C4F0091
	CComboBox	m_cmbDepthAlongWell;
	//##ModelId=3DDA0C4F00A0
	CComboBox	m_cmbWellComponent;
	//##ModelId=3DDA0C4F00A4
	int		m_iPresentation;
	//}}AFX_DATA

// Operations
public:
	
// Overrides
	//##ModelId=3CA0119D032D
	virtual BOOL CheckComplete() const;
	//##ModelId=3CA0119D032F
	virtual BOOL CheckEntry() const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultOverviewForm)
	public:
	//##ModelId=3DDA0C4F00AF
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3CA0119D0331
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//##ModelId=3DDA0C4F00B1
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3CA0119D033D
	virtual ~CResultOverviewForm();
#ifdef _DEBUG
	//##ModelId=3CA0119D033F
	virtual void AssertValid() const;
	//##ModelId=3CA0119D0341
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CResultOverviewForm)
	//##ModelId=3CA0119D034C
	afx_msg void OnoptDamageStatusOverviewTable();
	//##ModelId=3CA0119D035B
	afx_msg void OnoptRiskOfDamageBarChart();
	//##ModelId=3DDA0C4F00CF
	afx_msg void OnCloseupcmbWellComponent();
	//##ModelId=3DDA0C4F00DF
	afx_msg void OnCloseupcmbDepthAlongWell();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//##ModelId=3DDA0C4F00E1
	int m_iCurrentWellCompItem;
	//##ModelId=3DDA0C4F00ED
	void FillWellCompCombo();
	//##ModelId=3DDA0C4F00EE
	void FillDepthCombo();
	//##ModelId=3DDA0C4F00FE
	CAnalysisPoint* m_pCurrentAP;
	//##ModelId=3DDA0C4F010D
	void SendToDrawView();
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTOVERVIEWFORM_H__690C1FE3_223B_418F_921E_A34DB9956D5F__INCLUDED_)
