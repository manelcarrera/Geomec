#if !defined(AFX_GEOMECSTATUSBAR_H__BE8A8330_4810_4C1D_A43E_DAF0BB622B0B__INCLUDED_)
#define AFX_GEOMECSTATUSBAR_H__BE8A8330_4810_4C1D_A43E_DAF0BB622B0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// geomecstatusbar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeomecStatusBar window

// ##ModelId=3BE7AEC202C4
class CGeomecStatusBar : public CStatusBar {
  // Construction
public:
  // ##ModelId=3BE7AEC202D4
  CGeomecStatusBar();

  // Attributes
public:
  // Operations
public:
  int GetNeededWidth(const CString &text);
  int GetWidth(int nPaneIndex);
  void SetWidth(int nPaneIndex, int nWidth);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeomecStatusBar)
  //}}AFX_VIRTUAL

  // Implementation
public:
  // Generated message map functions
protected:
  //{{AFX_MSG(CGeomecStatusBar)
  // ##ModelId=3BE7AEC202D5
  afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOMECSTATUSBAR_H__BE8A8330_4810_4C1D_A43E_DAF0BB622B0B__INCLUDED_)
