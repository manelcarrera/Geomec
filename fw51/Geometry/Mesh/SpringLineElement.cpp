 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// SpringLineElement.cpp: implementation of the CSpringLineElement class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "SpringLineElement.h"
#include "IMesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo { 

CSpringLineElement::CSpringLineElement(IMesh& mesh, int nFirst, int nSecond)
: m_mesh(mesh)
{
	m_nodes[0] = nFirst;
	m_nodes[1] = nSecond;
	m_nIndex = m_mesh.RegisterElement(*this);
}

CSpringLineElement::CSpringLineElement(IMesh& mesh, const IPoint& first, const IPoint& second, bool bUnique)
: m_mesh(mesh)
{
	m_nodes[0] = mesh.RegisterNode(first, bUnique);
	m_nodes[1] = mesh.RegisterNode(second, bUnique);
	assert(m_nodes[0] >= 0);
	assert(m_nodes[1] >= 0);
	m_nIndex = m_mesh.RegisterElement(*this);
}

std::string CSpringLineElement::Type() const
{
	return "SP2RO";
}

const IElementSet* CSpringLineElement::IndexingElementSet() const
{
	return &m_mesh;
}

size_t CSpringLineElement::NrOfNodes() const
{
	return 2;
}

const INode& CSpringLineElement::Node(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return m_mesh.Node(PointIndex(nIndex));
}

void CSpringLineElement::Node(int nIndex, const IPoint& point)
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	m_mesh.Node(PointIndex(nIndex), point);
}

int CSpringLineElement::PointIndex(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < NrOfNodes());
	return m_nodes[nIndex];
}

int CSpringLineElement::Index() const
{
	return m_nIndex;
}

}
