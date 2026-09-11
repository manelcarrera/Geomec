#include "stdafx.h"

#include "IScalingBox_Delegate.h"
#include "IScalingBox.h"

IScalingBox_Delegate::IScalingBox_Delegate(IScalingBox* scalingBox)
: CColorNode_Delegate(scalingBox)
, m_scalingBox(scalingBox)
{
}

void IScalingBox_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  CColorNode_Delegate::AppendContextMenu(invoker); 
}