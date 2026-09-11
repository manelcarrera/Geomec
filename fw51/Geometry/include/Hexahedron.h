 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _HEXAHEDRON_H_
#define _HEXAHEDRON_H_

#include "IHexahedron.h"

#include "GeometryExports.h"

namespace geo {

#define HEXA_UPPERFACE	0
#define HEXA_FRONTFACE	1
#define HEXA_RIGHTFACE	2
#define HEXA_BACKFACE	3
#define HEXA_LEFTFACE	4
#define HEXA_LOWERFACE	5

class IMesh;

// Simple 1st order hexahedron
class GEOMETRY_EXPORT  CHexahedron: public IHexahedron
{	
  std::vector<int> m_vcNode;	// Mesh node indices
  int m_nIndex;				// Element index in the mesh
  IMesh& m_mesh;				// Reference to mesh

public:
  CHexahedron(IMesh& mesh, const std::vector<const IPoint*>& points);
  CHexahedron(IMesh& mesh, const std::vector<int>& points);
  ~CHexahedron();

  CHexahedron& operator=(const CHexahedron& rhs);
  bool operator==(const CHexahedron& rhs);

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

  virtual size_t Order() const;
  virtual IElement::TDoubleVec WorldToIso(const geo::IPoint& point) const;

  using IHexahedron::Intersection;
  virtual std::set<geo::CPoint> Intersection(const geo::IPlane &plane) const; // Only implemented because of pure virtual function, don't use!!
  //std::set<geo::CPoint> Intersection2(const geo::IPlane &plane, bool bQuads = false) const; // use this one!

  CPtrArray<IPoint> IntersectionPolygon(const CPolygon poly, bool corners /*= true*/);

  virtual void AssertValid() const;

  virtual std::string Type() const;

  double InfluenceVolume(int nNode) const;
  virtual double InfluenceSize(int nNode) const { return InfluenceVolume(nNode); }

  bool Contains(const geo::IPoint &point, bool bIncludeEdge) const;

  virtual const char* FaceName(int nIndex) const;

  const IMesh& Mesh() const;
  IMesh& Mesh();
};	

}

#endif /* _HEXAHEDRON_H_ */
