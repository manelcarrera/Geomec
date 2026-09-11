#if !defined(AFX_DEPTHSFORANALYSISFORM_H__B58AD95C_A2F4_4DF2_831C_2F9FB6A195EB__INCLUDED_)
#define AFX_DEPTHSFORANALYSISFORM_H__B58AD95C_A2F4_4DF2_831C_2F9FB6A195EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DepthsForAnalysisForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDepthsForAnalysisForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "DepthsForAnalysisDraw.h"
#include "AnalysisPointList.h"

//##ModelId=3C0DC15D03C8
class CDepthsForAnalysisForm : public CBaseFormView
{
private:
	

protected:
	//##ModelId=3C0DC15E0000
	CDepthsForAnalysisForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDepthsForAnalysisForm)

// Form Data
public:

	//{{AFX_DATA(CDepthsForAnalysisForm)
	enum { IDD = IDD_DepthsForAnalysis };
	//##ModelId=3C204404007E
	CAnalysisPointList	m_lstAnalysisPointList;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15E0001
	virtual BOOL CheckComplete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepthsForAnalysisForm)
	public:
	//##ModelId=3C60D57A008D
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15E000F
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15E0012
	virtual ~CDepthsForAnalysisForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15E001F
	virtual void AssertValid() const;
	//##ModelId=3C0DC15E0021
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDepthsForAnalysisForm)
	//##ModelId=3C20440400AC
	afx_msg void OnbtnAddAnalysisPoint();
	//##ModelId=3C20440400CC
	afx_msg void OnbtnRemoveAnalysisPoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPTHSFORANALYSISFORM_H__B58AD95C_A2F4_4DF2_831C_2F9FB6A195EB__INCLUDED_)
