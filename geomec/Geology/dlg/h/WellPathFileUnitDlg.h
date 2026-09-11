#if !defined(AFX_WELLPATHFILEUNITDLG_H__47DE6BBB_E939_431C_8024_7A6AEC0F6CFD__INCLUDED_)
#define AFX_WELLPATHFILEUNITDLG_H__47DE6BBB_E939_431C_8024_7A6AEC0F6CFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellPathFileUnitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellPathFileUnitDlg dialog

class CWellPathFileUnitDlg : public CDialog {
  // Construction
  int m_Unit;

public:
  CWellPathFileUnitDlg(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CWellPathFileUnitDlg)
  enum { IDD = IDD_WELLFILE_UNIT };

  //}}AFX_DATA

  CQuantity::UNIT Unit();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellPathFileUnitDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CWellPathFileUnitDlg)
  // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLPATHFILEUNITDLG_H__47DE6BBB_E939_431C_8024_7A6AEC0F6CFD__INCLUDED_)
