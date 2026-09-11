 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// MeshNodeGroup.cpp: implementation of the CMeshNodeGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "IMesh.h"
#include "INode.h"
#include "MeshNodeGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CMeshNodeGroup::CMeshNodeGroup(IMesh& mesh)
: m_mesh(mesh), m_nIndex(m_mesh.NodeGroupSize())
{
	m_mesh.AddNodeGroup(*this);
}

CMeshNodeGroup::~CMeshNodeGroup()
{
	m_mesh.RemoveNodeGroup(*this);
}

int CMeshNodeGroup::Index() const
{
	return m_nIndex;
}

bool CMeshNodeGroup::AddMeshNode(const INode& node)
{
	assert(node.IndexingElementSet() == &m_mesh);

	m_vcNode.push_back(&node);

	return true;
}

int CMeshNodeGroup::NodeSize() const
{
	return (int)m_vcNode.size();
}

const INode& CMeshNodeGroup::Node(int nIndex) const
{
	assert(nIndex>=0);
	assert(nIndex<m_vcNode.size());
	return *m_vcNode[nIndex];
}

//Overrides of IObject
void CMeshNodeGroup::Rotate(const IVector &/*vec*/, const double &/*dAngleDeg*/)
{
	assert(false);	//Nodes are const
}

void CMeshNodeGroup::Move(const IVector &/*vec*/)
{
	assert(false);	//Nodes are const
}

void CMeshNodeGroup::Transform(const IMatrix &/*matrix*/)
{
	assert(false);	//Nodes are const
}

void CMeshNodeGroup::AssertValid() const
{
}

bool CMeshNodeGroup::Empty() const
{
	return (NodeSize()==0);
}

CPoint CMeshNodeGroup::Min() const
{
	AssertValid();

	int nodes = NodeSize();
	if(nodes == 0)
		return CPoint();

	CPoint ptMin(Node(0));

	for(int i = 1; i < nodes; i++)
	{
		ptMin = ptMin.Min(Node(i));
	}

	ptMin.AssertValid();

	return ptMin;
}

CPoint CMeshNodeGroup::Max() const
{
	AssertValid();

	int nodes = NodeSize();
	if(nodes == 0)
		return CPoint();

	CPoint ptMax(Node(0));

	for(int i = 1; i < nodes; i++)
	{
		ptMax = ptMax.Max(Node(i));
	}

	ptMax.AssertValid();

	return ptMax;
}

} // namespace geo
