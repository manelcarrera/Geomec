#if !defined(AFX_CUTSURFACETREEVIEW_H__5BF88561_9EE4_45E7_9C14_A002D197DC86__INCLUDED_)
#define AFX_CUTSURFACETREEVIEW_H__5BF88561_9EE4_45E7_9C14_A002D197DC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CutsurfaceTreeView.h : header file
//

#include <afxcview.h>


class CCutSurfaceFrame;

/////////////////////////////////////////////////////////////////////////////
// CCutsurfaceTreeView view

class CCutsurfaceTreeView : public CTreeView
{
  
protected:
  CCutsurfaceTreeView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CCutsurfaceTreeView)

// Attributes
public:

  void SetFrame(CCutSurfaceFrame* frame){m_pFrame=frame;}
// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CCutsurfaceTreeView)
  protected:
  virtual void OnDraw(CDC* pDC);      // overridden to draw this view
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CCutsurfaceTreeView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CCutsurfaceTreeView)
    // NOTE - the ClassWizard will add and remove member functions here.
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  void		OnLButtonDown(unsigned int nFlags,CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  CCutSurfaceFrame* m_pFrame;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTSURFACETREEVIEW_H__5BF88561_9EE4_45E7_9C14_A002D197DC86__INCLUDED_)
