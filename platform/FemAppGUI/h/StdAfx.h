// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E162EABC_F662_4BCD_A8D8_26DE37E6F39F__INCLUDED_)
#define AFX_STDAFX_H__E162EABC_F662_4BCD_A8D8_26DE37E6F39F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxext.h> // MFC extensions
#include <afxole.h> // MFC OLE classes
#include <afxwin.h>

// QT libaries..
#include <qobject.h>

#include "ContextMenuInvoker.h"
#include "DianaCore.h" // Diana meshing and calculation
#include "ICommand.h"
#include "QuantityInclude.h" // Quantity

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E162EABC_F662_4BCD_A8D8_26DE37E6F39F__INCLUDED_)
