// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1A58870D_C19A_4EAD_AC62_87ACE2DC456B__INCLUDED_)
#define AFX_STDAFX_H__1A58870D_C19A_4EAD_AC62_87ACE2DC456B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#include <afxext.h>   // MFC extensions
#include <afxwin.h>   // MFC core and standard components
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1A58870D_C19A_4EAD_AC62_87ACE2DC456B__INCLUDED_)
