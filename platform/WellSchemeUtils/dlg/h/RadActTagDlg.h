#if !defined(AFX_RADACTTAGDLG_H__40465161_6F3E_4E10_A97B_85DE7701D432__INCLUDED_)
#define AFX_RADACTTAGDLG_H__40465161_6F3E_4E10_A97B_85DE7701D432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadActTagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRadActTagDlg dialog

class AFX_EXT_CLASS CRadActTagDlg : public CDialog
{
  const well::CWellLog *m_pWellLog;
// Construction
public:
  CRadActTagDlg(const well::CWellLog *pWellLog, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CRadActTagDlg)
  enum { IDD = UT_IDD_RADACT_DLG };
  CListCtrl	m_RadActListCtrl;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRadActTagDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CRadActTagDlg)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADACTTAGDLG_H__40465161_6F3E_4E10_A97B_85DE7701D432__INCLUDED_)
