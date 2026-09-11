// GeoSurfaceObserver.h: interface for the CSurfaceBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOSURFACEOBSERVER_H__F6EC9B60_1B76_4d06_ADA7_C4B4C456059C__INCLUDED_)
#define AFX_GEOSURFACEOBSERVER_H__F6EC9B60_1B76_4d06_ADA7_C4B4C456059C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSurfaceEntry_Delegate;

#include <afxtempl.h>
#include "TriaIntersecBox.h"
#include "ElementSet.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "UndefinedIconProvider.h"

typedef std::vector <STNamedSurface> CNamedSurfaces;

class SelSurfDlg;


class CSurfaceValueType;
class CSurfaceMeshZone;

#include "StateBranch_Delegate.h"
#include "geoSurface_Delegate.h"
#include "Plane.h"

typedef COpenGLNodeObserver_Delegate<CSurfaceBase, CSurfaceBase_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TSurfaceObs;
typedef CStateBranch_Delegate<CSurfaceEntry, CSurfaceEntry_Delegate, CSurfaceBase, TSurfaceObs, CUndefinedIconProvider, TRUE, DELETE_ITEM> TSurfaceEntryObserver;

#endif // !defined(AFX_GEOSURFACEOBSERVER_H__F6EC9B60_1B76_4d06_ADA7_C4B4C456059C__INCLUDED_)
