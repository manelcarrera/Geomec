// DSealect.h : main header file for the DSEALECT application
//

#if !defined(AFX_DSEALECT_H__7E822FC6_1828_4079_B0F0_C572C58AFDF4__INCLUDED_)
#define AFX_DSEALECT_H__7E822FC6_1828_4079_B0F0_C572C58AFDF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class IRegistry;

#include "resource.h"       // main symbols





/////////////////////////////////////////////////////////////////////////////
// CDSealectApp:
// See DSealect.cpp for the implementation of this class
//


//##ModelId=3C0DC15C02AF
class CDSealectApp : public CWinApp
{
private:
	//##ModelId=3CA011A60167
	virtual BOOL OnIdle(LONG lCount);


public:
	//##ModelId=3C204402038B
	IRegistry* RegistryX();
	//##ModelId=3C0DC15C02C0
	CDSealectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSealectApp)
	public:
	//##ModelId=3C0DC15C02C1
	virtual BOOL InitInstance();
	//##ModelId=3C204402038C
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDSealectApp)
	//##ModelId=3C0DC15C02C3
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CDSealectApp* GetApp();





/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DSEALECT_H__7E822FC6_1828_4079_B0F0_C572C58AFDF4__INCLUDED_)
