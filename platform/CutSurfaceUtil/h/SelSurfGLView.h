#if !defined(CUTSURF_GLVIEWINCLUDED_)
#define CUTSURF_GLVIEWINCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//

/////////////////////////////////////////////////////////////////////////////
// SelSurfGLView view
#include "openglview.h"


class COpenGLScene;



class SelSurfGLView : public COpenGLView
{


private:

  COpenGLScene* m_pScene;
  //QU::UNIT m_Us;

protected:
  SelSurfGLView();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(SelSurfGLView)

// Attributes
public:

  COpenGLScene* CurrentScene(); 



// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(SelSurfGLView)
  protected:
  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~SelSurfGLView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(SelSurfGLView)
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
