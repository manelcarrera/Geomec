#include "stdafx.h"

#include "HorizonBase_Delegate.h"
#include "HorizonBase.h"

CHorizonBase_Delegate::CHorizonBase_Delegate(CHorizonBase* horizonBase)
: CColorNode_Delegate(horizonBase)
, m_horizonBase(horizonBase)
{
}

bool CHorizonBase_Delegate::Destroy()
{
  return m_horizonBase->Destroy();
}
