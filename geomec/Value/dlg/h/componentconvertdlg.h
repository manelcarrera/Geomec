#if !defined(AFX_COMPONENTCONVERTDLG_H__BB73934D_DEEB_4C64_8575_1F01AB1F432D__INCLUDED_)
#define AFX_COMPONENTCONVERTDLG_H__BB73934D_DEEB_4C64_8575_1F01AB1F432D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// componentconvertdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComponentConvertDlg dialog
class IPointSet;
class CValueType;

class CComponentConvertDlg : public CDialog {
  // Construction
  const CValueType &m_value_type;
  IPointSet &m_target;
  CString DlgText() const;

public:
  CComponentConvertDlg(const CValueType &value_type, IPointSet &target, CWnd *pParent = NULL); // standard constructor
  geo::CValueMapper::TWeightingType Type() const;

  // Dialog Data
  //{{AFX_DATA(CComponentConvertDlg)
  enum { IDD = IDD_COMPONENT_CONVERT };
  int m_nType;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CComponentConvertDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CComponentConvertDlg)
  // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPONENTCONVERTDLG_H__BB73934D_DEEB_4C64_8575_1F01AB1F432D__INCLUDED_)
