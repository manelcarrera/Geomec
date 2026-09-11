/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "BodyQuadrilateral.h"
#include "IBody.h"

namespace geo {

CBodyQuadrilateral::CBodyQuadrilateral(IBody &parent, int nIndex) : m_parent(parent), m_nIndex(nIndex) {}

CBodyQuadrilateral::~CBodyQuadrilateral() {}

const IBody *CBodyQuadrilateral::Parent() const { return &m_parent; }

int CBodyQuadrilateral::PointIndex(int nIndex) const { return Parent()->PointIndex(BodyNode(nIndex)); }

size_t CBodyQuadrilateral::NrOfNodes() const {
  switch (Order()) {
  case 1:
    return 4;
  case 2:
    return 8;
  case 3:
    return 12;
  }

  assert(false);
  return 0;
}

const INode &CBodyQuadrilateral::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return Parent()->Node(BodyNode(nIndex));
}

void CBodyQuadrilateral::Node(int nIndex, const IPoint &node) {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  m_parent.Node(BodyNode(nIndex), node);
}

size_t CBodyQuadrilateral::Order() const { return Parent()->Order(); }

int CBodyQuadrilateral::BodyPoint(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return Parent()->FacePointIndices(m_nIndex)[nIndex];
}

int CBodyQuadrilateral::BodyNode(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return Parent()->FaceNodeIndices(m_nIndex)[nIndex];
}

int CBodyQuadrilateral::Index() const { return m_nIndex; }

} // namespace geo
