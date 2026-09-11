#if !defined(AFX_LEGENDVIEW_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_)
#define AFX_LEGENDVIEW_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LegendView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLegendView view
class CGeomecDoc;

#include "OpenGLView.h"

// ##ModelId=3B6549E20205
class CLegendView : public COpenGLView {
protected:
  // ##ModelId=3B6549E202B1
  CLegendView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CLegendView)
public:
  static void DoOnRButtonUp(CPoint &screenpoint, CGeomecDoc &doc);
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CLegendView)
public:
protected:
  afx_msg void OnRButtonUp(unsigned int nFlags, CPoint point);

  //}}AFX_VIRTUAL

  // Implementation
protected:
  // ##ModelId=3B6549E202B2
  virtual ~CLegendView();
#ifdef _DEBUG
  // ##ModelId=3B6549E202B4
  virtual void AssertValid() const;
  // ##ModelId=3B6549E202C2
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CLegendView)
  // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  DECLARE_MESSAGE_MAP()
  virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);

private:
  void UpdateScrollBars();
  CLegendFrame &LegendFrame();
  int HorizontalScrollRange();
  int VerticalScrollRange();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEGENDVIEW_H__F091B018_D3AF_46E5_BD86_4F980AFFCDFD__INCLUDED_)
