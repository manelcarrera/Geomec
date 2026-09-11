// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D69CFEFD_79D1_443B_866D_88974F469459__INCLUDED_)
#define AFX_STDAFX_H__D69CFEFD_79D1_443B_866D_88974F469459__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "geometry.h"
#include "QuantityInclude.h"
#include "OpenGl.h"
#include "DoubleEdit.h"

#include <qptrlist.h> 
#include <qobject.h> 
#include "WellPoint.h"
#include "WellSection.h"
#include "WellSectionList.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"
#include "WellPointDrawSpec.h"
#include "WellSectionDrawSpec.h"
#include "WellSceneInterMed.h"
#include "WellOverburdenDrawSpec.h"
#include "WellPointPickDlg.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D69CFEFD_79D1_443B_866D_88974F469459__INCLUDED_)
