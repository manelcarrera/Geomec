// BoundaryInterfaceElement.cpp: implementation of the CBoundaryInterfaceElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "BoundaryInterfaceElement.h"
#include "tetraboundary.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBoundaryInterfaceElement::CBoundaryInterfaceElement(geo::IMesh &mesh, const geo::IFace &face, CTetraBoundary &bound)
:m_Mesh(mesh), m_ParentFace(face)
{
	assert(face.NrOfPoints() == 3);
	int nNode1 = face.PointIndex(0);
	int nNode2 = face.PointIndex(1);
	int nNode3 = face.PointIndex(2);

	int nNode4, nNode5, nNode6;
	geo::CPoint pt4(mesh.Node(nNode1));
	geo::CPoint pt5(mesh.Node(nNode2));
	geo::CPoint pt6(mesh.Node(nNode3));

	std::vector<int> indices;
	std::set<geo::CPoint> &pool = bound.InterfaceNodes();
	int i, j;

	std::vector<geo::CPoint> vcPts;
	vcPts.push_back(pt4);
	vcPts.push_back(pt5);
	vcPts.push_back(pt6);
	std::vector<int> new_indices;
	new_indices.push_back(nNode4);
	new_indices.push_back(nNode5);
	new_indices.push_back(nNode6);
	std::vector<int> face_indices;
	face_indices.push_back(nNode1);
	face_indices.push_back(nNode2);
	face_indices.push_back(nNode3);

	for(i = 0; i < vcPts.size(); i++)
	{
		if(pool.insert(vcPts[i]).second)
		{
			new_indices[i] = mesh.RegisterNode(vcPts[i], false);
			bound.InterfaceNodesIndices().push_back(new_indices[i]);
		}
		else
		{
			 indices = mesh.NodeIndex(vcPts[i]);
			 
			 for(j = 0; j < indices.size(); j++)
			 {
				 if(face_indices[i] != indices[j])
				 	 new_indices[i] = indices[j];
			 }
		}
	}

	m_vcNodes.push_back(face_indices[0]);
	m_vcNodes.push_back(face_indices[1]);
	m_vcNodes.push_back(face_indices[2]);
	m_vcNodes.push_back(new_indices[0]);
	m_vcNodes.push_back(new_indices[1]);
	m_vcNodes.push_back(new_indices[2]);

	m_nIndex = m_Mesh.RegisterElement(*this);

	m_vcLines.push_back(geo::CLine(Node(0), Node(1)));
	m_vcLines.push_back(geo::CLine(Node(1), Node(2)));
	m_vcLines.push_back(geo::CLine(Node(2), Node(0)));
}

CBoundaryInterfaceElement::CBoundaryInterfaceElement(geo::IMesh &mesh,
													 const geo::IFace &face,
													 int nIdx1, 
													 int nIdx2, 
													 int nIdx3, 
													 int nIdx4,
													 int nIdx5, 
													 int nIdx6)
:m_Mesh(mesh), m_ParentFace(face)
{
	m_vcNodes.push_back(nIdx1);
	m_vcNodes.push_back(nIdx2);
	m_vcNodes.push_back(nIdx3);
	m_vcNodes.push_back(nIdx4);
	m_vcNodes.push_back(nIdx5);
	m_vcNodes.push_back(nIdx6);

	m_nIndex = m_Mesh.RegisterElement(*this);
	m_vcLines.push_back(geo::CLine(Node(0), Node(1)));
	m_vcLines.push_back(geo::CLine(Node(1), Node(2)));
	m_vcLines.push_back(geo::CLine(Node(2), Node(0)));
}

/*virtual*/ CBoundaryInterfaceElement::~CBoundaryInterfaceElement()
{
	// Destruct
}

/*virtual*/ int CBoundaryInterfaceElement::Order() const
{
	return 1;
}

/*virtual*/ int CBoundaryInterfaceElement::NrOfLines() const
{
	return 3;
}

/*virtual*/ const geo::ILine& CBoundaryInterfaceElement::Line(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < 3);
	return m_vcLines[nIndex];
}

/*virtual*/ double CBoundaryInterfaceElement::RepresentativeLength() const
{
	return 0; 
}

/*virtual*/ const geo::IElementSet* CBoundaryInterfaceElement::IndexingElementSet() const
{
	return &m_Mesh;
}

/*virtual*/ int CBoundaryInterfaceElement::Index() const
{
	return m_nIndex;
}

/*virtual*/ int CBoundaryInterfaceElement::PointIndex(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < 6);
	return m_vcNodes[nIndex];
}

/*virtual*/ int CBoundaryInterfaceElement::NrOfNodes() const
{
	return 6;
}

/*virtual*/ const geo::INode& CBoundaryInterfaceElement::Node(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < 6);
	return m_Mesh.Node(m_vcNodes[nIndex]);
}

/*virtual*/ void CBoundaryInterfaceElement::Node(int nIndex,const geo::IPoint& point)
{
	assert(false);
}

/*virtual*/ const geo::IPoint &CBoundaryInterfaceElement::Point(int nIndex) const
{
	return Node(nIndex);
}

/*virtual*/ void CBoundaryInterfaceElement::Point(int nIndex,const geo::IPoint &pt)
{
	assert(false); 
}

/*virtual*/ int CBoundaryInterfaceElement::NrOfPoints() const
{
	return 6;
}

/*virtual*/ std::string CBoundaryInterfaceElement::Type() const
{
	return std::string("T18IF"); 
}
/*virtual*/ bool CBoundaryInterfaceElement::Contains(const geo::IPoint &point,bool bIncludeEdge) const
{
	return m_ParentFace.Contains(point, bIncludeEdge);
}
