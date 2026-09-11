/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// triangle.cpp: implementation of the triangle class.
//
//////////////////////////////////////////////////////////////////////

#include "Triangle.h"
#include "IMesh.h"
#include "ISurface.h"
#include "Node.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CTriangle::CTriangle() : m_pPoint(0), m_pNode(0), m_nIndex(-1), m_pElementSet(0), m_pMesh(0) { assert(Empty()); }

CTriangle::CTriangle(const ITriangle &rhs) : m_pPoint(0), m_pNode(0), m_nIndex(-1), m_pElementSet(0), m_pMesh(0) {
  if (!rhs.Empty()) {
    assert(rhs.NrOfPoints() == 3);
    m_pPoint = new CPoint[3];
    m_pPoint[0] = rhs.Point(0);
    m_pPoint[1] = rhs.Point(1);
    m_pPoint[2] = rhs.Point(2);
  }
}

CTriangle::CTriangle(const IPoint &p1, const IPoint &p2, const IPoint &p3)
    : m_pPoint(0), m_pNode(0), m_nIndex(-1), m_pElementSet(0), m_pMesh(0) {
  m_pPoint = new CPoint[3];
  m_pPoint[0] = p1;
  m_pPoint[1] = p2;
  m_pPoint[2] = p3;
}

CTriangle::CTriangle(IMesh &mesh, int p1, int p2, int p3)
    : m_pPoint(0), m_pNode(0), m_nIndex(-1), m_pElementSet(0), m_pMesh(&mesh) {
  m_pNode = new int[3];
  m_pNode[0] = p1;
  m_pNode[1] = p2;
  m_pNode[2] = p3;

  m_nIndex = mesh.RegisterElement(*this);
}

CTriangle::CTriangle(IMesh &mesh, const IPoint &p1, const IPoint &p2, const IPoint &p3, bool bUnique)
    : m_pPoint(0), m_pNode(0), m_nIndex(-1), m_pElementSet(0), m_pMesh(&mesh) {
  m_pNode = new int[3];
  m_pNode[0] = mesh.RegisterNode(p1, bUnique);
  m_pNode[1] = mesh.RegisterNode(p2, bUnique);
  m_pNode[2] = mesh.RegisterNode(p3, bUnique);

  m_nIndex = mesh.RegisterElement(*this);
}

CTriangle::CTriangle(const ISurface &surface, int nIndex, int p1, int p2, int p3)
    : m_pPoint(0), m_pNode(0), m_nIndex(nIndex), m_pElementSet(&surface), m_pMesh(0) {
  m_pNode = new int[3];
  m_pNode[0] = p1;
  m_pNode[1] = p2;
  m_pNode[2] = p3;
}

CTriangle::~CTriangle() {
  if (m_pNode)
    delete[] m_pNode;
  if (m_pPoint)
    delete[] m_pPoint;
}

size_t CTriangle::NrOfNodes() const {
  if (m_pMesh)
    return 3;
  return 0;
}

const INode &CTriangle::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  assert(!m_pPoint);
  assert(m_pNode);
  assert(m_pMesh);

  return m_pMesh->Node(m_pNode[nIndex]);
}

void CTriangle::Node(int nIndex, const IPoint &point) {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  assert(!m_pPoint);
  assert(m_pNode);
  assert(m_pMesh);

  m_pMesh->Node(m_pNode[nIndex], point);
}

const IPoint &CTriangle::Point(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfPoints());

  if (m_pPoint)
    return m_pPoint[nIndex];

  assert(m_pNode);

  if (m_pMesh)
    return m_pMesh->Node(m_pNode[nIndex]);

  assert(m_pElementSet);
  return m_pElementSet->Point(m_pNode[nIndex]);
}

void CTriangle::Point(int nIndex, const IPoint &pt) {
  if (m_pNode) {
    if (m_pMesh)
      m_pMesh->Node(m_pNode[nIndex], pt);
    assert(false);
  }

  if (!m_pPoint)
    m_pPoint = new CPoint[3];

  m_pPoint[nIndex] = pt;
}

size_t CTriangle::Order() const { return 1; }

const IElementSet *CTriangle::IndexingElementSet() const {
  if (m_pMesh)
    return m_pMesh;
  return m_pElementSet;
}

int CTriangle::Index() const { return m_nIndex; }

int CTriangle::PointIndex(int nIndex) const {
  assert(!Empty() && nIndex < 3);
  assert(m_pNode);
  return m_pNode[nIndex];
}

std::string CTriangle::Type() const { return "T6EPS"; }

int CTriangle::NrOfPoints() const {
  if (m_pPoint || m_pNode)
    return ITriangle::NrOfPoints();

  return 0;
}

} // namespace geo
