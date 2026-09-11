// Spring.cpp: implementation of the CSpringElement class.
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "SpringElement.h"
#include "IMesh.h"


CSpringElement::CSpringElement(geo::IMesh &mesh, int nNodeIndex): m_mesh(mesh), m_nNodeIndex(nNodeIndex)
{
  mesh.RegisterElement(*this);
}

CSpringElement::~CSpringElement()
{

}

const geo::INode &CSpringElement::Node(int nIndex) const
{
  assert(!nIndex); // it can only be 0
  return m_mesh.Node(PointIndex(nIndex));
}

size_t CSpringElement::NrOfNodes() const
{
  return 1;
}

std::string CSpringElement::Type() const
{
  return std::string("SP1TR");	
}

const geo::IElementSet* CSpringElement::IndexingElementSet() const
{
  return &m_mesh;
}

int CSpringElement::PointIndex(int nIndex) const
{
  assert(!nIndex);
  return m_nNodeIndex;
}

//const geo::IPoint &CSpringElement::Point(int nIndex) const
//{
//	return Node(0); 
//}
//
//void CSpringElement::Point(int nIndex,const geo::IPoint &pt)
//{
//	assert(false);
//}
//
