/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleMesh.cpp: implementation of the CTriangleSurface class.
//
//////////////////////////////////////////////////////////////////////
typedef double REAL;

#include "TriangleSurface.h"
#include "Polygon.h"
#include "TriangleEdge.h"
#include "TriangleElement.h"
#include "TriangleNode.h"
#include "dimple.h"
#include "tri_triangle.h"

#include <cmath>
#include <float.h>

#define CMDSIZ 128

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriangleSurface::CTriangleSurface() : m_bAllowSteinerPointsOnBoundary(true) {
  m_input = new triangulateio;
  memset(m_input, 0, sizeof(triangulateio));
  m_mesh = new triangulateio;
  memset(m_mesh, 0, sizeof(triangulateio));
}

CTriangleSurface::CTriangleSurface(const geo::ISurface &rhs) : m_bAllowSteinerPointsOnBoundary(true) {
  m_input = new triangulateio;
  memset(m_input, 0, sizeof(triangulateio));
  m_mesh = new triangulateio;
  memset(m_mesh, 0, sizeof(triangulateio));
  SetSurface(rhs);
}

CTriangleSurface::CTriangleSurface(const std::vector<geo::CPoint> &vcPoint,
                                   const std::vector<std::vector<int>> &vcTriangle)
    : m_bAllowSteinerPointsOnBoundary(true) {
  m_input = new triangulateio;
  memset(m_input, 0, sizeof(triangulateio));
  m_mesh = new triangulateio;
  memset(m_mesh, 0, sizeof(triangulateio));
  SetSurface(vcPoint, vcTriangle);
}

CTriangleSurface::CTriangleSurface(const std::vector<const geo::IPoint *> &vcPoint,
                                   const std::vector<std::vector<int>> &vcTriangle)
    : m_bAllowSteinerPointsOnBoundary(true) {
  m_input = new triangulateio;
  memset(m_input, 0, sizeof(triangulateio));
  m_mesh = new triangulateio;
  memset(m_mesh, 0, sizeof(triangulateio));
  SetSurface(vcPoint, vcTriangle);
}

CTriangleSurface::~CTriangleSurface() {
  Clear();

  ClearTriangleStruct(m_input);
  delete m_input;

  ClearTriangleStruct(m_mesh);
  delete m_mesh;
}

void CTriangleSurface::SetSurface(const geo::ISurface &rhs) {
  assert(PointSize() == 0);
  assert(ElementSize() == 0);
  m_mesh->numberofpoints = rhs.PointSize();
  m_mesh->pointlist = (REAL *)malloc(2 * rhs.PointSize() * sizeof(REAL));

  int i;
  for (i = 0; i < rhs.PointSize(); i++) {
    m_mesh->pointlist[2 * i] = rhs.Point(i).X();
    m_mesh->pointlist[2 * i + 1] = rhs.Point(i).Y();
  }

  m_mesh->numberoftriangles = rhs.FaceSize();
  m_mesh->trianglelist = (int *)malloc(3 * rhs.FaceSize() * sizeof(REAL));

  for (i = 0; i < rhs.FaceSize(); i++) {
    const geo::IFace &face = rhs.Face(i);
    assert(face.NrOfPoints() == 3);

    int j;
    for (j = 0; j < 3; j++)
      m_mesh->trianglelist[3 * i + j] = face.PointIndex(j);
  }
}

void CTriangleSurface::SetSurface(const std::vector<geo::CPoint> &vcPoint,
                                  const std::vector<std::vector<int>> &vcTriangle) {
  assert(PointSize() == 0);
  assert(ElementSize() == 0);
  m_mesh->numberofpoints = (int)vcPoint.size();
  m_mesh->pointlist = (REAL *)malloc(2 * vcPoint.size() * sizeof(REAL));

  for (size_t i = 0; i < vcPoint.size(); i++) {
    m_mesh->pointlist[2 * i] = vcPoint[i].X();
    m_mesh->pointlist[2 * i + 1] = vcPoint[i].Y();
  }

  m_mesh->numberoftriangles = (int)vcTriangle.size();
  m_mesh->trianglelist = (int *)malloc(3 * vcTriangle.size() * sizeof(int));

  for (size_t i = 0; i < vcTriangle.size(); i++) {
    const std::vector<int> &tri = vcTriangle[i];
    assert(tri.size() == 3);

    for (size_t j = 0; j < tri.size(); j++)
      m_mesh->trianglelist[3 * i + j] = tri[j];
  }
}

