// DetailedResultsForm.h: interface for the CDetailedResultsForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILEDRESULTSFORM_H__5DE7BE6A_0B64_419A_B5EB_643C922D824E__INCLUDED_)
#define AFX_DETAILEDRESULTSFORM_H__5DE7BE6A_0B64_419A_B5EB_643C922D824E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "DetailedResultsDraw.h"
#include "CheckFrm.h"


//##ModelId=3DDA0C580283
class CDetailedResultsForm : public CBaseFormView
{
protected:
	//##ModelId=3DDA0C580294
	CDetailedResultsForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDetailedResultsForm)

	//##ModelId=3DDA0C5802A4
	CCheckFrame m_fraCompareResults;
	//##ModelId=3DDA0C5802A9
	CCheckFrame m_fraViewResults;
// Form Data
public:
	//{{AFX_DATA(CDetailedResultsForm)
	enum { IDD = IDD_DetailedResults };
	//##ModelId=3DDA0C5802B3
	CComboBox	m_cmbResultType;
	//##ModelId=3DDA0C5802C3
	CComboBox	m_cmbTypeOfDamage;
	//##ModelId=3DDA0C5802D3
	CComboBox	m_cmbPlotSpace;
	//##ModelId=3DDA0C5802D8
	CComboBox	m_cmbCementScenario;
	//##ModelId=3DDA0C5802E4
	CComboBox	m_cmbDepthAlongWell;
	//##ModelId=3DDA0C5802F2
	int		m_iPresent;
	//}}AFX_DATA

// Operations
public:
	
// Overrides
	//##ModelId=3DDA0C5802F3
	virtual BOOL CheckComplete() const;
	//##ModelId=3DDA0C580301
	virtual BOOL CheckEntry() const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetailedResultsForm)
	public:
	//##ModelId=3DDA0C580303
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3DDA0C580305
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//##ModelId=3DDA0C580310
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3DDA0C580320
	virtual ~CDetailedResultsForm();
#ifdef _DEBUG
	//##ModelId=3DDA0C580322
	virtual void AssertValid() const;
	//##ModelId=3DDA0C580324
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDetailedResultsForm)
	//##ModelId=3DDA0C58032F
	afx_msg void OnCloseupcmbResultType();
	//##ModelId=3DDA0C580331
	afx_msg void OnCloseupcmbDepthAlongWell();
	//##ModelId=3DDA0C580333
	afx_msg void OnCloseupcmbCementScenario();
	//##ModelId=3DDA0C580335
	afx_msg void OnCloseupcmbPlotSpace();
	//##ModelId=3DDA0C580337
	afx_msg void OnCloseupcmbTypeOfDamage();
	//##ModelId=3DDA0C58033F
	afx_msg void OnoptPresentResultAgainstLoadDamageTypes();
	//##ModelId=3DDA0C580341
	afx_msg void OnoptPresentResultAgainstSpaceTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//##ModelId=3DDA0C580343
	int GetCurrentDamageType(int iCurrentComboSelection);
	//##ModelId=3DDA0C580345
	int GetCurrentComboSelection(int iCurrentDamageType);
	//##ModelId=3DDA0C58034F
	int m_iCurrentResultTypeItem;
	//##ModelId=3DDA0C580350
	int m_iCurrentScenario;
	//##ModelId=3DDA0C58035E
	int m_iCurrentTypeOfDamage;
	//##ModelId=3DDA0C58035F
	int m_iCurrentPlotSpace;

	//##ModelId=3DDA0C58036E
	void FillDepthCombo();
	//##ModelId=3DDA0C58036F
	void FillCemScenarioCombo();
	//##ModelId=3DDA0C580370
	void FillResultTypeCombo();
	//##ModelId=3DDA0C580371
	void FillTypeOfDamageCombo();
	//##ModelId=3DDA0C580372
	void FillPlotSpaceCombo();
	//##ModelId=3DDA0C58037D
	void FillCorrespondingCemMat();
	//##ModelId=3DDA0C58037E
	void FillLoadType();

	//##ModelId=3DDA0C580380
	CAnalysisPoint* m_pCurrentAP;
	//##ModelId=3DDA0C58038E
	CBatchTableRow* m_pCurrentBatchTableRow;
	//##ModelId=3DDA0C580392
	void SendToDrawView();
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILEDRESULTSFORM_H__5DE7BE6A_0B64_419A_B5EB_643C922D824E__INCLUDED_)

  
