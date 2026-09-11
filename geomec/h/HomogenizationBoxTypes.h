#ifndef _HomogenizationBoxTypes_h_
#define _HomogenizationBoxTypes_h_

#include "HomogenizationBox_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CHomogenizationBox, CHomogenizationBox_Delegate, CDummyNode, CDummyObserver, TRUE,
                                     UNLINK_ITEM>
    CHomogenizationBoxObs;
typedef CStateBranch_Delegate<CHomoBoxEntry, CHomoBoxEntry_Delegate, CHomogenizationBox, CHomogenizationBoxObs,
                              CUndefinedIconProvider, TRUE, DELETE_ITEM>
    THomoEntryBoxObserver;

#endif // _HomogenizationBoxTypes_h_
