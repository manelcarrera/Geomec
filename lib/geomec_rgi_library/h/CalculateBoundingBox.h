#ifndef _CalculateBoundingBox_h_
#define _CalculateBoundingBox_h_

#include <map>

#include "BodyTriangle.h"
#include "CalculateBoundingSurfaces.h"

namespace GeomecRGI {

struct TCompareCBodyTriangle {
  bool operator()(const geo::CBodyTriangle *lhs, const geo::CBodyTriangle *rhs) const { return *lhs < *rhs; }
};

typedef std::map<const geo::CBodyTriangle *, size_t, TCompareCBodyTriangle> TTriangleMap;
typedef std::vector<const geo::CBodyTriangle *> TTriangles;
typedef std::vector<TTriangles> TTriangleSurfaces;

struct TBoundingBox {
  double front;
  double back;
  double right;
  double left;
  double top;
  double bottom;
};

class CCalculateBoundingBox : public CCalculateBoundingSurfaces {
public:
  CCalculateBoundingBox(geo::IElementSet &elementSet);

  TTriangleSurfaces collectTriangleSurfaces();

private:
  CCalculateBoundingBox(const CCalculateBoundingBox &rhs);
  CCalculateBoundingBox &operator=(const CCalculateBoundingBox &rhs);

  TTriangleMap getTriangleMap() const;

  TBoundingBox m_BoundingBox;
};

} // namespace GeomecRGI

#endif // _CalculateBoundingBox_h_
