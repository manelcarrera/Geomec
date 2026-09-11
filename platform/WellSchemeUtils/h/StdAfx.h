// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4AD6A0FD_70C3_4554_8A43_448472F9A9AE__INCLUDED_)
#define AFX_STDAFX_H__4AD6A0FD_70C3_4554_8A43_448472F9A9AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include <afxext.h> // MFC extensions
#include <afxwin.h> // MFC core and standard components

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>  // MFC Automation classes
#include <afxodlgs.h> // MFC OLE dialog classes
#include <afxole.h>   // MFC OLE classes
#endif                // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#ifndef _WIN64
#include <afxdb.h> // MFC ODBC database classes
#endif
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#ifndef _WIN64
#include <afxdao.h> // MFC DAO database classes
#endif
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include "DoubleEdit.h"
#include "WellPointDrawSpec.h"
#include "WellSceneInterMed.h"
#include "WellScheme.h"
#include "WellSectionDrawSpec.h"
#include <qfile.h>
#include <qobject.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4AD6A0FD_70C3_4554_8A43_448472F9A9AE__INCLUDED_)
