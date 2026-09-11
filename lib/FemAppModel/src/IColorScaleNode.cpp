// IColorScaleNode.cpp: implementation of the IColorScaleNode class.
//
//////////////////////////////////////////////////////////////////////
#include "IColorScaleNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IColorScaleNode::IColorScaleNode(CFemAppModel &model)
: CStorageNode(model)
{
}

IColorScaleNode::IColorScaleNode(const unsigned int uName, CFemAppModel &model)
: CStorageNode(uName, model)
{
}

IColorScaleNode::IColorScaleNode(const QString& strName, CFemAppModel &model)
: CStorageNode(strName, model)
{
}

IColorScaleNode::IColorScaleNode(const IColorScaleNode &rhs)
: CStorageNode(rhs)
{
}

bool IColorScaleNode::operator==(const IColorScaleNode& rhs) const
{
	return CStorageNode::operator ==(rhs);
}
	
IColorScaleNode& IColorScaleNode::operator=(const IColorScaleNode& rhs)
{
	CStorageNode::operator =(rhs);
	return *this;
}

IColorScaleNode::TColor IColorScaleNode::LoadColor(TSTREAM& stream)
{
	int nRed, nGreen, nBlue;
	stream >> nRed;
	stream >> nGreen;
	stream >> nBlue;
	return qRgb(nRed, nGreen, nBlue);
}

void IColorScaleNode::SaveColor(TSTREAM& stream, TColor color)
{
	int nRed = qRed(color);
	int nGreen = qGreen(color);
	int nBlue = qBlue(color);
	stream << nRed;
	stream << nGreen;
	stream << nBlue;
}

void IColorScaleNode::SetIndex(int nIndex)
{
	Index(nIndex);
}