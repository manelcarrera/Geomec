#pragma once

#include "GeometryExports.h"

namespace geo {

class IPoint;

class GEOMETRY_EXPORT BBox {
public:
  static bool Intersects(const geo::IPoint &bb1_corner1, const geo::IPoint &bb1_corner2, const geo::IPoint &bb2_corner1,
                         const geo::IPoint &bb2_corner2, bool IncludeEdge);

  static void setBoundingBox(const geo::IPoint &point1, const geo::IPoint &point2, double extend, geo::IPoint &min,
                             geo::IPoint &max);
};

} // namespace geo
