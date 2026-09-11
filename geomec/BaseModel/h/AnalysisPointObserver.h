// AnalysisPointObserver.h: interface for the CAnalysisPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSISPOINTOBSERVER_H__E9134DBC_7384_4776_AD54_DD1AE0A08123__INCLUDED_)
#define AFX_ANALYSISPOINTOBSERVER_H__E9134DBC_7384_4776_AD54_DD1AE0A08123__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGraphEntry_Delegate;
class CAnalysisPointEntry_Delegate;

#include "OpenGLNodeObserver.h"
#include "StateBranch_Delegate.h"

class CModelBase;

#include "AnalysisPoint_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CAnalysisPoint, CAnalysisPoint_Delegate, CDummyNode, CDummyObserver, FALSE,
                                     DELETE_ITEM>
    TAnalysisPointObserver;
typedef CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, CAnalysisPoint, TAnalysisPointObserver,
                              CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TAnalysisPointEntryObserver;

#endif // !defined(AFX_ANALYSISPOINTOBSERVER_H__E9134DBC_7384_4776_AD54_DD1AE0A08123__INCLUDED_)
