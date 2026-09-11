#include "stdafx.h"

#include "IColorScaleNode_Delegate.h"
#include "IColorScaleNode.h"

IColorScaleNode_Delegate::IColorScaleNode_Delegate(
  IColorScaleNode* colorScaleNode)
: CStorageNode_Delegate(colorScaleNode)
, m_colorScaleNode(colorScaleNode)
{
}
