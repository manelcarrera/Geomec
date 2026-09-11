#if !defined(AFX_SELSURFFORMVIEW_H__EDE67DA8_FEDB_4231_9E37_5FD396E72A58__INCLUDED_)
#define AFX_SELSURFFORMVIEW_H__EDE67DA8_FEDB_4231_9E37_5FD396E72A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelSurfFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelSurfFormView form view

#include "HScrollListBox.h"
#include "NamedSurfaceMap.h"
#include "SelSurfDlg.h"

class SelSurfGLView;
class CGeoObjectHandlerBase;
class CDisplayPair;

class AFX_EXT_CLASS SelSurfFormView : public CFormView {
  friend class SelSurfDlg;

protected:
  SelSurfFormView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(SelSurfFormView)

  // Form Data
public:
  //{{AFX_DATA(SelSurfFormView)
  // enum { IDD = IDD_FORMVIEW_SELECT_SURF };
  CHScrollListBox m_ListTo;
  CHScrollListBox m_ListFrom;
  //}}AFX_DATA

  // Attributes
public:
  // Operations
public:
  void Init(SelSurfDlg *dlg, SelSurfGLView *From, SelSurfGLView *To);
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(SelSurfFormView)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  virtual ~SelSurfFormView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
  //{{AFX_MSG(SelSurfFormView)
  afx_msg void OnButtonFrom();
  afx_msg void OnButtonFromAll();
  afx_msg void OnButtonTo();
  afx_msg void OnButtonToAll();
  afx_msg void OnSelchangeListFrom();
  afx_msg void OnSelchangeListTo();
  afx_msg void OnButtonOk();
  afx_msg void OnButtonCancel();
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  void DoFromTo(SelSurfGLView *ViewFrom, CListBox *FromList, SelSurfGLView *ViewTo, CListBox *ToList, bool all);
  void SwitchPairInView(CDisplayPair *pair, SelSurfGLView *ViewFrom, SelSurfGLView *ViewTo);
  void HandleSelect(SelSurfGLView *View, CListBox *List);
  void UpdateSelectionInViews();

  void GetMap(CListBox *List, CNamedSurfaces &);

  SelSurfGLView *m_ViewFrom;
  SelSurfGLView *m_ViewTo;
  SelSurfDlg *m_dlg;
  CGeoObjectHandlerBase *m_geo_handler;

  void GetSelecetedSurfaces(CNamedSurfaces &surfaces);
  void GetUnSelecetedSurfaces(CNamedSurfaces &surfaces);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELSURFFORMVIEW_H__EDE67DA8_FEDB_4231_9E37_5FD396E72A58__INCLUDED_)
