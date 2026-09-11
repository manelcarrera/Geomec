#include "stdafx.h"

#include "MeasuredTopDisplacementsNode_Delegate.h"
#include "Geomec.h" // Mandatory order, before "MeasuredTopDisplacementsNode.h"
#include "MeasuredTopDisplacementsNode.h"

CMeasuredTopDisplacementsNode_Delegate::CMeasuredTopDisplacementsNode_Delegate(
  CMeasuredTopDisplacementsNode* measuredTopDisplacementsNode)
: CStorageNode_Delegate(measuredTopDisplacementsNode)
, m_measuredTopDisplacementsNode(measuredTopDisplacementsNode)
{
}
