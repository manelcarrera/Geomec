// ColorScaleNode.cpp: implementation of the CColorScaleNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "ColorScaleNode.h"
#include "OpenGLSceneBase.h"
#include "ColorGradient.h"
#include "hotspot.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorScaleNode::CColorScaleNode(CGraphModel &model)
: CStorageNode(model)
{
	// Child links to colorscale entry
}

CColorScaleNode::CColorScaleNode(const CString& strName, CGraphModel &model)
: CStorageNode(strName, model)
{
	// Child links to colorscale entry
}

CColorScaleNode::CColorScaleNode(const UINT uName, CGraphModel &model)
: CStorageNode(uName, model)
{
	// Child links to colorscale entry
}

CColorScaleNode::CColorScaleNode(const CColorScaleNode &rhs)
: CStorageNode(rhs)
{
	ASSERT(!rhs.IsCopy());
}

bool CColorScaleNode::operator==(const CColorScaleNode& rhs) const
{
	return CModelNode::operator ==(rhs);
}

CColorScaleNode& CColorScaleNode::operator=(const CColorScaleNode& rhs)
{
	CModelNode::operator=(rhs);
	return *this;
}

COLORREF CColorScaleNode::LoadColor(TSTREAM& stream)
{
	int nRed, nGreen, nBlue;
	stream >> nRed;
	stream >> nGreen;
	stream >> nBlue;
	return RGB(nRed, nGreen, nBlue);
}

void CColorScaleNode::SaveColor(TSTREAM& stream, COLORREF color)
{
	int nRed = GetRValue(color);
	int nGreen = GetGValue(color);
	int nBlue = GetBValue(color);
	stream << nRed;
	stream << nGreen;
	stream << nBlue;
}
