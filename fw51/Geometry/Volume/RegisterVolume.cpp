/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IRegisterVolume.cpp: implementation of the IRegisterVolume class.
//
//////////////////////////////////////////////////////////////////////
#include "RegisterVolume.h"
#include "IBody.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CRegisterVolume::CRegisterVolume() : m_vcPoint(1) { Clear(); }

CRegisterVolume::~CRegisterVolume() {}

const IBody &CRegisterVolume::Body(int nIndex) const { return *m_vcBody[nIndex]; }

int CRegisterVolume::BodySize() const { return (int)m_vcBody.size(); }

void CRegisterVolume::Clear() {
  // Destroy hexahedron first
  for (size_t i = 0; i < m_vcBody.size(); i++)
    delete m_vcBody[i];

  // Clear map and vector
  m_vcBody.clear();
  m_vcPointToBody.clear();
  m_vcPoint.Clear();
  m_mpPoint.clear();
}

int CRegisterVolume::RegisterPoint(IPoint &point) {
  // Check point in register
  TPointMap::iterator it = m_mpPoint.find(&point);
  if (it == m_mpPoint.end()) {
    // Register the new point
    int nIndex = m_vcPoint.Size();

    m_mpPoint.insert(TPointMap::value_type(&point, nIndex));
    m_vcPoint.PushBack(point);
    m_vcPointToBody.push_back(TBodySet());

    return nIndex;
  }

  return it->second;
}

int CRegisterVolume::RegisterBody(IBody &body) {
  assert(body.IndexingElementSet() == this);

  // Register body in body vector
  int nElementIndex = (int)m_vcBody.size();

  m_vcBody.push_back(&body);

  // Register connnectivity
  for (int i = 0; i < body.NrOfPoints(); i++)
    m_vcPointToBody[body.PointIndex(i)].insert(nElementIndex);

  return nElementIndex;
}

const IPoint &CRegisterVolume::Point(int nIndex) const { return (const IPoint &)m_vcPoint.ObjectBase(nIndex); }

IPoint &CRegisterVolume::PointAt(int /*nIndex*/) {
  IPoint *pBogus = 0;
  assert(false);
  return *pBogus;
}

int CRegisterVolume::PointSize() const { return m_vcPoint.Size(); }

std::vector<int> CRegisterVolume::Nodes(const IElement & /*element*/) const {
  assert(false);
  return std::vector<int>();
}

std::vector<const IBody *> CRegisterVolume::PointToBody(const IPoint &point) const {
  TPointMap::const_iterator it = m_mpPoint.find(&point);
  assert(it != m_mpPoint.end());

  const TBodySet &body_set = m_vcPointToBody[it->second];

  std::vector<const IBody *> vcRet(body_set.size());
  int nIndex = 0;
  for (TBodySet::const_iterator itt = body_set.begin(); itt != body_set.end(); itt++) {
    vcRet[nIndex] = m_vcBody[*itt];
    nIndex++;
  }

  return vcRet;
}

void CRegisterVolume::AssertValid() const {}

} // namespace geo
