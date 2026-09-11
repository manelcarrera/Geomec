#if !defined(AFX_HORIZONDELDLG_H__55165B4B_256A_4250_92B4_4F2C136D54A6__INCLUDED_)
#define AFX_HORIZONDELDLG_H__55165B4B_256A_4250_92B4_4F2C136D54A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// horizondeldlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHorizonDelDlg dialog

#include "HexaFormation.h"

class CHorizonDelDlg : public CDialog
{
  CHexaFormation *m_pLower, *m_pUpper;
// Construction
public:
  CHorizonDelDlg(CHexaFormation *pLower, CHexaFormation *pUpper, CWnd* pParent = NULL);   // standard constructor
  const CHexaFormation& Selection() const;	
// Dialog Data
  //{{AFX_DATA(CHorizonDelDlg)
  enum { IDD = IDD_DELHORIZONDLG };
  int		m_nUpper;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHorizonDelDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL
  

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CHorizonDelDlg)
    // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HORIZONDELDLG_H__55165B4B_256A_4250_92B4_4F2C136D54A6__INCLUDED_)
