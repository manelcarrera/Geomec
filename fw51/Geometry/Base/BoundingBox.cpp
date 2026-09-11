#include "BoundingBox.h"
#include "Point.h"

namespace geo {

bool BBox::Intersects(const geo::IPoint &bb1_corner1, const geo::IPoint &bb1_corner2, const geo::IPoint &bb2_corner1,
                      const geo::IPoint &bb2_corner2, bool IncludeEdge) {
  geo::CPoint b1_max = bb1_corner1.Max(bb1_corner2);
  geo::CPoint b1_min = bb1_corner1.Min(bb1_corner2);
  geo::CPoint b2_max = bb2_corner1.Max(bb2_corner2);
  geo::CPoint b2_min = bb2_corner1.Min(bb2_corner2);

  if (b1_max.X() - b2_min.X() < -EPS)
    return false;
  if (b2_max.X() - b1_min.X() < -EPS)
    return false;
  if (b1_max.Y() - b2_min.Y() < -EPS)
    return false;
  if (b2_max.Y() - b1_min.Y() < -EPS)
    return false;
  if (b1_max.Z() - b2_min.Z() < -EPS)
    return false;
  if (b2_max.Z() - b1_min.Z() < -EPS)
    return false;

  if (fabs(b1_max.X() - b2_min.X()) < EPS)
    return IncludeEdge;
  if (fabs(b1_min.X() - b2_max.X()) < EPS)
    return IncludeEdge;
  if (fabs(b1_max.Y() - b2_min.Y()) < EPS)
    return IncludeEdge;
  if (fabs(b1_min.Y() - b2_max.Y()) < EPS)
    return IncludeEdge;
  if (fabs(b1_max.Z() - b2_min.Z()) < EPS)
    return IncludeEdge;
  if (fabs(b1_min.Z() - b2_max.Z()) < EPS)
    return IncludeEdge;

  return true;
}

void BBox::setBoundingBox(const geo::IPoint &point1, const geo::IPoint &point2, double extend, geo::IPoint &min,
                          geo::IPoint &max) {
  double min_x, min_y, min_z;
  double max_x, max_y, max_z;

  if (point1.X() < point2.X()) {
    min_x = point1.X() - extend;
    max_x = point2.X() + extend;
  } else {
    min_x = point2.X() - extend;
    max_x = point1.X() + extend;
  }

  if (point1.Y() < point2.Y()) {
    min_y = point1.Y() - extend;
    max_y = point2.Y() + extend;
  } else {
    min_y = point2.Y() - extend;
    max_y = point1.Y() + extend;
  }

  if (point1.Z() < point2.Z()) {
    min_z = point1.Z() - extend;
    max_z = point2.Z() + extend;
  } else {
    min_z = point2.Z() - extend;
    max_z = point1.Z() + extend;
  }

  min.Set(min_x, min_y, min_z);
  max.Set(max_x, max_y, max_z);
}

} // namespace geo
