#ifndef _WELLCASINGRTCIOBSERVER_H_
#define _WELLCASINGRTCIOBSERVER_H_

#include "OpenGLNodeObserver.h"

#include "ResultRegister.h"

class CWellCasingModel;

#include "OpenGLNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "WellCasingRTCI_Delegate.h"
#include "resourceIDI.h"

typedef COpenGLNodeObserver_Delegate<CWellCasingRTCI, CWellCasingRTCI_Delegate, CDummyNode, CDummyObserver, TRUE,
                                     UNLINK_ITEM>
    TWellCasingRTCIObserver;
typedef CStateBranch_Delegate<CWellCasingRTCIEntry, CWellCasingRTCIEntry_Delegate, CWellCasingRTCI,
                              TWellCasingRTCIObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TWellCasingRTCIEntryObserver;

#endif // _WELLCASINGRTCIOBSERVER_H_
