/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "Region.h"

#include "TriangleElement.h"
#include "TriangleSurface.h"

CRegion::CRegion(const CTriangleSurface &surface) : m_surface(surface), m_maxarea(-1), m_bAreaConstraint(false) {}

// note that m_segments does a deep copy of the segments parameter
CRegion::CRegion(const CTriangleSurface &surface, const TSegments &segments)
    : m_surface(surface), m_segments(segments), m_maxarea(-1), m_bAreaConstraint(false) {}

// note that m_segments does a deep copy of the segments parameter
CRegion::CRegion(const CTriangleSurface &surface, const TSegments &segments, const double &maxarea)
    : m_surface(surface), m_segments(segments), m_maxarea(maxarea), m_bAreaConstraint(true) {}

CRegion::~CRegion() {}

void CRegion::AddTriangle(int nIndex) {
  m_vcTriangleIndices.push_back(nIndex);
  const geo::ITriangle &triangle = Surface().Triangle(nIndex);
  int i;
  for (i = 0; i < triangle.NrOfPoints(); i++)
    m_stNodes.insert(triangle.PointIndex(i));
}

const CTriangleSurface &CRegion::Surface() const { return m_surface; }

size_t CRegion::TriangleSize() const { return m_vcTriangleIndices.size(); }

const geo::ITriangle &CRegion::Triangle(size_t nIndex) const {
  assert(nIndex < TriangleSize());

  return Surface().Triangle(m_vcTriangleIndices[nIndex]);
}

size_t CRegion::SegmentSize() const { return m_segments.size(); }

int CRegion::Segment(size_t nIndex) const {
  assert(nIndex < SegmentSize());
  return m_segments[nIndex];
}

void CRegion::Segments(const TSegments &segments) {
  m_segments.clear();
  m_segments = segments; // deep copy
}

void CRegion::MaximumArea(const double &maxarea) {
  m_maxarea = maxarea;
  m_bAreaConstraint = true;
}

const double &CRegion::MaximumArea() const {
  assert(AreaConstraint());
  return m_maxarea;
}

bool CRegion::AreaConstraint() const { return m_bAreaConstraint; }

void CRegion::ReleaseAreaConstraint() {
  m_bAreaConstraint = false;
  m_maxarea = -1;
}

void CRegion::Clear() {
  m_vcTriangleIndices.clear();
  m_stNodes.clear();
  m_vcNodes.clear();
}

void CRegion::Triangles(const std::vector<int> &vcTriangleIndices) {
  for (size_t i = 0; i < vcTriangleIndices.size(); i++)
    AddTriangle(vcTriangleIndices[i]);
}

void CRegion::BuildNodeCache() const {
  assert(m_vcNodes.empty());

  std::set<int>::const_iterator it;
  for (it = m_stNodes.begin(); it != m_stNodes.end(); it++)
    m_vcNodes.push_back(*it);
}

geo::IPoint &CRegion::PointAt(int /*nIndex*/) {
  // not allowed
  assert(false);
  geo::IPoint *pBogus = 0;
  return *pBogus;
}

void CRegion::AssertValid() const {}

const geo::IPoint &CRegion::Point(int nIndex) const {
  assert(nIndex >= 0 && nIndex < PointSize());
  if (m_vcNodes.empty())
    BuildNodeCache();
  assert(nIndex < m_vcNodes.size());

  return Surface().Point(m_vcNodes[nIndex]);
}

int CRegion::PointIndex(int nIndex) const {
  assert(nIndex >= 0 && nIndex < PointSize());
  if (m_vcNodes.empty())
    BuildNodeCache();
  assert(nIndex < m_vcNodes.size());

  return m_vcNodes[nIndex];
}

int CRegion::PointSize() const { return (int)m_stNodes.size(); }

const geo::IElement &CRegion::Element(int nIndex) const { return Triangle(nIndex); }

int CRegion::ElementSize() const { return TriangleSize(); }

std::vector<int> CRegion::Nodes(const geo::IElement &element) const {
  const CTriangleElement *tri = dynamic_cast<const CTriangleElement *>(&element);
  assert(tri);

  std::vector<int> ret;
  int i;

  for (i = 0; i < tri->NrOfPoints(); i++)
    ret.push_back(tri->PointIndex(i));

  return ret;
}
