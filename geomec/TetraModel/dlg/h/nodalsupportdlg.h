#if !defined(AFX_NODALSUPPORTDLG_H__D108EA71_1011_4222_99FF_05404349BDF8__INCLUDED_)
#define AFX_NODALSUPPORTDLG_H__D108EA71_1011_4222_99FF_05404349BDF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// nodalsupportdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNodalSupportDlg dialog

#include "SupportDlgBase.h"

class CNodalSupportDlg : public CSupportDlgBase {
  // Construction
public:
  CNodalSupportDlg(CDisplacementSupportNode::CNodalSupportDef &supdef, const CDisplacementSupportNode &node,
                   CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CNodalSupportDlg)
  enum { IDD = IDD_ATTRI_NODAL_SUPPORT };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNodalSupportDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CNodalSupportDlg)
  afx_msg void OnSelchangeStage();
  afx_msg void OnLocal();
  afx_msg void OnGlobal();
  afx_msg void OnNone();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CDisplacementSupportNode::CNodalSupportDef &m_SupportDef;

  virtual void SetRadioButton();
  virtual void GetRadioButton();
  virtual void FillDepletionStageCombo();

  void UpdateControls();

  typedef CDisplacementSupportNode::CNodalSupportDef::CSupportDef::TSupportType TSupportType;
  TSupportType SupportType();
  void SupportType(TSupportType nType);
  void FillUnitLabels();
  double UnitFactor();
  virtual void EmptyEditBoxes();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODALSUPPORTDLG_H__D108EA71_1011_4222_99FF_05404349BDF8__INCLUDED_)
