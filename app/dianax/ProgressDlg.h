#if !defined(AFX_PROGRESSDLG_H__3A01BC2B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
#define AFX_PROGRESSDLG_H__3A01BC2B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProgressDlg.h : header file
//

class CDianaXCtrl;

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
  CProgressDlg(CWnd* pParent = NULL);   // standard constructor
  static CProgressDlg *m_pDlg;
  BOOL m_bStop;
  BOOL m_bStopIsOk;
  BOOL m_bAutoCloseDialog; //only close if not succeded 
  BOOL m_bAlwaysAutoCloseDialog; //alway close dialog

  void AddMessage( CString Message );
  void AddWarning( CString Warning );
  void DisplayError( CString Error );
  int CallBack();
  void Finish();

// Dialog Data
  //{{AFX_DATA(CProgressDlg)
  enum { IDD = IDD_DIANAX };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProgressDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  void DisplayLastItem( CListBox *ListBox );
  CDianaXCtrl *GetControl();

  // Generated message map functions
  //{{AFX_MSG(CProgressDlg)
  afx_msg void OnButStop();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__3A01BC2B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
