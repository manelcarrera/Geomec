#if !defined(_I_CACHE_INTERFACE_H_)
#define _I_CACHE_INTERFACE_H_

#include "ElementPoint.h"
#include "Point.h"
#include <vector>

namespace geo {

class IPoint;
class CPoint;
class CElementPoint;
class IElementSet;
class CElementCacheObject;

class GEOMETRY_EXPORT ICacheInterface {
protected:
  ICacheInterface() {}

public:
  virtual ~ICacheInterface() {}

  virtual bool NeedsDelete() = 0;
  virtual void Resize(std::size_t size) = 0;

  virtual CPoint Min(int nIndex) = 0;
  virtual void Min(int nIndex, CPoint &Point) = 0;

  virtual CPoint Max(int nIndex) = 0;
  virtual void Max(int nIndex, CPoint &Point) = 0;

  virtual CElementPoint Mid(int nIndex) = 0;
  virtual void Mid(int nIndex, CElementPoint &ElementPoint) = 0;

  virtual bool PointMap(const IPoint &Point, std::vector<int> &Map) = 0;
  virtual void SetPointMap(const IPoint &Point, std::vector<int> &Map) = 0;

  virtual bool ShapeMap(int nIndex, const IPoint &Point, std::vector<double> &Map) = 0;
  virtual void SetShapeMap(int nIndex, const IPoint &Point, std::vector<double> &Map) = 0;

  virtual CElementCacheObject *ElementCacheObject(const IElementSet *elt_set, const IBody *body) = 0;
  virtual void ElementCacheObject(const IElementSet *elt_set, int nIndex, CElementCacheObject &ElementCacheObject) = 0;

  virtual void Invalidate() = 0;

  virtual bool Cached(const IElementSet *elt_set) = 0;

  // typedef std::vector<ElementCacheObject> TElementMapping;
};

} // namespace geo

#endif // _I_CACHE_INTERFACE_H_
