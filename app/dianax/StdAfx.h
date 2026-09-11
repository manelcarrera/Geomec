#if !defined(AFX_STDAFX_H__3A01BC1B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
#define AFX_STDAFX_H__3A01BC1B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

#include <afxctl.h>         // MFC support for ActiveX Controls

#include <qobject.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3A01BC1B_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
