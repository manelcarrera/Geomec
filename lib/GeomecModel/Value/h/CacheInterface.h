#if !defined(_C_CACHE_INTERFACE_H_)
#define _C_CACHE_INTERFACE_H_

#include "ICacheInterface.h"
#include "CacheManager.h"
#include "Point.h"
#include "ElementPoint.h"
#include "ElementCacheObject.h"

#include <vector>
#include <map>
#include <tbb/concurrent_hash_map.h>


class CCacheInterface : public geo::ICacheInterface
{
	CCacheManager *m_parent;
	const geo::IElementSet *m_elt_set;

	std::vector<geo::CPoint> m_min;
	std::vector<geo::CPoint> m_max;
	std::vector<geo::CElementPoint> m_mid;

  // we use this in non-experimental optimization mode
  typedef std::map<const geo::CPoint, std::vector<int> > TPointMapLegacy;
  TPointMapLegacy m_points_legacy;

  typedef std::pair<geo::CPoint, std::vector<int> > TPointVecIntPair;
  typedef tbb::concurrent_hash_map<size_t, std::vector<TPointVecIntPair> > TPointMap;
  TPointMap m_points;

  // we use this in non-experimental optimization mode
  typedef std::map<const geo::CPoint, std::vector<double> > TShapeMapLegacy;
	std::vector<TShapeMapLegacy> m_shapes_legacy;

  typedef std::pair<geo::CPoint, std::vector<double> > TPointVecDoublePair;
  typedef tbb::concurrent_hash_map<size_t, std::vector<TPointVecDoublePair> > TShapeMap;
  std::vector<TShapeMap> m_shapes;
  
	typedef std::vector<geo::CElementCacheObject> TElementCacheObjects;
	typedef std::map<const geo::IElementSet *, TElementCacheObjects> TElementSetCacheObjects;
	TElementSetCacheObjects m_cache_objects;


public:
	CCacheInterface(CCacheManager &parent, const geo::IElementSet *elt_set);

	virtual bool NeedsDelete();
	virtual void Resize(std::size_t size);

	virtual geo::CPoint Min(int nIndex);
	virtual void Min(int nIndex, geo::CPoint &Point);

	virtual geo::CPoint Max(int nIndex);
	virtual void Max(int nIndex, geo::CPoint &Point);

	virtual geo::CElementPoint Mid(int nIndex);
	virtual void Mid(int nIndex, geo::CElementPoint &ElementPoint);

	virtual bool PointMap(const geo::IPoint &Point, std::vector<int> &Map);
	virtual void SetPointMap(const geo::IPoint &Point, std::vector<int> &Map);

	virtual bool ShapeMap(int nIndex, const geo::IPoint &Point, std::vector<double> &Map);
	virtual void SetShapeMap(int nIndex, const geo::IPoint &Point, std::vector<double> &Map);

	virtual geo::CElementCacheObject* ElementCacheObject(const geo::IElementSet *elt_set, const geo::IBody *body);
	virtual void ElementCacheObject(const geo::IElementSet *elt_set, int nIndex, geo::CElementCacheObject &ElementCacheObject);

	virtual void Invalidate();
	virtual void InvalidateMappings(const geo::IElementSet *elt_set);

  virtual bool Cached(const geo::IElementSet* elt_set);

};


#endif
