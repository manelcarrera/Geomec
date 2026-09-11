/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// SurfaceBookKeeper.cpp: implementation of the CSurfaceBookKeeper class.
//
//////////////////////////////////////////////////////////////////////

#include "Surface.h"
#include "EmbeddedTriangle.h"
#include "dimple.h"
//////////////////////////////////////////////////////////////////////
// Implementation of CSurface::CFace
//////////////////////////////////////////////////////////////////////
namespace geo {

CSurface::CFace::CFace(CSurface &surface, int nIndex, const std::vector<int> &vcPoint)
    : m_vcPoint(vcPoint), m_surface(surface), m_nIndex(nIndex) {}

const IPoint &CSurface::CFace::Point(int nIndex) const {
  assert(nIndex >= 0);
  assert(nIndex < m_vcPoint.size());
  return m_surface.Point(m_vcPoint[nIndex]);
}

void CSurface::CFace::Point(int /*nIndex*/, const IPoint & /*pt*/) {
  assert(false); // Not implemented
}

int CSurface::CFace::NrOfPoints() const { return (int)m_vcPoint.size(); }

double CSurface::CFace::Size() const {
  assert(false); // Not implemented
  return 0;
}

CValue CSurface::CFace::InterpolateValue(const IPoint & /*point*/, const std::vector<CValue> & /*values*/) const {
  assert(false); // Not implemented
  return CValue();
}

const IElementSet *CSurface::CFace::IndexingElementSet() const { return &m_surface; }

int CSurface::CFace::Index() const {
  assert(this == &m_surface.Face(m_nIndex));
  return m_nIndex;
}

int CSurface::CFace::PointIndex(int nIndex) const {
  assert(nIndex >= 0);
  assert(nIndex < m_vcPoint.size());
  return m_vcPoint[nIndex];
}

size_t CSurface::CFace::Order() const { return 1; }

//////////////////////////////////////////////////////////////////////
// Implementation of CSurface::CFace
//////////////////////////////////////////////////////////////////////
CSurface::CSurface() {}

CSurface::CSurface(const ISurface &rhs) {
  for (int i = 0; i < rhs.PointSize(); i++) {
    VERIFY(RegisterPoint(rhs.Point(i)) == i);
  }
  for (int j = 0; j < rhs.FaceSize(); j++) {
    AddFace(rhs.Face(j));
  }
}

CSurface::CSurface(const std::vector<geo::CPoint> &vcPoint, const std::vector<std::vector<int>> &vcTriangle) {
  for (int i = 0; i < vcPoint.size(); i++) {
    VERIFY(RegisterPoint(vcPoint[i]) == i);
  }
  for (int j = 0; j < vcTriangle.size(); j++) {
    switch (vcTriangle[j].size()) {
    case 3:
      AddTriangle(vcPoint[vcTriangle[j][0]], vcPoint[vcTriangle[j][1]], vcPoint[vcTriangle[j][2]]);
      break;
    case 4:
      AddRectangle(vcPoint[vcTriangle[j][0]], vcPoint[vcTriangle[j][1]], vcPoint[vcTriangle[j][2]],
                   vcPoint[vcTriangle[j][3]]);
      break;
    default:
      m_vcFace.push_back(new CFace(*this, (int)m_vcFace.size(), vcTriangle[j]));
    }
  }
}

CSurface::~CSurface() { Clear(); }

int CSurface::RegisterPoint(const geo::IPoint &point) {
  if (m_mpPoint.find(&point) == m_mpPoint.end()) {
    CPoint *pPoint = new CPoint(point);
    m_mpPoint.insert(TPointMap::value_type(pPoint, (int)m_vcPoint.size()));
    m_vcPoint.push_back(pPoint);
    m_vcFaceAtPoint.push_back(std::vector<int>());
    assert(m_vcPoint.size() == m_vcFaceAtPoint.size());
  }

  return m_mpPoint.find(&point)->second;
}

int CSurface::AddTriangle(const IPoint &p0, const IPoint &p1, const IPoint &p2) {
  int nIndex = (int)m_vcFace.size();
  std::vector<int> vcPoint(3);
  vcPoint[0] = RegisterPoint(p0);
  vcPoint[1] = RegisterPoint(p1);
  vcPoint[2] = RegisterPoint(p2);
  m_vcFace.push_back(new CFace(*this, nIndex, vcPoint));
  return nIndex;
}

int CSurface::AddEmbeddedTriangle(const IBody &parent, const IPoint &p0, const IPoint &p1, const IPoint &p2) {
  int nIndex = (int)m_vcFace.size();
  std::vector<int> vcPoint(3);
  vcPoint[0] = RegisterPoint(p0);
  vcPoint[1] = RegisterPoint(p1);
  vcPoint[2] = RegisterPoint(p2);
  m_vcFace.push_back(new CEmbeddedTriangle(parent, *this, nIndex, vcPoint[0], vcPoint[1], vcPoint[2]));
  return nIndex;
}

int CSurface::AddRectangle(const IPoint &p0, const IPoint &p1, const IPoint &p2, const IPoint &p3) {
  int nIndex = (int)m_vcFace.size();
  std::vector<int> vcPoint(4);
  vcPoint[0] = RegisterPoint(p0);
  vcPoint[1] = RegisterPoint(p1);
  vcPoint[2] = RegisterPoint(p2);
  vcPoint[3] = RegisterPoint(p3);
  m_vcFace.push_back(new CFace(*this, nIndex, vcPoint));
  return nIndex;
}

int CSurface::AddFace(const IFace &face) {
  if (face.NrOfPoints() == 3)
    return AddTriangle(face.Point(0), face.Point(1), face.Point(2));
  if (face.NrOfPoints() == 4)
    return AddRectangle(face.Point(0), face.Point(1), face.Point(2), face.Point(3));
  int nIndex = (int)m_vcFace.size();
  std::vector<int> vcPoint(face.NrOfPoints());
  for (int i = 0; i < vcPoint.size(); i++)
    vcPoint[i] = RegisterPoint(face.Point(i));
  m_vcFace.push_back(new CFace(*this, nIndex, vcPoint));
  return nIndex;
}

void CSurface::reserveFaceSize(size_t size) { m_vcFace.reserve(size); }

const CPtrArray<IFace> CSurface::FacesAt(const IPoint &p) const {
  CPtrArray<IFace> vcRet;
  std::vector<int> vcElement = ElementsAt(p);
  for (int i = 0; i < vcElement.size(); i++) {
    vcRet.PushBack(const_cast<IFace &>(Face(vcElement[i])));
  }
  return vcRet;
}

const CPtrArray<IFace> CSurface::FacesAtNode(const IPoint &p) const {
  assert(m_mpPoint.find(&p) != m_mpPoint.end());
  CPtrArray<IFace> vcRet;
  std::vector<int> vcElement = ElementsAt(p);
  for (int i = 0; i < vcElement.size(); i++) {
    vcRet.PushBack(const_cast<IFace &>(Face(vcElement[i])));
  }
  return vcRet;
}

const IFace &CSurface::Face(int nIndex) const {
  assert(nIndex >= 0);
  assert(nIndex < m_vcFace.size());
  return *m_vcFace[nIndex];
}

int CSurface::FaceSize() const { return (int)m_vcFace.size(); }

const IPoint &CSurface::Point(int nIndex) const { return *m_vcPoint[nIndex]; }

void CSurface::Clear() {
  m_mpPoint.clear();
  int i;
  for (i = 0; i < m_vcFace.size(); i++)
    delete m_vcFace[i];
  m_vcFace.clear();
  for (i = 0; i < m_vcPoint.size(); i++)
    delete m_vcPoint[i];
  m_vcPoint.clear();
  m_vcFaceAtPoint.clear();
}

int CSurface::PointSize() const { return (int)m_vcPoint.size(); }

std::vector<int> CSurface::Nodes(const IElement & /*element*/) const {
  assert(false);
  return std::vector<int>();
}

IPoint &CSurface::PointAt(int /*nIndex*/) {
  assert(false);
  IPoint *pBogus = 0;
  return *pBogus;
}

} // namespace geo
