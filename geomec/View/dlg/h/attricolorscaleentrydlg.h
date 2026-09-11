#if !defined(AFX_ATTRICOLORSCALEENTRYDLG_H__FD497DAD_B094_46DE_8ED4_6026FDDC1F0E__INCLUDED_)
#define AFX_ATTRICOLORSCALEENTRYDLG_H__FD497DAD_B094_46DE_8ED4_6026FDDC1F0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attricolorscaleentrydlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriColorScaleEntryDlg dialog
class CColorScaleEntry;

#include "resource.h"

class CAttriColorScaleEntryDlg : public CDialog
{
// Construction
  CColorScaleEntry& m_entry;
  void OnUpdateHotSpotBox();
  void OnUpdateGlobalScaleBox();
public:
  CAttriColorScaleEntryDlg(CColorScaleEntry& entry, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriColorScaleEntryDlg)
  enum { IDD = IDD_ATTRI_COLOR_SCALE_ENTRY };
  CComboBox	m_cbHotSpot;
  CComboBox	m_cbGlobal;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriColorScaleEntryDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriColorScaleEntryDlg)
  afx_msg void OnDeleteGlobalScale();
  afx_msg void OnNewGlobalScale();
  afx_msg void OnAttributesGlobalScale();
  afx_msg void OnCopyGlobalScale();
  afx_msg void OnAttributesHotSpot();
  afx_msg void OnDeleteHotSpot();
  afx_msg void OnNewHotSpot();
  virtual BOOL OnInitDialog();
  afx_msg void OnGlobalSelectionChanged();
  afx_msg void OnHotSpotSelectionChanged();
  afx_msg void OnSelectionChanged();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRICOLORSCALEENTRYDLG_H__FD497DAD_B094_46DE_8ED4_6026FDDC1F0E__INCLUDED_)
