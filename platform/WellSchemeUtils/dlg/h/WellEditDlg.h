#if !defined(AFX_WELLEDITDLG_H__D685FD78_303F_4C6A_84DF_8A6776630E63__INCLUDED_)
#define AFX_WELLEDITDLG_H__D685FD78_303F_4C6A_84DF_8A6776630E63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellEditDlg.h : header file
//

#include "SpreadView.h"
#include "resource.h"

namespace well {
class CWellPathBase;
class CWellPathBaseDocument;
}; // namespace well

/////////////////////////////////////////////////////////////////////////////
// CWellEditDlg dialog

class CWellEditView;
class CWellEditFrame;

class AFX_EXT_CLASS CWellEditDlg : public CDialog {
  friend class CWellEditFrame;

  // Construction
public:
  virtual ~CWellEditDlg();
  CWellEditDlg(const well::CWellPathBase &wellpath, QU::UNIT us, CWnd *pParent = NULL); // standard constructor
  virtual void OnOK();
  virtual void OnCancel();
  void SetAzimuth(bool azimuth);
  void SetType(DEF_TYPE type);

  // Dialog Data
  //{{AFX_DATA(CWellEditDlg)
  enum { IDD = UT_IDD_DLG_WELL_EDIT };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  well::CWellPathBase &WellPath() const;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellEditDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CWellEditDlg)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  virtual BOOL OnInitDialog();
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg void OnId3dvCopypicture();
  afx_msg void OnUpdateId3dvCopypicture(CCmdUI *pCmdUI);
  afx_msg void OnMenuApply();
  afx_msg void OnMenuClose();
  afx_msg void OnMenuOpen();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  CWellEditFrame *m_pFrameWnd;
  CSplitterWnd m_cSplitter;

  QU::UNIT m_Us;
  mutable well::CWellPathBase *m_pWellPath;
  well::CWellPathBaseDocument *m_doc;

  int m_Height;

  CSpreadView *m_pSpreadView;
  CWellEditView *m_pWellEditView;
  CToolBar m_3DViewToolBar;
  CDialogBar m_SpreadDlgbar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLEDITDLG_H__D685FD78_303F_4C6A_84DF_8A6776630E63__INCLUDED_)
