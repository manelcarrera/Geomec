#if !defined(AFX_ATTRIGLOBALSTRESSDLG_H__A4E15BAF_741C_442D_8F81_94169D83C689__INCLUDED_)
#define AFX_ATTRIGLOBALSTRESSDLG_H__A4E15BAF_741C_442D_8F81_94169D83C689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attriglobalstressdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriGlobalStressDlg dialog

#include "AttributesTemplate.h"
#include "GlobalInitialStressNode.h"

class CAttriGlobalStressDlg : public CAttributesTemplate<CGlobalInitialStressNode> {
  // Construction
public:
  CAttriGlobalStressDlg(CGlobalInitialStressNode &stress, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAttriGlobalStressDlg)
  enum { IDD = IDD_ATTRI_GLOBAL_STRESS };
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriGlobalStressDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriGlobalStressDlg)
  //}}AFX_MSG
  BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIGLOBALSTRESSDLG_H__A4E15BAF_741C_442D_8F81_94169D83C689__INCLUDED_)
