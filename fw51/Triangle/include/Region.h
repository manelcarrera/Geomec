 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef _REGION_H_
#define _REGION_H_

class CTriangleSurface;
class CTriangleElement;

#include "ITriangle.h"
#include "IElementSet.h"

#include <vector>

#include "TriangleExports.h"

class CTriangleSurface;

class TRIANGLE_EXPORT CRegion : public geo::IElementSet
{
public:
	// indices of segments in CTriangleSurface::TSegmentVec
	typedef std::vector<int> TSegments;

private:
	// the only one that can create CRegion objects
	friend class CTriangleSurface;

	// construction
	CRegion(const CTriangleSurface &surface);
	CRegion(const CTriangleSurface &surface, const TSegments &segments);
	CRegion(const CTriangleSurface &surface, const TSegments &segments, const double &maxarea);

	// destruction
	virtual ~CRegion();

	// the surface this region belongs to
	const CTriangleSurface &m_surface;

	// segments in surface that compose this region
	TSegments m_segments;

	// maximum area for the triangles in this region
	double m_maxarea;

	// tells whether there is an area constraint
	bool m_bAreaConstraint;

	// indices of the triangles in the surface that this region contains after meshing
	std::vector<int> m_vcTriangleIndices;

	// indices of the (surface's) nodes (from the triangles) that this region contains
	std::set<int> m_stNodes;

	// cache of the node set for indexing purposes
	mutable std::vector<int> m_vcNodes;

	// builds the above vector of nodes
	void BuildNodeCache() const;

	// add triangles from the mesher
	void AddTriangle(int nIndex);

	// clear the triangles contained in the region from a cache invalidation
	void Clear();

protected:
	virtual geo::IPoint &PointAt(int nIndex);

public:
	// surface
	const CTriangleSurface &Surface() const;

	// set triangles references for loading purposes
	void Triangles(const std::vector<int> &vcTriangleIndices);

	// triangle access
  size_t TriangleSize() const;
  const geo::ITriangle &Triangle(size_t nIndex) const;

	// set the segments
	void Segments(const TSegments &segments);

	// set the maximum area
	void MaximumArea(const double &maxarea);

	// get the maximum area
	const double &MaximumArea() const;

	// tells whether the area is constraint
	bool AreaConstraint() const;

	// release the area constraint (reintroduce by setting one using MaximumArea)
	void ReleaseAreaConstraint();

	// segments access
  size_t SegmentSize() const;
  int Segment(size_t nIndex) const;

	// IElementSet interface
	virtual void AssertValid() const;
	virtual const geo::IPoint &Point(int nIndex) const;
	virtual int PointSize() const;
	virtual const geo::IElement &Element(int nIndex) const;
	virtual int ElementSize() const;
	virtual std::vector<int> Nodes(const geo::IElement &element) const;

	int PointIndex(int nIndex) const;
};

#endif // _REGION_H_
