#if !defined(AFX_TREEVIEWBASE_H__EBD66F84_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
#define AFX_TREEVIEWBASE_H__EBD66F84_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// treeviewbase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeViewBase view

#include <afxcview.h>
#include "treectrlbase.h"

class CTreeViewBase : public CView
{
  CTreeCtrlBase* m_pCtrl;

protected:
  CTreeViewBase();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CTreeViewBase)

public:
  CTreeCtrlBase& GetTreeCtrl() const;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTreeViewBase)
  protected:
  virtual void OnDraw(CDC* pDC);
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CTreeViewBase();
  virtual CTreeCtrlBase* OnCreateCtrl();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CTreeViewBase)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWBASE_H__EBD66F84_F63B_11D5_B8F5_F2227CA4A730__INCLUDED_)
