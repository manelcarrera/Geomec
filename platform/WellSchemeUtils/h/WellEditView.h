#if !defined(AFX_WELLEDITVIEW_H__CB96F736_0F8E_4FAD_9553_D2919D773541__INCLUDED_)
#define AFX_WELLEDITVIEW_H__CB96F736_0F8E_4FAD_9553_D2919D773541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WellEditView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellEditView view
#include "openglview.h"
#include "stdafx.h"
#include "wellpathbase.h"

class COpenGLScene;
class CWellEditDlg;
class CWellSceneInterMed;

class AFX_EXT_CLASS QCWellEditView : public COpenGLView, public QObject {};

class AFX_EXT_CLASS CWellEditView : public QCWellEditView {
  Q_OBJECT;
private slots:
  void geometryChanged();

private:
  friend class CWellEditDlg;

  COpenGLScene *m_pScene;
  well::CWellPathBase *m_pWellpath;
  CWellSceneInterMed *m_pWellSceneInterMed;
  QU::UNIT m_Us;

protected:
  CWellEditView(); // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CWellEditView)

  // Attributes
public:
  void Init(well::CWellPathBase &wellpath, QU::UNIT us);
  void UpdateWellView();

  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellEditView)
protected:
  //}}AFX_VIRTUAL

  // Implementation
protected:
  virtual ~CWellEditView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CWellEditView)
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLEDITVIEW_H__CB96F736_0F8E_4FAD_9553_D2919D773541__INCLUDED_)
