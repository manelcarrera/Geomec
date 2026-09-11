#if !defined(AFX_ATTRISURFACEDLG_H__E90AA56A_DE9D_41E6_80EB_ACD4F7091B1A__INCLUDED_)
#define AFX_ATTRISURFACEDLG_H__E90AA56A_DE9D_41E6_80EB_ACD4F7091B1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrisurfacedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriSurfaceDlg dialog

#include "AttributesTemplate.h"
#include "GeoSurface.h"

class CAttriSurfaceDlg : public CAttributesTemplate<CSurfaceBase>
{

// Construction
public:
  CAttriSurfaceDlg(CSurfaceBase& surface, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriSurfaceDlg)
  enum { IDD = IDD_ATRI_SURFACE };
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriSurfaceDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriSurfaceDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnBtColor();
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRISURFACEDLG_H__E90AA56A_DE9D_41E6_80EB_ACD4F7091B1A__INCLUDED_)
