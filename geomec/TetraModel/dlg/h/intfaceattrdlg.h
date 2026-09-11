#if !defined(AFX_INTFACEATTRDLG_H__C456865A_B2A6_4755_B489_6292DC16CA8E__INCLUDED_)
#define AFX_INTFACEATTRDLG_H__C456865A_B2A6_4755_B489_6292DC16CA8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// intfaceattrdlg.h : header file
//
#include "doubleedit.h"
#include "stressgradientquantity.h"
#include "SingleQuantity.h"
#include "NoneQuantity.h"
/////////////////////////////////////////////////////////////////////////////
// CIntFaceAttrDlg dialog
class CBoundaryInterfaceDef;

#include "resource.h"

class CIntFaceAttrDlg : public CDialog
{
  CBoundaryInterfaceDef &m_def;
  CDoubleQuantity::UNIT m_unit;
  CStressGradientQuantity m_KradTop;
  CStressGradientQuantity m_KradBottom;
  CStressGradientQuantity m_KradSides;
  CStressGradientQuantity m_KtanTop;
  CStressGradientQuantity m_KtanBottom;
  CStressGradientQuantity m_KtanSides;
  CSinglePressure m_Shear;
  CNoneQuantity m_Lambda;

  bool IsBranch() const;

// Construction
public:
  CIntFaceAttrDlg(CBoundaryInterfaceDef &def, CDoubleQuantity::UNIT unit, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CIntFaceAttrDlg)
  enum { IDD = IDD_DIALOG_INTFACE_ATTR };
  CButton m_RadioLambda;
  CButton	m_RadioShear;
  CButton m_RadioKs;
  CDoubleEdit m_EdLambda;
  CDoubleEdit	m_EdShear;
  CDoubleEdit	m_EdKradTop;
  CDoubleEdit	m_EdKradBottom;
  CDoubleEdit	m_EdKradSides;
  CDoubleEdit	m_EdKtanTop;
  CDoubleEdit	m_EdKtanBottom;
  CDoubleEdit	m_EdKtanSides;
  int		m_nRadio;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CIntFaceAttrDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CIntFaceAttrDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnRadioLambda();
  afx_msg void OnRadioShear();
  afx_msg void OnRadioKs();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTFACEATTRDLG_H__C456865A_B2A6_4755_B489_6292DC16CA8E__INCLUDED_)
