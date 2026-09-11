#if !defined(AFX_MODELOBSERVER_H__660AC93F_E795_42A0_B295_A0161C2EFF7D__INCLUDED_)
#define AFX_MODELOBSERVER_H__660AC93F_E795_42A0_B295_A0161C2EFF7D__INCLUDED_

#include "OpenGLNodeObserver_Delegate.h"

#include "ZoominModelEntry.h"
#include "StateBranch_Delegate.h"
#include "ZoominModelEntry_Delegate.h"
#include "ResourceIDI.h"
#include "ZoominModelPlaceHolder.h"
#include "ZoominModelPlaceHolder_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CZoominModelPlaceHolder, CZoominModelPlaceHolder_Delegate, CDummyNode, CDummyObserver, TRUE, DELETE_ITEM> TZoominModelPlaceHolderObserver;
typedef CStateBranch_Delegate<CZoominModelEntry, CZoominModelEntry_Delegate, CZoominModelPlaceHolder, TZoominModelPlaceHolderObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM> TZoominModelEntryObserver;

#endif // !defined(AFX_MODELOBSERVER_H__660AC93F_E795_42A0_B295_A0161C2EFF7D__INCLUDED_)
