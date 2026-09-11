#include "CacheInterface.h"
#include "IElementSet.h"

#include "Point.h"
#include "ElementPoint.h"
#include "ElementCacheObject.h"
#include "ValueMapper.h"

#include <vector>
#include <map>

#ifdef _WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <cmath>


namespace
{

inline double hash_round(double d)
{
  return round(d * 1E8) / 1E8;
}

inline size_t hash(const geo::IPoint& point)
{
#ifdef _WIN32
  size_t hx = std::hash<double>()(hash_round(point.X()));
  size_t hy = std::hash<double>()(hash_round(point.Y()));
  size_t hz = std::hash<double>()(hash_round(point.Z()));
#else
  size_t hx = std::tr1::hash<double>()(hash_round(point.X()));
  size_t hy = std::tr1::hash<double>()(hash_round(point.Y()));
  size_t hz = std::tr1::hash<double>()(hash_round(point.Z()));
#endif
  return (hx ^ (hy << 1)) ^ hz;
}

}


CCacheInterface::CCacheInterface(CCacheManager &parent, const geo::IElementSet *elt_set) : m_parent(&parent), m_elt_set(elt_set)
{
	int size = elt_set->ElementSize();
	if (size > 0)
	{
		m_min.resize(size);
		m_max.resize(size);
		m_mid.resize(size);
		m_shapes_legacy.resize(size);
    m_shapes.resize(size);
  }
}

bool CCacheInterface::NeedsDelete()
{
	return true;
}

void CCacheInterface::Resize(std::size_t size)
{
	if (size > m_min.size())
	{
		m_min.resize(size + 10);
		m_max.resize(size + 10);
		m_mid.resize(size + 10);
		m_shapes_legacy.resize(size + 10);
    m_shapes.resize(size + 10);
  }
}

geo::CPoint CCacheInterface::Min(int nIndex)
{
	return m_min[nIndex];
}

void CCacheInterface::Min(int nIndex, geo::CPoint &Point)
{
	m_min[nIndex] = Point;
}

geo::CPoint CCacheInterface::Max(int nIndex)
{
	return m_max[nIndex];
}

void CCacheInterface::Max(int nIndex, geo::CPoint &Point)
{
	m_max[nIndex] = Point;
}

geo::CElementPoint CCacheInterface::Mid(int nIndex)
{
	return m_mid[nIndex];
}

void CCacheInterface::Mid(int nIndex, geo::CElementPoint &ElementPoint)
{
	m_mid[nIndex] = ElementPoint;
}

bool CCacheInterface::PointMap(const geo::IPoint &Point, std::vector<int> &Map)
{
  bool bFound = false;

  if (CValueMapper::instance()->AllowParallelMapping())
  {
    size_t h = hash(Point);

    TPointMap::accessor ac;
    if (m_points.find(ac, h))
    {
      for (size_t i = 0; i < ac->second.size(); ++i)
      {
        if (ac->second[i].first == Point)
        {
          Map = std::vector<int>(ac->second[i].second);
          bFound = true;
          break;
        }
      }
    }
    ac.release();
  }
  else
  {
    TPointMapLegacy::iterator it = m_points_legacy.find(Point);
    if (it != m_points_legacy.end())
    {
      Map.reserve(it->second.size());
      for (size_t i = 0; i < it->second.size(); ++i)
        Map.push_back(it->second[i]);
      bFound = true;
    }
  }

  return bFound;
}

void CCacheInterface::SetPointMap(const geo::IPoint &Point, std::vector<int> &Map)
{
  if (CValueMapper::instance()->AllowParallelMapping())
  {
    size_t h = hash(Point);

    bool bFound = false;

    TPointMap::accessor ac;
    m_points.insert(ac, h);

    for (size_t i = 0; i < ac->second.size(); ++i)
    {
      if (ac->second[i].first == Point)
      {
        ac->second[i].second = std::vector<int>(Map);
        bFound = true;
        break;
      }
    }
    if (!bFound)
      ac->second.push_back(std::pair<geo::CPoint, std::vector<int> >(Point, std::vector<int>(Map)));
    ac.release();
  }
  else
  {
    m_points_legacy[Point] = std::vector<int>(Map);
  }
}

