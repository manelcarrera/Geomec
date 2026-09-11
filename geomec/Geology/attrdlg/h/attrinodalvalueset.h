#if !defined(AFX_ATTRIVALUESET_H__91443EC9_F751_4C0D_8B8D_163E876D481F__INCLUDED_)
#define AFX_ATTRIVALUESET_H__91443EC9_F751_4C0D_8B8D_163E876D481F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrivalueset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriValueSet dialog

class CUnitNode;

#include "NodalValueSet.h"

class CAttriNodalValueSet : public CDialog {
  CNodalValueSet &m_value_set;

public:
  // Construction
  CAttriNodalValueSet(CNodalValueSet &value_set, CWnd *pParent = NULL);

  // Dialog Data
  //{{AFX_DATA(CAttriValueSet)
  enum { IDD = IDD_ATTRI_VALUE_SET };
  CListCtrl m_lcValue;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriNodalValueSet)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriValueSet)
  virtual BOOL OnInitDialog();

  afx_msg void OnGetDisplayInfo(NMHDR *pNMHDR, LRESULT *pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIVALUESET_H__91443EC9_F751_4C0D_8B8D_163E876D481F__INCLUDED_)
