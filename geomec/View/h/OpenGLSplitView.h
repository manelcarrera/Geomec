#if !defined(AFX_OPENGLSPLITVIEW_H__26091476_682A_4E40_B26C_94C78A93F666__INCLUDED_)
#define AFX_OPENGLSPLITVIEW_H__26091476_682A_4E40_B26C_94C78A93F666__INCLUDED_

#include "GeomecSplitterWnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLSplitView.h : header file
//

// The OpenGLSplit view combines the COpenGLLegendView and the
// COpenGLModelView in one window.

// ##ModelId=3B6549E10205
class COpenGLSplitView : public CView {
  // ##ModelId=3B6549E102C2
  CGeomecSplitterWnd m_wndSplitter;

protected:
  // ##ModelId=3B6549E102D0
  COpenGLSplitView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(COpenGLSplitView)

  // Attributes
public:
  // Operations
public:
  void GetColumnInfo(int col, int &cxCur, int &cxMin, bool bOriginal = false) const;
  void SetColumnInfo(int col, int cxIdeal, int cxMin);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(COpenGLSplitView)
public:
  // ##ModelId=3B6549E102D1
  virtual void OnInitialUpdate();

protected:
  // ##ModelId=3B6549E102D3
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // ##ModelId=3B6549E102E2
  virtual ~COpenGLSplitView();
#ifdef _DEBUG
  // ##ModelId=3B6549E102E4
  virtual void AssertValid() const;
  // ##ModelId=3B6549E102E6
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(COpenGLSplitView)
  // ##ModelId=3B6549E102F1
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  // ##ModelId=3B6549E102F4
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg BOOL OnEraseBkgnd(CDC *pDC);
  //}}AFX_MSG
  afx_msg BOOL OnMouseWheel(unsigned int nFlags, short zDelta, CPoint pt);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  DECLARE_MESSAGE_MAP()

private:
  bool m_bRecursionLock;
  int m_originalCx;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLSPLITVIEW_H__26091476_682A_4E40_B26C_94C78A93F666__INCLUDED_)
