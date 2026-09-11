#if !defined(AFX_LISTVIEWBASE_H__95F38542_FACA_11D5_B8F6_444553540000__INCLUDED_)
#define AFX_LISTVIEWBASE_H__95F38542_FACA_11D5_B8F6_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// listviewbase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListViewBase view

#include "ListCtrlBase.h"

class CListViewBase : public CView
{
  CListCtrlBase *m_pCtrl;

protected:
  CListViewBase();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CListViewBase)

public:
  CListCtrlBase& GetListCtrl() const;
  virtual CListCtrlBase* OnCreateCtrl();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListViewBase)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
protected:

  virtual ~CListViewBase();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CListViewBase)
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTVIEWBASE_H__95F38542_FACA_11D5_B8F6_444553540000__INCLUDED_)
