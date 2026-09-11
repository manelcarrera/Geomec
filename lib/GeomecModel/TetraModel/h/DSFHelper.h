#pragma once

#include "Point.h"
#include <vector>

namespace gm_dsf {

struct IntermediateSurfaceInfo {
  std::vector<geo::CPoint> points;
  std::vector<std::vector<int>> triangles;

  IntermediateSurfaceInfo &operator=(const IntermediateSurfaceInfo &rhs);
};

} // namespace gm_dsf
