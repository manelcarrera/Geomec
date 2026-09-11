
#include <algorithm>

#include "MeshSurface.h"
#include "IFace.h"

CMeshSurface::CMeshSurface()
{
}

const geo::CPtrArray<geo::IFace> CMeshSurface::FacesAt(const geo::IPoint& /*p*/) const
{
  assert(false);
  return geo::CPtrArray<geo::IFace>();
}

const geo::CPtrArray<geo::IFace> CMeshSurface::FacesAtNode(const geo::IPoint& p) const
{
  TPointMap::const_iterator it = m_mpPoints.find(&p);
  assert(it != m_mpPoints.end()); // p must be a point (node) of the surface

  geo::CPtrArray<geo::IFace> arRet;

  for(size_t i = 0; i < it->second.size(); ++i)
  arRet.PushBack(const_cast<geo::IFace&>(*it->second[i]));

  return arRet;
}

const geo::IFace& CMeshSurface::Face(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcFaces.size());
  return *m_vcFaces[nIndex];
}

int CMeshSurface::FaceSize() const
{
  return m_vcFaces.size();
}

const geo::IPoint& CMeshSurface::Point(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcPoints.size());
  return *m_vcPoints[nIndex];
}

int CMeshSurface::PointSize() const
{
  return m_vcPoints.size();
}

std::vector<int> CMeshSurface::Nodes(const geo::IElement& /*element*/) const
{
  assert(false);
  return std::vector<int>();
}

void CMeshSurface::AddFace(const geo::IFace& face)
{
  assert(std::find(m_vcFaces.begin(), m_vcFaces.end(), &face) == m_vcFaces.end());
  m_vcFaces.push_back(&face);

  int i;
  for(i = 0; i < face.NrOfPoints(); ++i)
  {
  std::pair<TPointMap::iterator, bool> prInsert = m_mpPoints.insert(TPointMap::value_type(&face.Point(i), std::vector<const geo::IFace*>()));

  // register the face with the point
  prInsert.first->second.push_back(&face);

  // if inserted, add the point to the points vector
  if(prInsert.second)
      m_vcPoints.push_back(&face.Point(i));
  }
}

void CMeshSurface::reserveFaceSize(size_t size)
{
  m_vcFaces.reserve(size);
  m_vcPoints.reserve(size);
}

void CMeshSurface::Clear()
{
  m_vcFaces.clear();
  m_vcPoints.clear();
  m_mpPoints.clear();
}

geo::IPoint& CMeshSurface::PointAt(int /*nIndex*/)
{
  assert(false);
  geo::IPoint* p = 0;
  return *p;
}
