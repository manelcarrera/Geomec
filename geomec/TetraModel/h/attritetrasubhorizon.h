#if !defined(AFX_ATTRITETRASUBHORIZON_H__1C61B2D4_626E_4499_8F27_DBBBB1602727__INCLUDED_)
#define AFX_ATTRITETRASUBHORIZON_H__1C61B2D4_626E_4499_8F27_DBBBB1602727__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attritetrasubhorizon.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriTetraSubHorizon dialog
#include "TetraSubHorizon.h"
#include "AttributesTemplate.h"

class CAttriTetraSubHorizon : public CAttributesTemplate<CTetraSubHorizon>
{
// Construction
public:
  CAttriTetraSubHorizon(CTetraSubHorizon& horizon, UINT uTemplateID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriTetraSubHorizon)
  enum { IDD = IDD_ATTRI_SUB_FAULT };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriTetraSubHorizon)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriTetraSubHorizon)
  afx_msg void OnPaint();
  afx_msg void OnStColor();
  afx_msg void OnStick();
  afx_msg void OnSlip();
  afx_msg void OnUserDefined();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  void EnableValues(BOOL val);
  void EnableStickValues(BOOL val);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRITETRASUBHORIZON_H__1C61B2D4_626E_4499_8F27_DBBBB1602727__INCLUDED_)
