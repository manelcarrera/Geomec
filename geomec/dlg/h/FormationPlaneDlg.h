#if !defined(AFX_FORMATIONPLANEDLG_H__2EDA671A_E1BA_4B33_AAC0_9401B5B825C4__INCLUDED_)
#define AFX_FORMATIONPLANEDLG_H__2EDA671A_E1BA_4B33_AAC0_9401B5B825C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormationPlaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormationPlaneDlg dialog

class CFormationPlaneDlg : public CDialog {
  // Construction
public:
  CFormationPlaneDlg(CWnd *pParent = NULL); // standard constructor

  void SetPlane(int level, int state);
  int GetPlaneLevel(int level);

  // Dialog Data
  //{{AFX_DATA(CFormationPlaneDlg)
  enum { IDD = IDD_FORMATIONPLANEDLG };
  int m_nTop;
  int m_nMiddle;
  int m_nBottom;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFormationPlaneDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation

protected:
  // Generated message map functions
  //{{AFX_MSG(CFormationPlaneDlg)
  // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATIONPLANEDLG_H__2EDA671A_E1BA_4B33_AAC0_9401B5B825C4__INCLUDED_)
