#include "stdafx.h"

#include "IPointSet_Delegate.h"
#include "IPointSet.h"

IPointSet_Delegate::IPointSet_Delegate(IPointSet* pointSet)
: CColorNode_Delegate(pointSet)
, m_pointSet(pointSet)
{
}

void IPointSet_Delegate::Export()
{
  assert(false);
}
