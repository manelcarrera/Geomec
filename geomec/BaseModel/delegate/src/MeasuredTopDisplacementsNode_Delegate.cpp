#include "stdafx.h"

#include "Geomec.h" // Mandatory order, before "MeasuredTopDisplacementsNode.h"
#include "MeasuredTopDisplacementsNode.h"
#include "MeasuredTopDisplacementsNode_Delegate.h"

CMeasuredTopDisplacementsNode_Delegate::CMeasuredTopDisplacementsNode_Delegate(
    CMeasuredTopDisplacementsNode *measuredTopDisplacementsNode)
    : CStorageNode_Delegate(measuredTopDisplacementsNode),
      m_measuredTopDisplacementsNode(measuredTopDisplacementsNode) {}
