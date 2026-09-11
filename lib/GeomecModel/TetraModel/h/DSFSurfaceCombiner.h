#pragma once

#include <vector>
#include <map>
#include <set>

#include "Point.h"
#include "Vector.h"
#include "Triangle.h"

namespace gm_dsf
{

struct IntermediateSurfaceInfo;


class DSFSurfaceCombiner
{
  const IntermediateSurfaceInfo& m_surf_src_0;
  const IntermediateSurfaceInfo& m_surf_src_1;

public:
  DSFSurfaceCombiner(const IntermediateSurfaceInfo& surf_src_0, const IntermediateSurfaceInfo& surf_src_1);

  void Generate(IntermediateSurfaceInfo& surf_dst, std::vector<IntermediateSurfaceInfo> *helpers = nullptr);

private:
  void getSurfaceEdges(const IntermediateSurfaceInfo& surf, std::set<std::pair<int, int> >& edges) const;
  void getOrderedSurfaceEdges(const IntermediateSurfaceInfo& surf, std::vector<geo::CPoint>& points, std::vector<std::pair<size_t, size_t> >& ordered_edges) const;

  bool createStitchedCopy(const IntermediateSurfaceInfo& surf, IntermediateSurfaceInfo& surf_cpy) const;

  struct PlaneInfo
  {
  geo::CPoint mid_point;
  geo::CVector avg_normal;
  double avg_area;
  double min_area;
  double max_area;

  geo::CPoint rot_mid_point;
  geo::CVector rot_axis;
  geo::CVector rot_translation;
  double rot_angle;
  geo::CVector rot_avg_normal;
  };
  
  void createRotatedCopy(const IntermediateSurfaceInfo& surf, const geo::CPoint& plane_mid, const geo::CVector& plane_normal, IntermediateSurfaceInfo& surf_cpy, PlaneInfo& plane_info) const;
  void createFlatCopy(const IntermediateSurfaceInfo& surf, IntermediateSurfaceInfo& surf_cpy) const;

  void getRotationInfo(const IntermediateSurfaceInfo& surf_0, const IntermediateSurfaceInfo& surf_1, geo::CPoint& rotation_mid, geo::CVector& rotation_normal) const;
  void getCorrectionSurface(const IntermediateSurfaceInfo& surf_rot_0, const IntermediateSurfaceInfo& surf_flat_0,
  const IntermediateSurfaceInfo& surf_rot_1, const IntermediateSurfaceInfo& surf_flat_1,
  IntermediateSurfaceInfo& surf_correction_0, bool direction) const;

  struct TriangleInfo
  {
  double x, y, z;
  double r2;

  const IntermediateSurfaceInfo& surf;
  int triangle;

  double SquaredDistance(const geo::IPoint& p) const;
  double Contains(const geo::IPoint& p) const;

  TriangleInfo(const IntermediateSurfaceInfo& surf, int triangle);
  };

  struct TyingInfo
  {
  int triangle;
  geo::CPoint point;

  TyingInfo() : triangle(-1) {}
  TyingInfo(int triangle, const geo::IPoint& p) : triangle(triangle) { point.Set(p.X(), p.Y(), p.Z()); }
  };

  typedef std::map<int, TyingInfo> TyingInfoMap;

  void getTyingInformation(TyingInfoMap& mpNodeInfo, const IntermediateSurfaceInfo& surf_src, const IntermediateSurfaceInfo& surf_dst, double eps = .1) const;

};

}
