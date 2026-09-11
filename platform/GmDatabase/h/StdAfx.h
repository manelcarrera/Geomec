// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__98D2AF06_502B_4717_AF9B_89715F8A3436__INCLUDED_)
#define AFX_STDAFX_H__98D2AF06_502B_4717_AF9B_89715F8A3436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>
#ifndef _WIN64 // 64 bits Windows does not support DAO
#include <afxdb.h>
#include <afxdao.h>
#endif

#include <assert.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__98D2AF06_502B_4717_AF9B_89715F8A3436__INCLUDED_)
