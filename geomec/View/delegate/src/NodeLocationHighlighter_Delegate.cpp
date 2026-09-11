#include "stdafx.h"

#include "NodeLocationHighlighter_Delegate.h"

CNodeLocationHighlighter_Delegate::CNodeLocationHighlighter_Delegate(
  CNodeLocationHighlighter* nodeLocationHighlighter)
: m_nodeLocationHighlighter(nodeLocationHighlighter)
{
}

CNodeLocationHighlighter_Delegate::CPointHighlightNode_Delegate::
  CPointHighlightNode_Delegate(
  CNodeLocationHighlighter::CPointHighlightNode* pointHighlightNode)
: COpenGLNode_Delegate(pointHighlightNode)
, m_pointHighlightNode(pointHighlightNode)
{
}
