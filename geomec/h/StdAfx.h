// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7EAE0B86_ED67_49E9_AF0D_962886E0DC12__INCLUDED_)
#define AFX_STDAFX_H__7EAE0B86_ED67_49E9_AF0D_962886E0DC12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES
#else
#include <math.h>
#endif

#include "Geometry.h"		// Geometry

#ifdef VERIFY
#undef VERIFY
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes

#ifndef _WIN64
#include <afxdb.h>	
#include <afxdao.h>
#endif

//QT libaries..
#include <qobject.h>

// TNO libraries ...
#include "tsmesh.h"			// Mesher
#include "QuantityInclude.h"// Quantities
#include "DianaCore.h"		// Diana meshing and calculation
#include "RPN_INC.h"		// Old RPN Shit 
#include "WellScheme.h"		// Well objects
#include "resource.h"		// main symbols

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7EAE0B86_ED67_49E9_AF0D_962886E0DC12__INCLUDED_)
