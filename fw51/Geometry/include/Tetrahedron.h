 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _TETRAHEDRON_H_
#define _TETRAHEDRON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITetrahedron.h"

#include "GeometryExports.h"

namespace geo {

class IMesh;

// simple 1st order tetrahedron
class GEOMETRY_EXPORT  CTetrahedron : public ITetrahedron
{
  std::vector<int> m_vcNode;	// Mesh node indices
  int m_nIndex;							// Element index in the mesh
  IMesh& m_mesh;					// Reference to mesh

public:
  // Point insert
  CTetrahedron(IMesh& mesh, const std::vector<const IPoint*>& points);
  CTetrahedron(IMesh& mesh, const IPoint &p0, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique = true);

  // Connectivity insert
  CTetrahedron(IMesh& mesh, const std::vector<int>& points);
  CTetrahedron(IMesh& mesh, int p0, int p1, int p2, int p3);

  ~CTetrahedron();

  // Index and indexing element set
  virtual const IElementSet* IndexingElementSet() const;
  virtual int Index() const;
  virtual int PointIndex(int nIndex) const;

  // Query of points
  virtual const geo::IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const geo::IPoint &pt);

  virtual size_t NrOfNodes() const;
  virtual const INode& Node(int nIndex) const;
  virtual void Node(int nIndex, const IPoint& point);

  // replace the node with local index nIndex by the node with mesh node index nNewGlobalMeshNodeIndex
  // this replaces the node only locally in this element, not globally in the mesh
  virtual void ReplaceNode(int nIndex, int nNewGlobalMeshNodeIndex);

  virtual size_t Order() const;

  bool operator==(const CTetrahedron& rhs);

  std::set<CPoint> Intersection(const IPlane &plane, bool bQuads = false) const;
  using ITetrahedron::Intersection;

  CPtrArray<IPoint> IntersectionPolygon(CPolygon poly, bool corners = true);

  virtual void AssertValid() const;

  virtual std::string Type() const;

  virtual const char* FaceName(int nIndex) const;

  const IMesh& Mesh() const;
  IMesh& Mesh();
};

typedef std::vector<CTetrahedron> TTetrahedron;
typedef TTetrahedron::iterator TTetrahedronIt;

}


#endif /* _TETRAHEDRON_H_ */
