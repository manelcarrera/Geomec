#if !defined(AFX_WIZZARDPAGEBASE_H__48CC8529_FB53_4843_836E_BE171C27FFC2__INCLUDED_)
#define AFX_WIZZARDPAGEBASE_H__48CC8529_FB53_4843_836E_BE171C27FFC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// wizzardpagebase.h : header file
//

#include "treectrlbase.h"

/////////////////////////////////////////////////////////////////////////////
// CWizzardPageBase dialog

class CWizzardPageBase : public CPropertyPage
{
	class CPageTreeItem : public ITreeObject
	{
		CPropertyPage& m_page;
	public:
		CPageTreeItem(CPropertyPage& page, CTreeCtrl &ctrl);
		virtual QString Text() const;
		virtual unsigned int Icon() const;
		virtual BOOL OnSelect();

	};
	unsigned int m_uCaption;
// Construction
public:
	CWizzardPageBase(unsigned int uTemplateID, unsigned int uCaption);   // standard constructor
	const CPropertySheet& Sheet() const;
	CPropertySheet& Sheet();
	virtual BOOL Enable() const;
	virtual CString Text() const;

// Dialog Data
	//{{AFX_DATA(CWizzardPageBase)
	enum { IDD = IDD_TEMPLATE_WIZZARD };
	CTreeCtrlBase	m_lcOverview;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizzardPageBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWizzardPageBase)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZZARDPAGEBASE_H__48CC8529_FB53_4843_836E_BE171C27FFC2__INCLUDED_)
