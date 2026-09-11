#if !defined(AFX_ECLIPSEHORIZONPAGE_H__A4548D20_DB5F_4498_AC80_0CF9F5F2955C__INCLUDED_)
#define AFX_ECLIPSEHORIZONPAGE_H__A4548D20_DB5F_4498_AC80_0CF9F5F2955C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// eclipsehorizonpage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEclipseHorizonPage dialog
#include "WizzardPageBase.h"
#include "EclipseModel.h"

class CEclipseHorizonPage : public CWizzardPageBase
{
  DECLARE_DYNCREATE(CEclipseHorizonPage)
// Construction
public:
  CEclipseHorizonPage();
  CEclipseHorizonPage(CEclipseModel& model);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CEclipseHorizonPage)
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CEclipseHorizonPage)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CEclipseHorizonPage)
    // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECLIPSEHORIZONPAGE_H__A4548D20_DB5F_4498_AC80_0CF9F5F2955C__INCLUDED_)
