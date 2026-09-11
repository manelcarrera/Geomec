// TestViewExe.h : main header file for the TESTVIEWEXE application
//

#if !defined(AFX_TESTVIEWEXE_H__614CD6D7_BEB3_4DB4_8CB6_FA5C73E32431__INCLUDED_)
#define AFX_TESTVIEWEXE_H__614CD6D7_BEB3_4DB4_8CB6_FA5C73E32431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestViewExeApp:
// See TestViewExe.cpp for the implementation of this class
//

class CTestViewExeApp : public CWinApp
{
public:
  CTestViewExeApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTestViewExeApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

// Implementation
  //{{AFX_MSG(CTestViewExeApp)
  afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEWEXE_H__614CD6D7_BEB3_4DB4_8CB6_FA5C73E32431__INCLUDED_)
