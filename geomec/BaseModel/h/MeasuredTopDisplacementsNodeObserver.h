#ifndef _MEASUREDTOPDISPLACEMENTSNODEOBSERVER_H_
#define _MEASUREDTOPDISPLACEMENTSNODEOBSERVER_H_

class CMeasuredTopDisplacementsNode_Delegate;
class IValueComponent_Delegate;

#include "ValueVector.h"
#include "NodeObserver_Delegate.h"

class C3DModel;

#include "ValueVector_Delegate.h"
#include "RpnOperand.h"

typedef CNodeObserver_Delegate<IValueComponentBase, IValueComponent_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TVectorComponentObserver;
typedef CNodeObserver_Delegate<TDisplacementVector, TDisplacementVector_Delegate, IValueComponentBase, TVectorComponentObserver, FALSE, UNLINK_ITEM> TDisplacementVectorObserver;
typedef CNodeObserver_Delegate<CMeasuredTopDisplacementsNode, CMeasuredTopDisplacementsNode_Delegate, TDisplacementVector, TDisplacementVectorObserver, FALSE, UNLINK_ITEM> TMeasuredTopDisplacementsNodeObserver;

#endif // _MEASUREDTOPDISPLACEMENTSNODEOBSERVER_H_