void CTriangleSurface::SetSurface(const std::vector<const geo::IPoint *> &vcPoint,
                                  const std::vector<std::vector<int>> &vcTriangle) {
  assert(PointSize() == 0);
  assert(ElementSize() == 0);
  m_mesh->numberofpoints = (int)vcPoint.size();
  m_mesh->pointlist = (REAL *)malloc(2 * vcPoint.size() * sizeof(REAL));

  for (size_t i = 0; i < vcPoint.size(); i++) {
    m_mesh->pointlist[2 * i] = vcPoint[i]->X();
    m_mesh->pointlist[2 * i + 1] = vcPoint[i]->Y();
  }

  m_mesh->numberoftriangles = (int)vcTriangle.size();
  m_mesh->trianglelist = (int *)malloc(3 * vcTriangle.size() * sizeof(int));

  for (size_t i = 0; i < vcTriangle.size(); i++) {
    const std::vector<int> &tri = vcTriangle[i];
    assert(tri.size() == 3);

    for (size_t j = 0; j < tri.size(); j++)
      m_mesh->trianglelist[3 * i + j] = tri[j];
  }
}

void CTriangleSurface::ClearTriangleStruct(triangulateio *mesh) {
  if (mesh->pointlist)
    free(mesh->pointlist);
  if (mesh->trianglelist)
    free(mesh->trianglelist);
  if (mesh->pointmarkerlist)
    free(mesh->pointmarkerlist);
  if (mesh->triangleattributelist)
    free(mesh->triangleattributelist);
  if (mesh->neighborlist)
    free(mesh->neighborlist);
  if (mesh->segmentlist)
    free(mesh->segmentlist);
  if (mesh->segmentmarkerlist)
    free(mesh->segmentmarkerlist);
  if (mesh->regionlist)
    free(mesh->regionlist);
  if (mesh->edgelist)
    free(mesh->edgelist);
  if (mesh->edgemarkerlist)
    free(mesh->edgemarkerlist);

  // TODO: delete any other struct members

  // set everything to zero
  memset(mesh, 0, sizeof(triangulateio));
}

void CTriangleSurface::ClearTriangleMesh() {
  ClearTriangleStruct(m_mesh);
  ClearTriangleStruct(m_input);
  for (size_t i = 0; i < m_vcRegions.size(); i++)
    delete m_vcRegions[i];
  m_vcRegions.clear();
}

void CTriangleSurface::InvalidateCache() {
  for (size_t i = 0; i < m_vcNode.size(); i++)
    delete m_vcNode[i];
  m_vcNode.clear();

  for (size_t i = 0; i < m_vcElement.size(); i++)
    delete m_vcElement[i];
  m_vcElement.clear();

  for (size_t i = 0; i < m_vcEdge.size(); i++)
    delete m_vcEdge[i];
  m_vcEdge.clear();

  for (int i = 0; i < RegionSize(); i++)
    Region(i).Clear();

  ISurface::InvalidateCache();
}

void CTriangleSurface::BuildNodeInterface() const {
  assert(m_vcNode.size() == 0);
  m_vcNode.resize(m_mesh->numberofpoints);
  for (int i = 0; i < m_mesh->numberofpoints; i++)
    m_vcNode[i] = new CTriangleNode(const_cast<CTriangleSurface &>(*this), i, &m_mesh->pointlist[2 * i]);
}

const geo::IPoint &CTriangleSurface::Point(int nIndex) const {
  assert(nIndex >= 0 && nIndex < PointSize());
  if (m_vcNode.empty())
    BuildNodeInterface();
  return *m_vcNode[nIndex];
}

