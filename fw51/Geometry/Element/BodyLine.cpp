/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "BodyLine.h"
#include "IBody.h"

namespace geo {

CBodyLine::CBodyLine(IBody &parent, int nIndex) : m_parent(parent), m_nIndex(nIndex) {}

CBodyLine::~CBodyLine() {}

const IBody &CBodyLine::localParent() const { return m_parent; }

IBody &CBodyLine::localParent() { return m_parent; }

size_t CBodyLine::NrOfNodes() const { return Order() + 1; }

const INode &CBodyLine::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return localParent().Node(BodyNode(nIndex));
}

void CBodyLine::Node(int nIndex, const IPoint &point) {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  localParent().Node(BodyNode(nIndex), point);
}

size_t CBodyLine::Order() const { return localParent().Order(); }

int CBodyLine::PointIndex(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return localParent().PointIndex(BodyNode(nIndex));
}

int CBodyLine::BodyPoint(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfPoints());
  return localParent().LinePointIndices(m_nIndex)[nIndex];
}

int CBodyLine::BodyNode(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return localParent().LineNodeIndices(m_nIndex)[nIndex];
}

} // namespace geo
