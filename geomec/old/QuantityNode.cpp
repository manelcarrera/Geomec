// QuantityNode.cpp: implementation of the CQuantityNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "QuantityNode.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuantityNode::CQuantityNode(const CString& strInstanceName)
:CGraphNode(strInstanceName)
{
}

CQuantityNode::CQuantityNode(UINT uInstanceName)
:CGraphNode(uInstanceName)
{
}

const COpenGLNode* CQuantityNode::ProjectOn() const
{
	return 0;
}

UINT CQuantityNode::TypeNameId() const
{
	return 0;
}