int CTriangleSurface::PointSize() const { return m_mesh->numberofpoints; }

void CTriangleSurface::BuildElementInterface() const {
  assert(m_vcElement.size() == 0);
  m_vcElement.resize(m_mesh->numberoftriangles);
  for (int i = 0; i < m_mesh->numberoftriangles; i++)
    m_vcElement[i] = new CTriangleElement(*this, i, &m_mesh->trianglelist[3 * i]);
}

void CTriangleSurface::BuildEdgeInterface() const {
  assert(m_vcEdge.size() == 0);
  m_vcEdge.resize(m_mesh->numberofedges);
  for (int i = 0; i < m_mesh->numberofedges; i++)
    m_vcEdge[i] = new CTriangleEdge(*this, i);
}

void CTriangleSurface::Points(const TPointPtrVec &vcPoint) {
  assert(m_input->numberofpoints == 0); // must clear first

  m_input->numberofpoints = (int)vcPoint.size();
  m_input->pointlist = (REAL *)malloc(2 * vcPoint.size() * sizeof(REAL));

  for (size_t i = 0; i < vcPoint.size(); i++) {
    m_input->pointlist[2 * i] = vcPoint[i]->X();
    m_input->pointlist[2 * i + 1] = vcPoint[i]->Y();
  }
}

void CTriangleSurface::Points(const TPointVec &vcPoint) {
  assert(m_input->numberofpoints == 0); // must clear first

  m_input->numberofpoints = (int)vcPoint.size();
  m_input->pointlist = (REAL *)malloc(2 * vcPoint.size() * sizeof(REAL));

  for (size_t i = 0; i < vcPoint.size(); i++) {
    m_input->pointlist[2 * i] = vcPoint[i].X();
    m_input->pointlist[2 * i + 1] = vcPoint[i].Y();
  }
}

void CTriangleSurface::Segments(const TSegmentVec &vcSegment) {
  assert(m_input->numberofsegments == 0); // must clear first

  m_input->numberofsegments = (int)vcSegment.size();
  m_input->segmentlist = (int *)malloc(2 * vcSegment.size() * sizeof(int));

  for (size_t i = 0; i < vcSegment.size(); i++) {
    m_input->segmentlist[2 * i] = vcSegment[i].first;
    m_input->segmentlist[2 * i + 1] = vcSegment[i].second;
  }
}

CRegion &CTriangleSurface::NewRegion(const CRegion::TSegments &segments, const double &maxarea) {
  CRegion *pRegion = new CRegion(*this, segments, maxarea);
  m_vcRegions.push_back(pRegion);

  return *pRegion;
}

CRegion &CTriangleSurface::NewRegion(const CRegion::TSegments &segments) {
  CRegion *pRegion = new CRegion(*this, segments);
  m_vcRegions.push_back(pRegion);

  return *pRegion;
}

CRegion &CTriangleSurface::NewRegion() {
  CRegion *pRegion = new CRegion(*this);
  m_vcRegions.push_back(pRegion);

  return *pRegion;
}

bool CTriangleSurface::IsMesh() const { return (FaceSize() > 0); }

