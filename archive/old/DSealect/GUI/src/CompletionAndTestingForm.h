#if !defined(AFX_COMPLETIONANDTESTINGFORM_H__64564370_21C4_47A3_81A9_66B67ECCD952__INCLUDED_)
#define AFX_COMPLETIONANDTESTINGFORM_H__64564370_21C4_47A3_81A9_66B67ECCD952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompletionAndTestingForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompletionAndTestingForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "BaseFormView.h"
#include "CompletionAndTestingDraw.h"
#include "AnalysisPointList.h"
#include "CheckFrm.h"


//##ModelId=3C0DC15E00EA
class CCompletionAndTestingForm : public CBaseFormView
{
private:
  class CUserColumnInfo : public CAnalysisPointList::IUserColumnInfo
  {
  public:
    CUserColumnInfo();
    virtual CString Title() const;
    virtual CString Text(CAnalysisPoint& analysisPoint) const;
  };

protected:
	//##ModelId=3C0DC15E00FF
	CCompletionAndTestingForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCompletionAndTestingForm)

	//##ModelId=3CAAA66D00EE
	CCheckFrame m_fraCompletion;

// Form Data
public:
	//##ModelId=3C0DC15E0119
	BOOL CheckComplete() const;

	//{{AFX_DATA(CCompletionAndTestingForm)
	enum { IDD = IDD_CompletionAndTesting };
	//##ModelId=3C60D57A0272
	CButton	m_fraAnalysisPoint;
	//##ModelId=3C3040BA007C
	CAnalysisPointList	m_lstAnalysisPoints;
	//##ModelId=3CAAA66D010D
	CComboBox	m_cmbWellType;
	//##ModelId=3CAAA66D011D
	CStatic		m_lbPressureDifference;
	//##ModelId=3CAAA66D012D
	CDoubleEdit	m_debTimeOfExposToDrilFluid;
	//##ModelId=3CAAA66D014C
	CDoubleEdit	m_debTestPressure;
  CDoubleEdit m_debFluidPressureGradient;
	//##ModelId=3CAAA66D015C
	CDoubleEdit	m_debPressureDifference;
	//##ModelId=3CAAA66D017B
	CDoubleEdit	m_debDrillingFluidTemperature;
	//##ModelId=3C0DC15E0196
	int			m_intBalancedCompletion;
	//##ModelId=3DDA0C5A01D8
	BOOL		m_chkShutIn;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompletionAndTestingForm)
	public:
	//##ModelId=3C0DC15E0197
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15E01A6
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15E01B5
	virtual ~CCompletionAndTestingForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15E01B7
	virtual void AssertValid() const;
	//##ModelId=3C0DC15E01B9
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCompletionAndTestingForm)
	//##ModelId=3C0DC15E0203
	afx_msg void OnoptBalancedCompletion();
	//##ModelId=3C0DC15E0205
	afx_msg void OnoptOverbalancedCompletion();
	//##ModelId=3C0DC15E0213
	afx_msg void OnoptUnderbalancedCompletion();
	//##ModelId=3C0DC15E0222
	afx_msg void OnCloseupcmbWellType();
	//##ModelId=3C3040BA0211
	afx_msg void OnKeydownlstAnalysisPoints(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3DDA0C5A01F7
	afx_msg void OnchkShutIn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPLETIONANDTESTINGFORM_H__64564370_21C4_47A3_81A9_66B67ECCD952__INCLUDED_)
