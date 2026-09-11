#if !defined(AFX_NONLINPARAMS_H__C487F4E4_F56F_4F11_A5AB_F76D4EB1C53F__INCLUDED_)
#define AFX_NONLINPARAMS_H__C487F4E4_F56F_4F11_A5AB_F76D4EB1C53F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NonlinParams.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNonlinParams dialog
class CModelBase;
class CDepletionStage;
class CNonlinParams : public CDialog {
  CModelBase &m_model;
  typedef std::pair<CDepletionStage *, CDepletionStage *> TDepletionStagePair;
  typedef std::vector<TDepletionStagePair> TDepletionStageVec;
  TDepletionStageVec m_vcDepletionStage;

  int m_iPreviousSelection;

  void UpdateStageControls();

  // Construction
public:
  CNonlinParams(CModelBase &model, CWnd *pParent = NULL); // standard constructor
  virtual ~CNonlinParams();

  // Dialog Data
  //{{AFX_DATA(CNonlinParams)
  enum { IDD = IDD_NONLIN_PARAMS };
  CComboBox m_cbDepletionStage;
  BOOL m_bApplyAll;
  BOOL m_bLargeDeform;
  BOOL m_bAllowLinearCalc;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNonlinParams)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CNonlinParams)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSelchangeDepletionstage();
  afx_msg void OnCheckApplyAll();
  afx_msg void OnSetDefault();
  afx_msg void OnLargeDeform();
  afx_msg void OnAllowLinearCalc();
  afx_msg void OnHelpClicked();
  afx_msg void OnAutoStepSizes();
  afx_msg void OnManualStepSizes();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedRadio2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NONLINPARAMS_H__C487F4E4_F56F_4F11_A5AB_F76D4EB1C53F__INCLUDED_)