bool CTriangleSurface::CreateMesh(double minangle /* = 20 */, double max_area /* = 0 */) {
  assert(m_input);
  assert(m_input->numberofpoints > 0);
  assert(m_input->pointlist);
  assert(m_input->numberofsegments > 0);
  assert(m_input->segmentlist);

  InvalidateCache();

  char cmdline[CMDSIZ];
  if (max_area > 0)
    sprintf(cmdline, /*CMDSIZ,*/ "Qpzq%fena%f", minangle, max_area);
  else
    sprintf(cmdline, /*CMDSIZ,*/ "Qpzq%fen", minangle);
  // _snprintf() was used here in windows so that the line size was
  // limited to CMDSIZE and the buffer could not overwritten.

  // set segment markers
  if (m_input->segmentmarkerlist)
    free(m_input->segmentmarkerlist);
  m_input->segmentmarkerlist = 0;

  m_input->segmentmarkerlist = (int *)malloc(m_input->numberofsegments * sizeof(int));

  for (int i = 0; i < m_input->numberofsegments; i++)
    m_input->segmentmarkerlist[i] = i + 1;

  // set regions
  if (m_input->regionlist)
    free(m_input->regionlist);
  m_input->regionlist = 0;

  m_input->numberofregions = RegionSize();
  if (RegionSize()) {
    m_input->regionlist = (REAL *)malloc(4 * RegionSize() * sizeof(REAL));

    assert(strlen(cmdline) < CMDSIZ - 3);
    if (max_area > 0)
      strcat(cmdline, "A");
    else
      strcat(cmdline, "aA");
  }

  for (int i = 0; i < RegionSize(); i++) {
    // set the region's attributes for the mesher
    // - the index of the region, so we can recognise the triangles when they come back
    // - the area constraint
    // the region is identified by a point inside it, this is solved by the RegionInternalPoint function

    const CRegion &region = Region(i);

    assert(region.SegmentSize() > 2); // must add a closed set of segments to regions

    geo::CPoint ptInternal = RegionInternalPoint(region);
    assert(!ptInternal.Empty());

    assert(i < m_input->numberofregions);
    assert(m_input->regionlist);

    double maxarea;
    if (region.AreaConstraint())
      maxarea = region.MaximumArea();
    else
      maxarea = DBL_MAX;

    assert(maxarea > 0);

    m_input->regionlist[4 * i] = ptInternal.X();
    m_input->regionlist[4 * i + 1] = ptInternal.Y();
    m_input->regionlist[4 * i + 2] = i;
    m_input->regionlist[4 * i + 3] = maxarea;
  }

  if (!m_bAllowSteinerPointsOnBoundary)
    strcat(cmdline, "YY");
  ClearTriangleStruct(m_mesh);

  triangulate(cmdline, m_input, m_mesh, 0);
  m_mesh->regionlist = 0;
  m_mesh->numberofregions = 0;

  if (RegionSize() > 0) {
    // look at the triangle attributes and put them in the right region
    assert(m_mesh->numberoftriangleattributes == 1);
    for (int i = 0; i < m_mesh->numberoftriangles; i++) {
      int iAttrib = int(m_mesh->triangleattributelist[i]);
      assert(iAttrib < RegionSize());

      Region(iAttrib).AddTriangle(i);
    }
  }

  return true;
}

void CTriangleSurface::Clear() {
  InvalidateCache();
  ClearTriangleMesh();
}

int CTriangleSurface::FaceSize() const { return m_mesh->numberoftriangles; }

const geo::IFace &CTriangleSurface::Face(int nIndex) const {
  assert(nIndex >= 0 && nIndex < FaceSize());
  if (m_vcElement.empty())
    BuildElementInterface();
  return *m_vcElement[nIndex];
}

const geo::ITriangle &CTriangleSurface::Triangle(int nIndex) const {
  assert(nIndex >= 0 && nIndex < FaceSize());
  if (m_vcElement.empty())
    BuildElementInterface();
  return *m_vcElement[nIndex];
}

int CTriangleSurface::EdgeSize() const { return m_mesh->numberofedges; }

void CTriangleSurface::Edges(const TEdgeVec &vcEdge) {
  m_mesh->numberofedges = (int)vcEdge.size();
  m_mesh->edgemarkerlist = (int *)malloc(vcEdge.size() * sizeof(int));
  m_mesh->edgelist = (int *)malloc(vcEdge.size() * 2 * sizeof(int));
  for (size_t i = 0; i < vcEdge.size(); i++) {
    m_mesh->edgemarkerlist[i] = vcEdge[i].second + 1;     // Segment index
    m_mesh->edgelist[2 * i] = vcEdge[i].first.first;      // First point index
    m_mesh->edgelist[2 * i + 1] = vcEdge[i].first.second; // Second point index
  }
}

const CTriangleEdge &CTriangleSurface::Edge(int nIndex) const {
  assert(nIndex >= 0 && nIndex < EdgeSize());
  if (m_vcEdge.empty())
    BuildEdgeInterface();
  return *m_vcEdge[nIndex];
}

