#include "stdafx.h"

#include "HorizonBase.h"
#include "HorizonBase_Delegate.h"

CHorizonBase_Delegate::CHorizonBase_Delegate(CHorizonBase *horizonBase)
    : CColorNode_Delegate(horizonBase), m_horizonBase(horizonBase) {}

bool CHorizonBase_Delegate::Destroy() { return m_horizonBase->Destroy(); }
