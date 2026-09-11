// StdAfx.h: interface for the CStdAfx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__0F42F136_6E4C_4B4F_B676_49A75B6DC7C4__INCLUDED_)
#define AFX_STDAFX_H__0F42F136_6E4C_4B4F_B676_49A75B6DC7C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif



#include "Conus.h"
#include "Polygon.h"

#include <QObject> 
#include <QList> 
#include <QFile> 
#include "WellPoint.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"


#endif // !defined(AFX_STDAFX_H__0F42F136_6E4C_4B4F_B676_49A75B6DC7C4__INCLUDED_)
