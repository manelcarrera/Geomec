#if !defined(AFX_DIANAX_H__3A01BC1D_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
#define AFX_DIANAX_H__3A01BC1D_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DianaX.h : main header file for DIANAX.DLL

#if !defined(__AFXCTL_H__)
#error include 'afxctl.h' before including this file
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDianaXApp : See DianaX.cpp for implementation.

class CDianaXApp : public COleControlModule {
public:
  BOOL InitInstance();
  int ExitInstance();
  DWORD m_dwTlsControlIndex;
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIANAX_H__3A01BC1D_BA9F_11D2_B3B4_00104B2556D9__INCLUDED)