const geo::CPtrArray<geo::IFace> CTriangleSurface::FacesAt(const geo::IPoint & /*p*/) const {
  assert(false);
  return geo::CPtrArray<geo::IFace>();
}

const geo::CPtrArray<geo::IFace> CTriangleSurface::FacesAtNode(const geo::IPoint & /*p*/) const {
  assert(false);
  return geo::CPtrArray<geo::IFace>();
}

std::vector<int> CTriangleSurface::Nodes(const geo::IElement &element) const {
  const CTriangleElement *pTriangle = dynamic_cast<const CTriangleElement *>(&element);
  assert(pTriangle);

  std::vector<int> ret(3);
  int i;
  for (i = 0; i < 3; i++)
    ret[i] = pTriangle->PointIndex(i);

  return ret;
}

geo::IPoint &CTriangleSurface::PointAt(int nIndex) {
  assert(nIndex >= 0 && nIndex < PointSize());
  if (m_vcNode.empty())
    BuildNodeInterface();
  return *m_vcNode[nIndex];
}

geo::CPoint CTriangleSurface::RegionInternalPoint(const CRegion &region) const {
  // map point indices to the connected segments
  typedef std::map<int, std::vector<int>> TPointToSegmentMap;
  TPointToSegmentMap mpSegments;

  for (int i = 0; i < region.SegmentSize(); i++) {
    int iSeg = region.Segment(i);
    assert(iSeg < m_input->numberofsegments);

    int iFirst = SegmentPtr(iSeg)[0];
    int iSecond = SegmentPtr(iSeg)[1];

    std::pair<TPointToSegmentMap::iterator, bool> prInsert;

    // first point
    prInsert = mpSegments.insert(std::make_pair(iFirst, std::vector<int>()));
    assert(prInsert.first->second.size() < 2); // more than 2 segments in a point? looks like a fork!
    prInsert.first->second.push_back(i);

    // second point
    prInsert = mpSegments.insert(std::make_pair(iSecond, std::vector<int>()));
    assert(prInsert.first->second.size() < 2); // more than 2 segments in a point? looks like a fork!
    prInsert.first->second.push_back(i);
  }

#ifdef _DEBUG
  assert(mpSegments.size() == region.SegmentSize());

  TPointToSegmentMap::iterator itDbg;
  for (itDbg = mpSegments.begin(); itDbg != mpSegments.end(); itDbg++) {
    assert(itDbg->first >= 0 && itDbg->first < m_input->numberofpoints);
    assert(itDbg->second.size() == 2);
    assert(itDbg->second[0] >= 0 && itDbg->second[0] < m_input->numberofsegments);
    assert(itDbg->second[1] >= 0 && itDbg->second[1] < m_input->numberofsegments);
    assert(itDbg->second[0] < region.SegmentSize());
    assert(itDbg->second[1] < region.SegmentSize());
  }
#endif

  std::vector<int> vcPointsInOrder;
  std::set<int> stSegmentsDone;

  vcPointsInOrder.push_back(SegmentPtr(region.Segment(0))[0]);
  stSegmentsDone.insert(0);

  int iNextPoint = SegmentPtr(region.Segment(0))[1];

  while (iNextPoint != SegmentPtr(region.Segment(0))[0]) {
    TPointToSegmentMap::iterator it = mpSegments.find(iNextPoint);
    assert(it != mpSegments.end()); // no closed ring?

    assert(it->first >= 0 && it->first < m_input->numberofpoints);
    assert(it->second.size() == 2);
    assert(it->second[0] >= 0 && it->second[0] < m_input->numberofsegments);
    assert(it->second[1] >= 0 && it->second[1] < m_input->numberofsegments);
    assert(it->second[0] < region.SegmentSize());
    assert(it->second[1] < region.SegmentSize());

    // local segment indices in the region
    int iFirstSegment = it->second[0];
    int iSecondSegment = it->second[1];

    if (stSegmentsDone.find(iFirstSegment) == stSegmentsDone.end()) {
      // use it
      vcPointsInOrder.push_back(iNextPoint);
      stSegmentsDone.insert(iFirstSegment);

      if (iNextPoint == SegmentPtr(region.Segment(iFirstSegment))[0]) {
        iNextPoint = SegmentPtr(region.Segment(iFirstSegment))[1];
      } else {
        assert(iNextPoint == SegmentPtr(region.Segment(iFirstSegment))[1]);
        iNextPoint = SegmentPtr(region.Segment(iFirstSegment))[0];
      }
    } else {
      // take the other one
      assert(stSegmentsDone.find(iSecondSegment) == stSegmentsDone.end());
      vcPointsInOrder.push_back(iNextPoint);
      stSegmentsDone.insert(iSecondSegment);

      if (iNextPoint == SegmentPtr(region.Segment(iSecondSegment))[0]) {
        iNextPoint = SegmentPtr(region.Segment(iSecondSegment))[1];
      } else {
        assert(iNextPoint == SegmentPtr(region.Segment(iSecondSegment))[1]);
        iNextPoint = SegmentPtr(region.Segment(iSecondSegment))[0];
      }
    }
  }

  assert(vcPointsInOrder.size() == region.SegmentSize());

  assert(vcPointsInOrder.size() == stSegmentsDone.size());

  geo::CArray<geo::CPoint> arPoints;

  for (size_t i = 0; i < vcPointsInOrder.size(); i++) {
    assert(vcPointsInOrder[i] < m_input->numberofpoints);
    geo::CPoint pt_tmp(PointCoord(vcPointsInOrder[i])[0], PointCoord(vcPointsInOrder[i])[1], 0);
    arPoints.PushBack(pt_tmp);
  }

  geo::CPolygon poly(arPoints);

  double signedarea = poly.SignedArea();
  assert(fabs(signedarea) > EPS);

  // get the first segment of the polygon and take its midpoint
  geo::CPoint mid = (poly.Point(0) + poly.Point(1)) / 2;

  // we are in 2D, so get crossproduct with z-axis to obtain normal
  geo::CVector vecSegment = poly.Point(1) - poly.Point(0);

  geo::CVector vecNormal = vecSegment.CrossProduct(geo::CVector::Zaxis);

  vecNormal = vecNormal.UnitVector() * 2 * EPS; // stay very close to the edge, to prevent running out on the other side

  // If signed area is positive, the normal points out of the polygon region
  if (signedarea > 0)
    vecNormal.Flip();

  geo::CPoint ret = mid + vecNormal;
  assert(poly.Contains(ret, false));

  return ret;
}

