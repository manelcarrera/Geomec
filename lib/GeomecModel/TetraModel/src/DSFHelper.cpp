#include "DSFHelper.h"

namespace gm_dsf {

IntermediateSurfaceInfo &IntermediateSurfaceInfo::operator=(const IntermediateSurfaceInfo &rhs) {
  points.resize(rhs.points.size());

  for (size_t i = 0; i < points.size(); ++i) {
    points[i].Set(rhs.points[i].X(), rhs.points[i].Y(), rhs.points[i].Z());
  }

  triangles.resize(rhs.triangles.size());

  for (size_t i = 0; i < triangles.size(); ++i) {
    triangles[i].resize(3);
    std::copy(rhs.triangles[i].begin(), rhs.triangles[i].end(), triangles[i].begin());
  }

  return *this;
}

} // end namespace gm_dsf
