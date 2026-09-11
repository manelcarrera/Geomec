#if !defined(AFX_CEMENTPROPERTIESFORM_H__E33D1799_DE69_4BC8_A81B_DCD2F4B3753C__INCLUDED_)
#define AFX_CEMENTPROPERTIESFORM_H__E33D1799_DE69_4BC8_A81B_DCD2F4B3753C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CementPropertiesForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCementPropertiesForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "CementMatListCtrl.h"
#include "CementPropertiesDraw.h"
#include "BaseFormView.h"

class CCementScenario;

//##ModelId=3C0DC15E02DE
class CCementPropertiesForm : public CBaseFormView
{
private:
	//##ModelId=3DDA0C5A0320
	BOOL bIsInKillFocusLoop;
	
	//##ModelId=3C6A14C90194
	CCementScenario* GetSelectedScenario();
	//##ModelId=3C6A14C901A4
	CCementScenario *m_pCurrentScenario;
	//##ModelId=3C6A14C901B3
	//BOOL PreTranslateMessage(MSG* pMsg);
	

protected:
	//##ModelId=3C0DC15E02FD
	CCementPropertiesForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCementPropertiesForm)

// Form Data
public:

	//{{AFX_DATA(CCementPropertiesForm)
	enum { IDD = IDD_CementProperties };
	//##ModelId=3C3040BB003C
	int	m_iNumberOfCementScenarios;
	//##ModelId=3C3040BB00AB
	CCementMatListCtrl	m_lstCementPropertiesParams;
	//##ModelId=3C3040BB00CA
	CListCtrl	m_lstCementProperties;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	//##ModelId=3C0DC15E02FE
	BOOL CheckComplete() const;
	//##ModelId=3C6A14C901E4
	BOOL TailSelected();
	//##ModelId=3C6A14C901F2
	CString GetSelectedMatName();
	//##ModelId=3C3040BB00E8
	void BuildCementPropertieList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCementPropertiesForm)
	public:
	//##ModelId=3C3040BB0117
	virtual void OnInitialUpdate();
	//##ModelId=3C6A14C901B3
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//##ModelId=3C0DC15E030F
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3C0DC15E0312
	virtual ~CCementPropertiesForm();
#ifdef _DEBUG
	//##ModelId=3C0DC15E031D
	virtual void AssertValid() const;
	//##ModelId=3C0DC15E031F
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCementPropertiesForm)
	//##ModelId=3C3040BB0175
	afx_msg void OnClicklstCementProperties(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C3040BB026F
	afx_msg void OnKeydownlstCementProperties(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C3040BB0398
	afx_msg void OnKillfocusebNumberOfCementScenarios();
	//##ModelId=3C60D57B0010
	afx_msg void OnEdit();
	//##ModelId=3C60D57B0020
	afx_msg void OnFromLibrary();
	//##ModelId=3C60D57B0022
	afx_msg void OnNew();
	//##ModelId=3C60D57B002F
	afx_msg void OnToLibrary();
	//##ModelId=3C6A14C90211
	afx_msg void OnItemchanginglstCementPropertiesParams(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEMENTPROPERTIESFORM_H__E33D1799_DE69_4BC8_A81B_DCD2F4B3753C__INCLUDED_)
