/*
 */
 /* Copyright (c) 2012 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "Polygon.h"
#include "Tetrahedron.h"
#include "IMesh.h"

// bodyface names
static const char sL1[] = "L1";
static const char sL2[] = "L2";
static const char sL3[] = "L3";
static const char sL4[] = "L4";

namespace geo 
{

CTetrahedron::CTetrahedron(IMesh& mesh, const std::vector<const IPoint*>& points)
: m_vcNode(points.size()), m_mesh(mesh)
{
	assert(points.size() == 4);
	// Register nodes
	for(int i = 0; i < m_vcNode.size(); i++)
	{
		m_vcNode[i] = m_mesh.RegisterNode(*points[i]);
	}

	// Register element
	m_nIndex = m_mesh.RegisterElement(*this);

	assert(Contains(CPoint((Point(0).X() + Point(1).X() + Point(2).X() + Point(3).X())/4,
							(Point(0).Y() + Point(1).Y() + Point(2).Y() + Point(3).Y())/4,
							(Point(0).Z() + Point(1).Z() + Point(2).Z() + Point(3).Z())/4), true));
}

CTetrahedron::CTetrahedron(IMesh& mesh, const IPoint &p0, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique)
: m_vcNode(4), m_mesh(mesh)
{
	// Register nodes
	m_vcNode[0] = m_mesh.RegisterNode(p0, bUnique);
	m_vcNode[1] = m_mesh.RegisterNode(p1, bUnique);
	m_vcNode[2] = m_mesh.RegisterNode(p2, bUnique);
	m_vcNode[3] = m_mesh.RegisterNode(p3, bUnique);
	
	// Register element
	m_nIndex = m_mesh.RegisterElement(*this);

	assert(Contains(CPoint((Point(0).X() + Point(1).X() + Point(2).X() + Point(3).X())/4,
							(Point(0).Y() + Point(1).Y() + Point(2).Y() + Point(3).Y())/4,
							(Point(0).Z() + Point(1).Z() + Point(2).Z() + Point(3).Z())/4), true));
}

CTetrahedron::CTetrahedron(IMesh& mesh, const std::vector<int>& points)
: m_vcNode(points), m_mesh(mesh)
{
	// Register element
	m_nIndex = m_mesh.RegisterElement(*this);
	
	assert(Contains(CPoint((Point(0).X() + Point(1).X() + Point(2).X() + Point(3).X())/4,
							(Point(0).Y() + Point(1).Y() + Point(2).Y() + Point(3).Y())/4,
							(Point(0).Z() + Point(1).Z() + Point(2).Z() + Point(3).Z())/4), true));
}

CTetrahedron::CTetrahedron(IMesh& mesh, int p0, int p1, int p2, int p3)
: m_vcNode(4), m_mesh(mesh)
{
	m_vcNode[0] = p0;
	m_vcNode[1] = p1;
	m_vcNode[2] = p2;
	m_vcNode[3] = p3;
	
	// Register element
	m_nIndex = m_mesh.RegisterElement(*this);

	assert(Contains(CPoint((Point(0).X() + Point(1).X() + Point(2).X() + Point(3).X())/4,
							(Point(0).Y() + Point(1).Y() + Point(2).Y() + Point(3).Y())/4,
							(Point(0).Z() + Point(1).Z() + Point(2).Z() + Point(3).Z())/4), true));
}

CTetrahedron::~CTetrahedron()
{
}

const IElementSet* CTetrahedron::IndexingElementSet() const
{
	return &m_mesh;
}

int CTetrahedron::Index() const
{
	return m_nIndex;
}

int CTetrahedron::PointIndex(int nIndex) const
{
	return m_vcNode[nIndex];
}

const geo::IPoint &CTetrahedron::Point(int nIndex) const
{
	return m_mesh.Point(m_vcNode[nIndex]);
}

void CTetrahedron::Point(int nIndex, const geo::IPoint &pt)
{
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  assert(NrOfPoints() == NrOfNodes()); // must be first order

  Node(nIndex, pt);
}

size_t CTetrahedron::NrOfNodes() const
{
  return 4;
}

const INode& CTetrahedron::Node(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return Mesh().Node(m_vcNode[nIndex]);
}

void CTetrahedron::Node(int nIndex, const IPoint& node)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  Mesh().Node(m_vcNode[nIndex], node);
}

void CTetrahedron::ReplaceNode(int nIndex, int nNewGlobalMeshNodeIndex)
{
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  const_cast <INode&> (Node(nIndex)).UnregisterElement(*this);
  m_vcNode[nIndex] = nNewGlobalMeshNodeIndex;
  const_cast <INode&> (Node(nIndex)).RegisterElement(*this);
}

size_t CTetrahedron::Order() const
{
	return 1;
}

// Compare if 2 tetrahedron elements are the same and return true if they do.
bool CTetrahedron::operator==(const CTetrahedron& rhs)
{
	if(IndexingElementSet() != rhs.IndexingElementSet())
		return false;

	for(int i = 0; i < 4; i++)
		if(PointIndex(i) != rhs.PointIndex(i))
			return false;

	return true;
}

//=============================================================================

std::set<CPoint> CTetrahedron::Intersection(const IPlane &/*plane*/, bool /*bQuads*/ /*= false*/ ) const
{
	assert(false);
	std::set<CPoint> retset;
	return retset;
}

CPtrArray<IPoint> CTetrahedron::IntersectionPolygon(CPolygon poly, bool corners /*= true*/)
{
	CPtrArray<IPoint> result;

	for (int i=0; i<NrOfFaces(); i++)
	{
		const IFace *face = &Face(i);

		int j;
		for (j=0; j<poly.NrOfPoints(); j++)
		{
			int n = j+1;
			if (n > poly.NrOfPoints()-1) n = 0;
			CLine line(poly.Point(j), poly.Point(n));
			CPoint pt = face->Intersection(line);
			if (!pt.Empty() && face->Contains(pt, true) && line.Contains(pt, true))
				result.PushBack(*(new CPoint(pt)));
		}

		for (j=0; j<face->NrOfPoints(); j++)
		{
			int n = j+1;
			if (n > face->NrOfPoints()-1) n = 0;
			CLine line(face->Point(j), face->Point(n));
			CPoint pt = poly.Intersection(line);
			if (!pt.Empty() && poly.Contains(pt, true) && line.Contains(pt, true))
				result.PushBack(*(new CPoint(pt)));
		}
	}

	if (corners) InsertCorners(poly, result);

	return result;
}

void CTetrahedron::AssertValid() const
{
	// Call base-class
	IBody::AssertValid();
}

std::string CTetrahedron::Type() const
{
	return std::string("TE12L");
}

const IMesh& CTetrahedron::Mesh() const
{
	return m_mesh;
}

IMesh& CTetrahedron::Mesh()
{
	return m_mesh;
}

const char* CTetrahedron::FaceName(int nIndex) const
{
	const char* ret = 0;
	switch(nIndex)
	{
	case 0:
		ret = sL1;
		break;
	case 1:
		ret = sL2;
		break;
	case 2:
		ret = sL3;
		break;
	case 3:
		ret = sL4;
		break;
	default:
		assert(false);
	}

	return ret;
}

}
