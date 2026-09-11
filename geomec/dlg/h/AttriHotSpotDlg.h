#if !defined(AFX_ATTRIHOTSPOTDLG_H__77CD8BFE_CC63_429D_844D_11735C830D93__INCLUDED_)
#define AFX_ATTRIHOTSPOTDLG_H__77CD8BFE_CC63_429D_844D_11735C830D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttriHotSpotDlg.h : header file
//
#include "AttributesTemplate.h"
#include "HotSpot.h"
/////////////////////////////////////////////////////////////////////////////
// CAttriHotSpotDlg dialog

class COpenGLDlgView;

#include "resource.h"

class CAttriHotSpotDlg : public CAttributesTemplate<CHotSpot> {
  CPoint m_ptClicked;
  CRect m_rcMinRect;
  CRect m_rcMaxRect;
  CRect m_rcTopView;
  CRect m_rcBottomView;

  COpenGLDlgView *m_pTopOpenGLView;
  COpenGLDlgView *m_pBottomOpenGLView;

  // Construction
public:
  CAttriHotSpotDlg(CHotSpot &hot_spot, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAttriHotSpotDlg)
  enum { IDD = IDD_ATTRI_HOTSPOT };
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriHotSpotDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriHotSpotDlg)
  afx_msg void OnPaint();
  virtual BOOL OnInitDialog();
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
  afx_msg void OnChangeColor();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnRadioTop();
  afx_msg void OnRadioBottom();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIHOTSPOTDLG_H__77CD8BFE_CC63_429D_844D_11735C830D93__INCLUDED_)
