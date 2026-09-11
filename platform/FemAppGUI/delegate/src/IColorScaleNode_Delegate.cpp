#include "stdafx.h"

#include "IColorScaleNode.h"
#include "IColorScaleNode_Delegate.h"

IColorScaleNode_Delegate::IColorScaleNode_Delegate(IColorScaleNode *colorScaleNode)
    : CStorageNode_Delegate(colorScaleNode), m_colorScaleNode(colorScaleNode) {}
