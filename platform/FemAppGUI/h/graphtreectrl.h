#if !defined(AFX_GRAPHTREECTRL_H__INCLUDED_)
#define AFX_GRAPHTREECTRL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// treectrlbase.h : header file
//

#include "TreeCtrlBase.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlBase window
class CGraphTreeView;
class CGraphTreeCtrl : public CTreeCtrlBase {
  CGraphTreeView &m_view;

public:
  CGraphTreeCtrl(CGraphTreeView &view);

protected:
  virtual void OnSelect(ITreeObject *pOld, ITreeObject *pNew);
  virtual void OnContext(const TTreeObjectVec &vcContext);

public:
  CGraphTreeView &TreeView();
  const CGraphTreeView &TreeView() const;

  // Overrides
  virtual CWnd &GetContextWnd() const;
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTreeCtrlBase)
  //}}AFX_VIRTUAL

  // Implementation
public:
  //	virtual ~CGraphTreeCtrl();

  // Generated message map functions
protected:
  //{{AFX_MSG(CTreeCtrlBase)
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFAFX_GRAPHTREECTRL_H__INCLUDED_)
