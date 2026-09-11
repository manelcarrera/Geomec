 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "BodyTriangle.h"
#include "IBody.h"

namespace geo {

CBodyTriangle::CBodyTriangle(IBody &parent, int nIndex)
: m_parent(parent), m_nIndex(nIndex)
{
}

CBodyTriangle::~CBodyTriangle()
{
}

const IBody* CBodyTriangle::Parent() const
{
	return &m_parent;
}

int CBodyTriangle::PointIndex(int nIndex) const
{
	return Parent()->PointIndex(BodyNode(nIndex));
}

size_t CBodyTriangle::NrOfNodes() const
{
	return Order() * 3;
}

const INode& CBodyTriangle::Node(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return Parent()->Node(BodyNode(nIndex));
}

void CBodyTriangle::Node(int nIndex, const IPoint& node)
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	m_parent.Node(BodyNode(nIndex), node);
}

size_t CBodyTriangle::Order() const
{
	return Parent()->Order();
}

int CBodyTriangle::BodyPoint(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfPoints());
	return Parent()->FacePointIndices(m_nIndex)[nIndex];
}

int CBodyTriangle::BodyNode(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return Parent()->FaceNodeIndices(m_nIndex)[nIndex];
}

int CBodyTriangle::Index() const
{
	return m_nIndex;
}

} // namespace geo
