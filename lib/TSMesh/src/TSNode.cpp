#include <assert.h>

#include "gts.h"
#include "TSNode.h"

//##ModelId=3BBD944902F1
CTSNode::CTSNode():m_pVertex(0)
{
}

CTSNode::CTSNode(GtsVertex *pVertex):m_pVertex(pVertex)
{
}

//##ModelId=3BBD944902F2
CTSNode::~CTSNode()
{
}

bool CTSNode::Empty() const
{
	return m_pVertex == 0;
}

void CTSNode::AssertValid() const
{
	assert(m_pVertex);
}

const double &CTSNode::X() const
{
	AssertValid();
	return m_pVertex->p.x;
}

const double &CTSNode::Y() const
{
	AssertValid();
	return m_pVertex->p.y;
}

const double &CTSNode::Z() const
{
	AssertValid();
	return m_pVertex->p.z;
}

void CTSNode::X(const double &dX)
{
	AssertValid();
	m_pVertex->p.x = dX;
}

void CTSNode::Y(const double &dY)
{
	AssertValid();
	m_pVertex->p.y = dY;
}

void CTSNode::Z(const double &dZ)
{
	AssertValid();
	m_pVertex->p.z = dZ;
}

GtsVertex* CTSNode::Vertex()
{
	return m_pVertex;
}

const GtsVertex* CTSNode::Vertex() const
{
	return m_pVertex;
}

CCSNode::CCSNode()
{
	m_vcCoor.clear();
}

CCSNode::CCSNode(const geo::IPoint &point, const void* /*pHorizon*/)
{
	X(point.X());
	Y(point.Y());
	Z(point.Z());
}

CCSNode::~CCSNode()
{

}

const double &CCSNode::X() const
{
	AssertValid();
	return m_vcCoor[0];
}

const double &CCSNode::Y() const
{
	AssertValid();
	return m_vcCoor[1];
}

const double &CCSNode::Z() const
{
	AssertValid();
	return m_vcCoor[2];
}

void CCSNode::X(const double &dX)
{
	if(Empty())
		m_vcCoor.resize(3);
	m_vcCoor[0] = dX;
}

void CCSNode::Y(const double &dY)
{
	if(Empty())
		m_vcCoor.resize(3);
	m_vcCoor[1] = dY;
}

void CCSNode::Z(const double &dZ)
{
	if(Empty())
		m_vcCoor.resize(3);
	m_vcCoor[2] = dZ;
}
	
void CCSNode::AssertValid() const
{
	assert(!Empty());
}

bool CCSNode::Empty() const
{
	return m_vcCoor.size() == 0;
}

