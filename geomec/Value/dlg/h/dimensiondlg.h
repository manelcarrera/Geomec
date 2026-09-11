#if !defined(AFX_DIMENSIONDLG_H__998ACB2C_C98C_47B5_9D4F_F64D14E30795__INCLUDED_)
#define AFX_DIMENSIONDLG_H__998ACB2C_C98C_47B5_9D4F_F64D14E30795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dimensiondlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDimensionDlg dialog

#include "PointSet.h"

class CDimensionDlg : public CDialog
{
  CPointSet::DIMENSION m_dim;
// Construction
public:
  CDimensionDlg(CPointSet::DIMENSION dim, CWnd* pParent = NULL);   // standard constructor
  CPointSet::DIMENSION Dimension() const;
// Dialog Data
  //{{AFX_DATA(CDimensionDlg)
  enum { IDD = IDD_SET_DIMENSION_DLG };
  //int		nDim;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDimensionDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CDimensionDlg)
    // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMENSIONDLG_H__998ACB2C_C98C_47B5_9D4F_F64D14E30795__INCLUDED_)