bool CCacheInterface::ShapeMap(int nIndex, const geo::IPoint &Point, std::vector<double> &Map)
{
  bool bFound = false;

  if (CValueMapper::instance()->AllowParallelMapping())
  {
    size_t h = hash(Point);

    TShapeMap::accessor ac;
    if (m_shapes[nIndex].find(ac, h))
    {
      for (size_t i = 0; i < ac->second.size(); ++i)
      {
        if (ac->second[i].first == Point)
        {
          Map = std::vector<double>(ac->second[i].second);
          bFound = true;
          break;
        }
      }
    }
    ac.release();
  }
  else
  {
    TShapeMapLegacy::iterator it = m_shapes_legacy[nIndex].find(Point);
    if (it != m_shapes_legacy[nIndex].end())
    {
      std::vector<double> &source = it->second;
      Map.reserve(source.size());
      for (size_t i = 0; i < source.size(); ++i)
        Map.push_back(source[i]);
      bFound = true;
    }
  }
	
	return bFound; 
}

void CCacheInterface::SetShapeMap(int nIndex, const geo::IPoint &Point, std::vector<double> &Map)
{
  if (CValueMapper::instance()->AllowParallelMapping())
  {
    size_t h = hash(Point);

    bool bFound = false;

    TShapeMap::accessor ac;
    m_shapes[nIndex].insert(ac, h);

    for (size_t i = 0; i < ac->second.size(); ++i)
    {
      if (ac->second[i].first == Point)
      {
        ac->second[i].second = std::vector<double>(Map);
        bFound = true;
        break;
      }
    }
    if (!bFound)
      ac->second.push_back(std::pair<geo::CPoint, std::vector<double> >(Point, std::vector<double>(Map)));
    ac.release();
  }
  else
  {
    m_shapes_legacy[nIndex][Point] = std::vector<double>(Map);
  }
}

geo::CElementCacheObject* CCacheInterface::ElementCacheObject(const geo::IElementSet *elt_set, const geo::IBody *body)
{
	TElementSetCacheObjects::iterator it = m_cache_objects.find(elt_set);
	if (it == m_cache_objects.end())
	{
		m_cache_objects[elt_set] = TElementCacheObjects();
		m_cache_objects[elt_set].resize(body->IndexingElementSet()->ElementSize());
	}
	return &m_cache_objects[elt_set][body->Index()];
}

void CCacheInterface::ElementCacheObject(const geo::IElementSet *elt_set, int nIndex, geo::CElementCacheObject &ElementCacheObject)
{
	ElementCacheObject.Valid(true);
	m_cache_objects[elt_set][nIndex] = ElementCacheObject;
}


void CCacheInterface::Invalidate()
{
	int size = m_min.size();

#ifdef _WIN32
  std::vector<geo::CPoint>(size).swap(m_min);
  std::vector<geo::CPoint>(size).swap(m_max);
  std::vector<geo::CElementPoint>(size).swap(m_mid);
  std::vector<TShapeMapLegacy>(size).swap(m_shapes_legacy);
  std::vector<TShapeMap>(size).swap(m_shapes);
#else
  for (int i = 0; i < size; ++i)
  {
    m_min[i] = geo::CPoint();
    m_max[i] = geo::CPoint();
    m_mid[i] = geo::CElementPoint();
    m_shapes_legacy[i] = TShapeMapLegacy();
    m_shapes[i] = TShapeMap();
  }
#endif

	m_points.clear();

	m_cache_objects.clear();
}

void CCacheInterface::InvalidateMappings(const geo::IElementSet *elt_set)
{
	int size = m_shapes_legacy.size();

#ifdef _WIN32
  std::vector<TShapeMapLegacy>(size).swap(m_shapes_legacy);
  std::vector<TShapeMap>(size).swap(m_shapes);
#else
  for (int i = 0; i < size; ++i)
  {
    m_shapes_legacy[i] = TShapeMapLegacy();
    m_shapes[i] = TShapeMap();
  }
#endif

	m_points.clear();

	TElementSetCacheObjects::iterator it = m_cache_objects.find(elt_set);
	if (it != m_cache_objects.end())
	{
		m_cache_objects.erase(it);
	}

}


bool CCacheInterface::Cached(const geo::IElementSet* elt_set)
{
  return m_cache_objects.count(elt_set) > 0;
}



