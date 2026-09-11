#if !defined(AFX_FACESUPPORTDLG_H__1F66ABBD_7100_457F_88D5_42B7A63E249F__INCLUDED_)
#define AFX_FACESUPPORTDLG_H__1F66ABBD_7100_457F_88D5_42B7A63E249F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// facesupportdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFaceSupportDlg dialog

#include "SupportDlgBase.h"

class CFaceSupportDlg : public CSupportDlgBase {
  // Construction
  void SetButton(unsigned int uButton, bool bCheck);
  bool GetButton(unsigned int uButton);

public:
  CFaceSupportDlg(CPressureSupportNode::CFaceSupportDef &supdef, const CPressureSupportNode &node,
                  CWnd *pParent = NULL);

  // Dialog Data
  //{{AFX_DATA(CFaceSupportDlg)
  enum { IDD = IDD_ATTRI_FACE_SUPPORT_SHORT };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFaceSupportDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CFaceSupportDlg)
  afx_msg void OnSelchangeStage();
  afx_msg void OnLocal();
  afx_msg void OnGlobal();
  afx_msg void OnUnloaded();
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnStress();
  afx_msg void OnSupported();
  //}}AFX_MSG
  afx_msg void OnSupportModeChanged();
  DECLARE_MESSAGE_MAP()

private:
  CPressureSupportNode::CFaceSupportDef &m_SupportDef;

  virtual void FillDepletionStageCombo();
  virtual void SetRadioButton();
  virtual void GetRadioButton();
  virtual void UpdateControls();
  virtual void FillUnitLabels();
  virtual void EmptyEditBoxes();

  typedef CPressureSupportNode::CFaceSupportDef::CSupportDef::TPressureType TPressureType;
  TPressureType PressureType();
  void PressureType(TPressureType nType);

  typedef CPressureSupportNode::CFaceSupportDef::TSupportType TSupportType;
  TSupportType SupportType();
  void SupportType(TSupportType nType);

  double UnitFactor();

  void SetSupportModeButton();
  void GetSupportModeButton();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACESUPPORTDLG_H__1F66ABBD_7100_457F_88D5_42B7A63E249F__INCLUDED_)
