#include "stdafx.h"

#include "GraphEntry_Delegate.h"

CGraphEntry_Delegate::CGraphEntry_Delegate(CGraphEntry* graphEntry)
: CGraphNode_Delegate(graphEntry)
, m_graphEntry(graphEntry)
{
}
