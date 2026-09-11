/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMesh.h: interface for the CTriangleMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLEMESH_H__FA62D08B_FB35_4EB9_9006_BF2D42368C78__INCLUDED_)
#define AFX_TRIANGLEMESH_H__FA62D08B_FB35_4EB9_9006_BF2D42368C78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISurface.h"
#include "Region.h"

#include "TriangleExports.h"

class CTriangleNode;
class CTriangleElement;
class CTriangleEdge;

struct triangulateio;

class TRIANGLE_EXPORT CTriangleSurface : public geo::ISurface {
  // allow insertion of extra points on boundary
  // default = true
  bool m_bAllowSteinerPointsOnBoundary;

  // the Triangle library's data structure
  friend class CTriangleEdge;
  // the input
  triangulateio *m_input;

  // the actual mesh
  triangulateio *m_mesh;

  // clear out a specific mesh
  void ClearTriangleStruct(triangulateio *mesh);

  // clear out m_triangle_mesh
  void ClearTriangleMesh();

  // Node interface
  mutable std::vector<CTriangleNode *> m_vcNode;

  // Edge interface
  mutable std::vector<CTriangleEdge *> m_vcEdge;

  // Element interface
  mutable std::vector<CTriangleElement *> m_vcElement;

  // regions container
  std::vector<CRegion *> m_vcRegions;

  // sets up the node interface (fills m_vcNode)
  void BuildNodeInterface() const;

  // sets up the element interface (fills m_vcElement)
  void BuildElementInterface() const;

  // sets up the edge interface (fills m_vcEdge)
  void BuildEdgeInterface() const;

  // internal point in the given region, to identify the region for Triangle
  geo::CPoint RegionInternalPoint(const CRegion &region) const;

  // end point indices (length is 2) of segment with index n (direct access in m_triangle_mesh)
  const int *SegmentPtr(int n) const;

  // point coordinates (length is 2) of point with index n (direct access in m_triangle_mesh)
  const double *PointCoord(int n) const;

protected:
  // clears the node and element interfaces
  virtual void InvalidateCache();

  // non-const point access
  virtual geo::IPoint &PointAt(int nIndex);

public:
  // Construction
  CTriangleSurface();
  CTriangleSurface(const geo::ISurface &rhs);
  CTriangleSurface(const std::vector<geo::CPoint> &vcPoint, const std::vector<std::vector<int>> &vcTriangle);
  CTriangleSurface(const std::vector<const geo::IPoint *> &vcPoint, const std::vector<std::vector<int>> &vcTriangle);

  // Destruction
  virtual ~CTriangleSurface();

  // Set surface (call clear first)
  void SetSurface(const geo::ISurface &rhs);
  void SetSurface(const std::vector<geo::CPoint> &vcPoint, const std::vector<std::vector<int>> &vcTriangle);
  void SetSurface(const std::vector<const geo::IPoint *> &vcPoint, const std::vector<std::vector<int>> &vcTriangle);

  // point vector types
  typedef std::vector<geo::IPoint *> TPointPtrVec;
  typedef std::vector<geo::CPoint> TPointVec;

  // segments
  typedef std::pair<size_t, size_t> TSegment;
  typedef std::vector<TSegment> TSegmentVec;

  // set the points
  void Points(const TPointPtrVec &vcPoint);
  void Points(const TPointVec &vcPoint);

  // set the segments
  void Segments(const TSegmentVec &vcSegment);

  // set the edges (for loading purposes)
  typedef std::pair<int, int> TEdgePoint;   // Two point indices of the edge
  typedef std::pair<TEdgePoint, int> TEdge; // Point indices and the corresponding segment index
  typedef std::vector<TEdge> TEdgeVec;
  void Edges(const TEdgeVec &vcEdge);

  // request a new region
  // immediately setting the segments and the maximum area
  CRegion &NewRegion(const CRegion::TSegments &segments, const double &maxarea);

  // only setting the segments, no area constraint (or set it later)
  CRegion &NewRegion(const CRegion::TSegments &segments);

  // empty region, set the segments later, no area constraint (or set it later)
  CRegion &NewRegion();

  // is it a valid mesh?
  bool IsMesh() const;

  // (re)generate the mesh, call Clear() to generate from scratch
  bool CreateMesh(double minangle = 20, double maxarea = 0);

  // region access
  int RegionSize() const;
  const CRegion &Region(int nIndex) const;
  CRegion &Region(int nIndex);

  // clear the mesh
  virtual void Clear();

  virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint &p) const;
  virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint &p) const;

  // face (triangle) access
  virtual int FaceSize() const;
  virtual const geo::IFace &Face(int nIndex) const;
  virtual const geo::ITriangle &Triangle(int nIndex) const;

  // edge access
  int EdgeSize() const;
  const CTriangleEdge &Edge(int nIndex) const;
  using geo::ISurface::Edge;

  // segment access
  int SegmentSize() const;
  TSegment Segment(size_t nIndex) const;

  // point access
  virtual int PointSize() const;
  virtual const geo::IPoint &Point(int nIndex) const;

  // the nodes of the given element, must be an element from this mesh
  virtual std::vector<int> Nodes(const geo::IElement &element) const;

  // allow insertion of extra points on boundary
  // default = true
  void AllowSteinerPointsOnBoundary(bool);

  // For debugging purposes
  int InternalInputPointSize() const;
  const double *InternalInputPoint(int index) const;
};

#endif // !defined(AFX_TRIANGLEMESH_H__FA62D08B_FB35_4EB9_9006_BF2D42368C78__INCLUDED_)
