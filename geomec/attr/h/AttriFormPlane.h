#if !defined(AFX_ATTRIFORMPLANE_H__7436E6C9_59A5_49C7_AFD0_2A488AA80868__INCLUDED_)
#define AFX_ATTRIFORMPLANE_H__7436E6C9_59A5_49C7_AFD0_2A488AA80868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttriFormPlane.h : header file
//

#include "FormationPlane.h"
#include "AttributesTemplate.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAttriFormPlane dialog

class CAttriFormPlane : public  CAttributesTemplate<CFormationPlane>
{
public:
  CAttriFormPlane(CFormationPlane &plane, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriFormPlane)
  enum { IDD = IDD_ATTRI_FORMPLANE_DLG };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriFormPlane)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriFormPlane)
  afx_msg void OnColor();
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIFORMPLANE_H__7436E6C9_59A5_49C7_AFD0_2A488AA80868__INCLUDED_)
