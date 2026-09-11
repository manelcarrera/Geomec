 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriSurface.cpp: implementation of the CTriSurface class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "TriSurface.h"
#include "Triangle.h"

//////////////////////////////////////////////////////////////////////
// Implementation of CTriSurface::CTriFace
//////////////////////////////////////////////////////////////////////
namespace geo {
CTriSurface::CTriSurface()
{
}

CTriSurface::CTriSurface(const ISurface& rhs)
: m_vcPoint(rhs.PointSize()), m_vcFace(rhs.FaceSize())
{
  // When the system is indexed building a pointmap is not nessarry.
  bool bIndexed = false;		
  std::map<geo::CPoint, int> mpPoint;
  if(rhs.FaceSize() > 0)
  {
    if(rhs.Face(0).IndexingElementSet() == &rhs)
      bIndexed = true;
  }

  for(int i = 0; i < rhs.PointSize(); i++)
  {
    m_vcPoint[i] = rhs.Point(i);
    if(!bIndexed)
    {
      bool bSuccess = mpPoint.insert(std::map<geo::CPoint, int>::value_type(m_vcPoint[i], i)).second;
      assert(bSuccess);
    }
  }

  for(int j = 0; j < rhs.FaceSize(); j++)
  {
    if(bIndexed)
    {
      assert(rhs.Face(j).NrOfPoints() == 3);
      assert(rhs.Face(j).IndexingElementSet() == &rhs);
      m_vcFace[j] = new CTriangle(*this, j, rhs.Face(j).PointIndex(0), rhs.Face(j).PointIndex(1), rhs.Face(j).PointIndex(2));
      InsertFaceIntoPointMap(*m_vcFace[j]);
    }
    else
    {
      std::map<geo::CPoint, int>::iterator it0 = mpPoint.find(rhs.Face(j).Point(0));
      std::map<geo::CPoint, int>::iterator it1 = mpPoint.find(rhs.Face(j).Point(1));
      std::map<geo::CPoint, int>::iterator it2 = mpPoint.find(rhs.Face(j).Point(2));			
      assert(it0 != mpPoint.end());
      assert(it1 != mpPoint.end());
      assert(it2 != mpPoint.end());
      m_vcFace[j] = new CTriangle(*this, j, it0->second, it1->second, it2->second);
      InsertFaceIntoPointMap(*m_vcFace[j]);
    }
  }
}

CTriSurface::CTriSurface(const std::vector<geo::CPoint>& vcPoint, const std::vector<std::vector<int> >& vcTriangle)
: m_vcPoint(vcPoint), m_vcFace(vcTriangle.size())
{
  for(int j = 0; j < vcTriangle.size(); j++)
  {
    assert(vcTriangle[j].size() == 3);
    m_vcFace[j] = new CTriangle(*this, j, vcTriangle[j][0], vcTriangle[j][1], vcTriangle[j][2]);
    InsertFaceIntoPointMap(*m_vcFace[j]);
  }
}

CTriSurface::~CTriSurface()
{
  Clear();
}

void CTriSurface::InsertFaceIntoPointMap(CTriangle &newface)
{
  TPointToFacesMap::iterator it;
  it = m_mpPointToFaces.insert(TPointToFacesMap::value_type(&m_vcPoint[newface.PointIndex(0)], CPtrArray<IFace>())).first;
  it->second.PushBack(newface);
  it = m_mpPointToFaces.insert(TPointToFacesMap::value_type(&m_vcPoint[newface.PointIndex(1)], CPtrArray<IFace>())).first;
  it->second.PushBack(newface);
  it = m_mpPointToFaces.insert(TPointToFacesMap::value_type(&m_vcPoint[newface.PointIndex(2)], CPtrArray<IFace>())).first;
  it->second.PushBack(newface);
}

int CTriSurface::AddPoint(const IPoint& point)
{
  int nIndex = (int)m_vcPoint.size();
  m_vcPoint.push_back(CPoint(point));
  return nIndex;
}
    
int CTriSurface::AddTriangle(int P0, int P1, int P2)
{
  int nIndex = (int)m_vcFace.size();
  CTriangle *pFace = new CTriangle(*this, nIndex, P0, P1, P2);
  m_vcFace.push_back(pFace);
  InsertFaceIntoPointMap(*pFace);
  return nIndex;
}

void CTriSurface::reserveFaceSize(size_t size)
{
  m_vcFace.reserve(size);
  m_vcPoint.reserve(size);
}

const CPtrArray <IFace> CTriSurface::FacesAt(const IPoint &/*p*/) const
{
  assert(false); // Not implemented yet
  return CPtrArray<IFace>();
}

const CPtrArray <IFace> CTriSurface::FacesAtNode(const IPoint &p) const
{
  TPointToFacesMap::const_iterator it = m_mpPointToFaces.find(&p);
  assert(it != m_mpPointToFaces.end());
  return it->second;
}

const IFace &CTriSurface::Face(int nIndex) const
{
  assert(nIndex >= 0);
  assert(nIndex < m_vcFace.size());
  return *m_vcFace[nIndex];
}

int CTriSurface::FaceSize() const
{
  return (int)m_vcFace.size();
}

const IPoint& CTriSurface::Point(int nIndex) const
{
  return m_vcPoint[nIndex];
}

void CTriSurface::Clear()
{
  m_vcPoint.clear();
  for(int i = 0; i < m_vcFace.size(); i++)
    delete m_vcFace[i];
  m_vcFace.clear();
}

int CTriSurface::PointSize() const
{
  return (int)m_vcPoint.size();
}

std::vector<int> CTriSurface::Nodes(const IElement &element) const
{
  assert(element.IndexingElementSet() == this);
  std::vector<int> vcRet(element.NrOfPoints());
  for(int i = 0; i < element.NrOfPoints(); i++)
    vcRet[i] = element.PointIndex(i);
  return vcRet;
}

IPoint &CTriSurface::PointAt(int nIndex)
{
  assert((nIndex >= 0)||(nIndex < m_vcPoint.size()));
  assert(false);
  return m_vcPoint[nIndex];
}

void CTriSurface::Point(int nIndex, const IPoint& point)
{
  assert((nIndex >= 0)||(nIndex < m_vcPoint.size()));
  m_vcPoint[nIndex] = point;
  InvalidateCache();
}

}
