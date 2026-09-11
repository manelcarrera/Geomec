 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MeshTriangle.cpp: implementation of the CMeshTriangle class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "MeshTriangle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {
CMeshTriangle::CMeshTriangle(IMesh& mesh, const IPoint& p0, const IPoint& p1, const IPoint& p2)
: m_mesh(mesh)
{
	m_vcNode[0] = mesh.RegisterNode(p0, false);
	m_vcNode[1] = mesh.RegisterNode(p1, false);
	m_vcNode[2] = mesh.RegisterNode(p2, false);
	m_nIndex = mesh.RegisterElement(*this);
}

CMeshTriangle::	CMeshTriangle(IMesh& mesh, int p0, int p1, int p2)
: m_mesh(mesh)
{
	m_vcNode[0] = p0;
	m_vcNode[1] = p1;
	m_vcNode[2] = p2;
	m_nIndex = mesh.RegisterElement(*this);
}

CMeshTriangle::~CMeshTriangle()
{
}

const IElementSet* CMeshTriangle::IndexingElementSet() const
{
	return &m_mesh;
}
	
int CMeshTriangle::Index() const
{
	return m_nIndex;
}

int CMeshTriangle::PointIndex(int nIndex) const
{
	return m_vcNode[nIndex];
}

const geo::IPoint &CMeshTriangle::Point(int nIndex) const
{
	return m_mesh.Point(m_vcNode[nIndex]);
}

void CMeshTriangle::Point(int /*nIndex*/, const geo::IPoint &/*pt*/)
{
	assert(false);
}

int CMeshTriangle::NrOfPoints() const
{
	return 3;
}

}
