#pragma once

#include <vector>
#include "Point.h"


namespace gm_dsf
{

struct IntermediateSurfaceInfo
{
  std::vector<geo::CPoint> points;
  std::vector<std::vector<int> > triangles;

  IntermediateSurfaceInfo& operator=(const IntermediateSurfaceInfo& rhs);
};

}