int CTriangleSurface::RegionSize() const { return (int)m_vcRegions.size(); }

const CRegion &CTriangleSurface::Region(int nIndex) const {
  assert(nIndex >= 0 && nIndex < RegionSize());

  return *m_vcRegions[nIndex];
}

CRegion &CTriangleSurface::Region(int nIndex) {
  assert(nIndex >= 0 && nIndex < RegionSize());

  return *m_vcRegions[nIndex];
}

const int *CTriangleSurface::SegmentPtr(int n) const {
  assert(n >= 0 && n < m_input->numberofsegments);
  return &m_input->segmentlist[2 * n];
}

int CTriangleSurface::SegmentSize() const { return m_input->numberofsegments; }

CTriangleSurface::TSegment CTriangleSurface::Segment(size_t nIndex) const {
  assert(nIndex < m_input->numberofsegments);
  return TSegment(m_input->segmentlist[2 * nIndex], m_input->segmentlist[2 * nIndex + 1]);
}

const double *CTriangleSurface::PointCoord(int n) const {
  assert(n >= 0 && n < m_input->numberofpoints);
  return &m_input->pointlist[2 * n];
}

void CTriangleSurface::AllowSteinerPointsOnBoundary(bool allow) { m_bAllowSteinerPointsOnBoundary = allow; }

int CTriangleSurface::InternalInputPointSize() const { return m_input ? m_input->numberofpoints : 0; }
const double *CTriangleSurface::InternalInputPoint(int index) const { return m_input->pointlist + 2 * index; }
