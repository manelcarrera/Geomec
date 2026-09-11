#if !defined(AFX_TREETESTVIEW_H__7C2E45DB_203D_432E_B338_80001D9B736D__INCLUDED_)
#define AFX_TREETESTVIEW_H__7C2E45DB_203D_432E_B338_80001D9B736D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// treetestview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeTestView view
#include "TreeViewBase.h"

class CTreeTestView : public CTreeViewBase
{
protected:
	CTreeTestView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeTestView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeTestView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeTestView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREETESTVIEW_H__7C2E45DB_203D_432E_B338_80001D9B736D__INCLUDED_)
