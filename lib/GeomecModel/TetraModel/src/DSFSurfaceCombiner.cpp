#include "DSFSurfaceCombiner.h"
#include "DSFHelper.h"
#include "Triangle.h"
#include "Line.h"
#include "Plane.h"
#include "Matrix.h"
#include "TriangleSurface.h"

namespace gm_dsf
{

// helper functions

// This is not a generic general closeness operator; it works because we expect our sparse map to contain points
// that are either close (duplicates) or far away. If we had a random point cloud, this operator would break
// strict weak ordering. (Also breaks if we have 3 duplicates where two of them are further apart than 2.)
// Reiterating: THIS WILL NOT WORK ON A SURFACE MESH WITH TINY TRIANGLES!
//
// TODO: replace with a genuine nearest point algorithm, but I don't like OUR nearest point algorithm/
struct PointLess
{
  bool operator()(const geo::CPoint& lhs, const geo::CPoint& rhs) const
  {
  if (lhs.Distance(rhs) < 3)
      return false;

  return lhs < rhs;
  }
};



DSFSurfaceCombiner::DSFSurfaceCombiner(const IntermediateSurfaceInfo& surf_src_0, const IntermediateSurfaceInfo& surf_src_1)
  : m_surf_src_0(surf_src_0)
  , m_surf_src_1(surf_src_1)
{
}



void DSFSurfaceCombiner::Generate(IntermediateSurfaceInfo& surf_dst, std::vector<IntermediateSurfaceInfo> *helpers)
{
  // Create stitched copies of the input surfaces
  IntermediateSurfaceInfo surf_0, surf_1;
  
  bool stitched_0 = createStitchedCopy(m_surf_src_0, surf_0);
  bool stitched_1 = createStitchedCopy(m_surf_src_1, surf_1);

  // Get rotation normal and mid point
  geo::CPoint rotation_mid;
  geo::CVector rotation_normal;
  getRotationInfo(surf_0, surf_1, rotation_mid, rotation_normal);

  // And rotate (+translate) them to the XY plane (Z = 0-ish)
  IntermediateSurfaceInfo surf_0_rot, surf_1_rot;
  PlaneInfo plane_info_0, plane_info_1;

  createRotatedCopy(surf_0, rotation_mid, rotation_normal, surf_0_rot, plane_info_0);
  createRotatedCopy(surf_1, rotation_mid, rotation_normal, surf_1_rot, plane_info_1);

  // Create flat copies of the rotated surfaces
  IntermediateSurfaceInfo surf_0_flat, surf_1_flat;
  createFlatCopy(surf_0_rot, surf_0_flat);
  createFlatCopy(surf_1_rot, surf_1_flat);

  // Now get a global correction factor to get a plane in the middle


  // Select one of the surfaces as input for the mesher
  IntermediateSurfaceInfo *surf_rot_src = nullptr;
  IntermediateSurfaceInfo *surf_rot_other = nullptr;
  IntermediateSurfaceInfo *surf_flat_src = nullptr;
  IntermediateSurfaceInfo *surf_flat_other = nullptr;
  PlaneInfo *plane_info_src = nullptr;
  PlaneInfo *plane_info_other = nullptr;

  if (stitched_0 && !stitched_1)
  {
  surf_rot_src = &surf_1_rot;
  surf_flat_src = &surf_1_flat;
  plane_info_src = &plane_info_1;
  surf_rot_other = &surf_0_rot;
  surf_flat_other = &surf_0_flat;
  plane_info_other = &plane_info_0;
  }
  else if (!stitched_0 && stitched_1)
  {
  surf_rot_src = &surf_0_rot;
  surf_flat_src = &surf_0_flat;
  plane_info_src = &plane_info_0;
  surf_rot_other = &surf_1_rot;
  surf_flat_other = &surf_1_flat;
  plane_info_other = &plane_info_1;
  }
  else if (plane_info_0.max_area > plane_info_1.max_area)
  {
  surf_rot_src = &surf_0_rot;
  surf_flat_src = &surf_0_flat;
  plane_info_src = &plane_info_0;
  surf_rot_other = &surf_1_rot;
  surf_flat_other = &surf_1_flat;
  plane_info_other = &plane_info_1;
  }
  else
  {
  surf_rot_src = &surf_1_rot;
  surf_flat_src = &surf_1_flat;
  plane_info_src = &plane_info_1;
  surf_rot_other = &surf_0_rot;
  surf_flat_other = &surf_0_flat;
  plane_info_other = &plane_info_0;
  }

  double optimum_size = (((plane_info_0.min_area + plane_info_0.max_area + plane_info_1.min_area + plane_info_1.max_area) / 4) + plane_info_0.avg_area + plane_info_1.avg_area) / 3;

  // And get outer edges of surface as input for mesher (and an identity vector with indices as extra input)
  std::vector<geo::CPoint> points;
  std::vector<std::pair<size_t, size_t> > segments;

  getOrderedSurfaceEdges(*surf_flat_src, points, segments);

  std::vector<int> segment_indices(segments.size());

  for (int i = 0; i < segment_indices.size(); ++i)
  segment_indices[i] = i;

  // Now mesh
  CTriangleSurface surf;

  surf.Points(points);
  surf.Segments(segments);
  surf.NewRegion(segment_indices);

  surf.CreateMesh(20, std::ceil(optimum_size));

  // Gather the meshed surface
  IntermediateSurfaceInfo mesh_result;
  mesh_result.points.reserve(surf.PointSize());
  mesh_result.triangles.reserve(surf.FaceSize());

  for (int i = 0; i < surf.PointSize(); ++i)
  {
  mesh_result.points.push_back(geo::CPoint(surf.Point(i).X(), surf.Point(i).Y(), 0));

  }
  for (int i = 0; i < surf.FaceSize(); ++i)
  {
  std::vector<int> t(3, 0);
  for (int j = 0; j < 3; ++j)
      t[j] = surf.Triangle(i).PointIndex(j);
  mesh_result.triangles.push_back(t);
  }

  // Project it on the flat surface (which is point-tied to the possibly curved rotated surface)
  TyingInfoMap projectBack;
  getTyingInformation(projectBack, mesh_result, *surf_flat_src, 0.000001);

  // Get correction information
  geo::CPoint correction_mid;
  geo::CVector correction_normal;
  IntermediateSurfaceInfo correction_src, correction_other;

  bool direction = plane_info_src->rot_mid_point < plane_info_other->rot_mid_point;

  getCorrectionSurface(*surf_rot_src, *surf_flat_src, *surf_rot_other, *surf_flat_other, correction_src, direction);
  getCorrectionSurface(*surf_rot_other, *surf_flat_other, *surf_rot_src, *surf_flat_src, correction_other, !direction);

  getRotationInfo(correction_src, correction_other, correction_mid, correction_normal);


  // Map new mesh surface back to the correct place
  for (int i = 0; i < mesh_result.points.size(); ++i)
  {
  geo::CPoint& p = mesh_result.points[i];

  // correction
  geo::CVector correction(correction_mid - p);
  double correction_dot = correction.DotProduct(correction_normal);

  p.Z((correction_normal * correction_dot).Z());

  // map flat surface back to original shape
  TyingInfoMap::iterator it = projectBack.find(i);
  if (it != projectBack.end())
  {
      const TyingInfo tying_info = it->second;
      const std::vector<geo::CPoint>& points_rot = surf_rot_src->points;
      const std::vector<int>& tri = surf_flat_src->triangles[tying_info.triangle];

      geo::CTriangle triangle(points_rot[tri[0]], points_rot[tri[1]], points_rot[tri[2]]);

      geo::CPlane plane(triangle.Line(0).First(), triangle.Normal());

      geo::CPoint pt0(p.X(), p.Y(), -1000);
      geo::CPoint pt1(p.X(), p.Y(), 10000);

      geo::CLine line(pt0, pt1);

      geo::CPoint inter = plane.Intersection(line);

      if (!inter.Empty())
    p.Z(p.Z() + inter.Z());
  }

  // rotate and translate back to original position
  p.Rotate(plane_info_src->rot_axis, -plane_info_src->rot_angle);
  p += plane_info_src->rot_translation;
  }

  surf_dst.points.swap(mesh_result.points);
  surf_dst.triangles.swap(mesh_result.triangles);

  if (helpers)
  {
  helpers->reserve(8);
  helpers->push_back(surf_0);
  helpers->push_back(surf_0_rot);
  helpers->push_back(surf_0_flat);
  helpers->push_back(surf_rot_src == &surf_0_rot ? correction_src : correction_other);
  helpers->push_back(surf_1);
  helpers->push_back(surf_1_rot);
  helpers->push_back(surf_1_flat);
  helpers->push_back(surf_rot_src == &surf_0_rot ? correction_other : correction_src);
  }

#ifdef SHOW_EDGES

  surf_dst.points.clear();
  surf_dst.triangles.clear();

  surf_dst.points.resize(points.size());
  std::copy(points.begin(), points.end(), surf_dst.points.begin());
  int mid = surf_dst.points.size();
  surf_dst.points.push_back(plane_info_src->mid_point);
  surf_dst.points.back().Z(0);

  surf_dst.triangles.reserve(segments.size());

  for (int i = 0; i < segments.size(); ++i)
  {
  surf_dst.triangles.push_back(std::vector<int>{ (int)segments[i].first, (int)segments[i].second, mid });
  }

#endif

}


void DSFSurfaceCombiner::getSurfaceEdges(const IntermediateSurfaceInfo& surf, std::set<std::pair<int, int> >& edges) const
{
  typedef std::pair<int, int> TEdge;
  typedef std::set<int> TOppositeNodes;
  typedef std::map<TEdge, TOppositeNodes> TEdgeMap;
  TEdgeMap edge_map;

  // collect all edges of triangles, mapping them to their opposite node of the triangle
  for (int i = 0; i < surf.triangles.size(); ++i)
  {
  std::vector<int> tri = surf.triangles[i];

  std::sort(tri.begin(), tri.end());

  TEdgeMap::iterator it = edge_map.insert(std::make_pair(std::make_pair(tri[0], tri[1]), std::set<int>())).first;
  it->second.insert(tri[2]);

  it = edge_map.insert(std::make_pair(std::make_pair(tri[0], tri[2]), std::set<int>())).first;
  it->second.insert(tri[1]);

  it = edge_map.insert(std::make_pair(std::make_pair(tri[1], tri[2]), std::set<int>())).first;
  it->second.insert(tri[0]);
  }

  typedef std::set<TEdge> TEdges;
  typedef std::map<int, TEdges> TNodes2Edges;
  typedef std::pair<TNodes2Edges::iterator, bool> TNodes2EdgesRetval;
  TNodes2Edges nodes2edges;

  // collect all edges with only one triangle attached: surface edges
  for (TEdgeMap::const_iterator it = edge_map.begin(); it != edge_map.end(); ++it)
  {
  if (it->second.size() == 1)
  {
      edges.insert(it->first);
  }
  }
}

void DSFSurfaceCombiner::getOrderedSurfaceEdges(const IntermediateSurfaceInfo & surf, std::vector<geo::CPoint>& points, std::vector<std::pair<size_t, size_t>>& ordered_edges) const
{
  std::set<std::pair<int, int> > surf_edges;
  getSurfaceEdges(surf, surf_edges);

  points.reserve(surf_edges.size());
  ordered_edges.reserve(surf_edges.size());

  // create a map from a node to its two neighbors (assuming all edges connect)
  std::map<int, std::pair<int, int> > node2neighbors;

  for (std::set<std::pair<int, int> >::iterator it = surf_edges.begin(); it != surf_edges.end(); ++it)
  {
  std::pair<std::map<int, std::pair<int, int> >::iterator, bool> retval = node2neighbors.insert(std::make_pair(it->first, std::pair<int, int>()));
  if (retval.second)
      retval.first->second.first = it->second;
  else
      retval.first->second.second = it->second;

  retval = node2neighbors.insert(std::make_pair(it->second, std::pair<int, int>()));
  if (retval.second)
      retval.first->second.first = it->first;
  else
      retval.first->second.second = it->first;
  }


  const std::pair<int, int> first_edge = *surf_edges.begin();

  ordered_edges.push_back(first_edge);

  while (ordered_edges.size() < surf_edges.size())
  {
  std::map<int, std::pair<int, int> >::iterator it = node2neighbors.find(ordered_edges.back().second);
  if (it == node2neighbors.end())
  {
      break;
  }
  int next = (it->second.first == ordered_edges.back().first) ? it->second.second : it->second.first;

  ordered_edges.push_back(std::make_pair(ordered_edges.back().second, next));
  }

  points.push_back(surf.points[ordered_edges[0].first]);

  for (int i = 0; i < ordered_edges.size() - 1; ++i)
  {
  points.push_back(surf.points[ordered_edges[i].second]);
  ordered_edges[i].first = points.size() - 2;
  ordered_edges[i].second = points.size() - 1;
  }

  ordered_edges.back().first = points.size() - 1;
  ordered_edges.back().second = 0;
}



// An input surface can have a thin hole in it wherever another double-sided fault intersects with it.
// This hole can go all the way up to the outer edge, dividing our input surface into multiple ones.
// Here we want to get rid of these gaps on the outer edge. (If possible, we will also get rid of
// the hole altogether, but that can only be done if this hole has nodes in the same places on either
// side of the hole; if not, the hole should be filled with triangles, but as these are small, we don't
// want that.)
bool DSFSurfaceCombiner::createStitchedCopy(const IntermediateSurfaceInfo& surf, IntermediateSurfaceInfo& surf_cpy) const
{
  typedef std::pair<int, int> TEdge;
  typedef std::set<int> TOppositeNodes;
  typedef std::map<TEdge, TOppositeNodes> TEdgeMap;
  TEdgeMap edge_map;

  // collect all edges of triangles, mapping them to their opposite node of the triangle
  for (int i = 0; i < surf.triangles.size(); ++i)
  {
  std::vector<int> tri = surf.triangles[i];

  std::sort(tri.begin(), tri.end());

  TEdgeMap::iterator it = edge_map.insert(std::make_pair(std::make_pair(tri[0], tri[1]), std::set<int>())).first;
  it->second.insert(tri[2]);

  it = edge_map.insert(std::make_pair(std::make_pair(tri[0], tri[2]), std::set<int>())).first;
  it->second.insert(tri[1]);

  it = edge_map.insert(std::make_pair(std::make_pair(tri[1], tri[2]), std::set<int>())).first;
  it->second.insert(tri[0]);
  }

  typedef std::set<TEdge> TEdges;
  typedef std::map<int, TEdges> TNodes2Edges;
  typedef std::pair<TNodes2Edges::iterator, bool> TNodes2EdgesRetval;
  TNodes2Edges nodes2edges;

  // collect all edges with only one triangle attached: surface edges
  for (TEdgeMap::const_iterator it = edge_map.begin(); it != edge_map.end(); ++it)
  {
  if (it->second.size() == 1)
  {
      TNodes2EdgesRetval retval = nodes2edges.insert(std::make_pair(it->first.first, TEdges()));
      retval.first->second.insert(it->first);

      retval = nodes2edges.insert(std::make_pair(it->first.second, TEdges()));
      retval.first->second.insert(it->first);
  }
  }

  // copy points and triangles; triangles to final destination, points to intermediate
  std::vector<geo::CPoint> new_points(surf.points);

  surf_cpy.triangles.resize(surf.triangles.size());
  std::copy(surf.triangles.begin(), surf.triangles.end(), surf_cpy.triangles.begin());


  typedef std::set<int> TNodes;
  typedef std::map<geo::CPoint, TNodes, PointLess> TPoint2Nodes;
  typedef std::pair<TPoint2Nodes::iterator, bool> TPoint2NodesRetval;
  TPoint2Nodes point2nodes;

  // collect all surface edge nodes that have duplicates on edges close by (#nodes > 1)
  for (TNodes2Edges::const_iterator it = nodes2edges.begin(); it != nodes2edges.end(); ++it)
  {
  TPoint2NodesRetval retval = point2nodes.insert(std::make_pair(surf.points[it->first], TNodes()));
  retval.first->second.insert(it->first);
  }

  std::vector<bool> use(surf.points.size(), true);
  std::map<int, int> map_duplicates;

  bool do_reindexing = false;

  geo::CPoint point;

  for (TPoint2Nodes::const_iterator it = point2nodes.begin(); it != point2nodes.end(); ++it)
  {
  if (it->second.size() > 1) // we expect 2, but who knows?
  {
      int keep = *it->second.begin();
      point.Set(new_points[keep].X(), new_points[keep].Y(), new_points[keep].Z());
      TNodes::iterator jt = ++it->second.begin();

      int processed = 1;

      for (; jt != it->second.end(); ++jt)
      {
    TEdge edge(keep, *jt); if (edge.first > edge.second) std::swap(edge.first, edge.second);
    TEdgeMap::iterator kt = edge_map.find(edge);

    if (kt == edge_map.end())
    {
          do_reindexing = true;
          use[*jt] = false;
          map_duplicates[*jt] = keep;
          point.Set(point.X() + new_points[*jt].X(), point.Y() + new_points[*jt].Y(), point.Z() + new_points[*jt].Z());
          ++processed;
    }
      }
      if (processed > 1)
    new_points[keep].Set(point.X() / processed, point.Y() / processed, point.Z() / processed); // not really necessary, but this way the non-duplicate node lies in the middle
  }
  }


  if (do_reindexing)
  {
  std::vector<int> dropped(surf.points.size() + 1, 0);
  std::vector<int> re_index(surf.points.size());
  surf_cpy.points.reserve(surf.points.size());

  // re-index points
  for (int i = 0; i < re_index.size(); ++i)
      re_index[i] = i;

  for (int i = 0; i < re_index.size(); ++i)
  {
      dropped[i + 1] = dropped[i];

      if (use[i])
      {
    surf_cpy.points.push_back(new_points[i]);
    re_index[i] = i - dropped[i];
      }
      else
      {
    ++dropped[i + 1];
    re_index[i] = map_duplicates[i];
      }
  }

  // re-index triangles
  for (int i = 0; i < surf_cpy.triangles.size(); ++i)
  {
      for (int j = 0; j < 3; ++j)
      {
    surf_cpy.triangles[i][j] = re_index[surf_cpy.triangles[i][j]];
      }
  }
  }
  else // no re-indexing, just use copied points
  {
  surf_cpy.points.swap(new_points);
  }

  return do_reindexing;
}

// Rotate the surface to the XY plane, and translate it to Z = zero.
// NOT the whole surface will be at zero, depending on how flat it is!
// We also gather some stuff that might be interesting, like average normal (pre-rotated), area (avg/min/max) etc.
void DSFSurfaceCombiner::createRotatedCopy(const IntermediateSurfaceInfo& surf, const geo::CPoint& plane_mid, const geo::CVector& plane_normal, IntermediateSurfaceInfo& surf_cpy, PlaneInfo& plane_info) const
{
  plane_info.mid_point.Set(0, 0, 0);
  for (int i = 0; i < surf.points.size(); ++i)
  {
  plane_info.mid_point += surf.points[i];
  }

  plane_info.mid_point = plane_info.mid_point / surf.points.size();

  plane_info.avg_normal.Set(0, 0, 0);
  plane_info.avg_area = 0;
  plane_info.min_area = DBL_MAX;
  plane_info.max_area = -DBL_MAX;

  for (int i = 0; i < surf.triangles.size(); ++i)
  {
  const std::vector<int>& face = surf.triangles[i];
  geo::CTriangle tri(surf.points[face[0]], surf.points[face[1]], surf.points[face[2]]);
  plane_info.avg_normal += tri.Normal();
  double area = tri.Area();
  plane_info.avg_area += area;
  if (area < plane_info.min_area) plane_info.min_area = area;
  if (area > plane_info.max_area) plane_info.max_area = area;
  }

  plane_info.avg_normal = plane_info.avg_normal / surf.triangles.size();
  plane_info.avg_area /= surf.triangles.size();

  double cos_phi = plane_normal.Z() / plane_normal.Length();

  plane_info.rot_angle = acos(cos_phi) * 180 / M_PI;
  plane_info.rot_axis.Set(plane_normal.Y(), -plane_normal.X(), 0);

  surf_cpy = surf;

  plane_info.rot_mid_point.Set(0, 0, 0);

  plane_info.rot_translation = plane_mid;

  for (int i = 0; i < surf_cpy.points.size(); ++i)
  {
  surf_cpy.points[i] = surf_cpy.points[i] - plane_info.rot_translation;
  plane_info.rot_mid_point += surf_cpy.points[i];
  }

  for (int i = 0; i < surf_cpy.points.size(); ++i)
  {
  surf_cpy.points[i].Rotate(plane_info.rot_axis, plane_info.rot_angle);
  }

  plane_info.rot_mid_point = plane_info.rot_mid_point / surf_cpy.points.size();

  plane_info.rot_avg_normal.Set(0, 0, 0);
  for (int i = 0; i < surf_cpy.triangles.size(); ++i)
  {
  const std::vector<int>& face = surf_cpy.triangles[i];
  geo::CTriangle tri(surf_cpy.points[face[0]], surf_cpy.points[face[1]], surf_cpy.points[face[2]]);
  plane_info.rot_avg_normal += tri.Normal();
  }
  plane_info.rot_avg_normal = plane_info.rot_avg_normal / surf_cpy.triangles.size();
}

void DSFSurfaceCombiner::createFlatCopy(const IntermediateSurfaceInfo& surf, IntermediateSurfaceInfo& surf_cpy) const
{
  surf_cpy = surf;

  for (int i = 0; i < surf_cpy.points.size(); ++i)
  surf_cpy.points[i].Z(0);
}

// Calculate an optimal plane through all the points, by taking the smallest eigen value's vector as normal,
// and return the corresponding mid point and normal.
void DSFSurfaceCombiner::getRotationInfo(const IntermediateSurfaceInfo& surf_0, const IntermediateSurfaceInfo& surf_1, geo::CPoint& rotation_mid, geo::CVector& rotation_normal) const
{
  geo::CMatrix points(3, surf_0.points.size() + surf_1.points.size());

  int size_0 = surf_0.points.size();
  int size_1 = surf_1.points.size();

  geo::CPoint mid(0, 0, 0);

  for (int i = 0; i < size_0; ++i)
  mid += surf_0.points[i];
  for (int i = 0; i < size_1; ++i)
  mid += surf_1.points[i];

  mid = mid / (size_0 + size_1);

  for (int i = 0; i < size_0; ++i)
  {
  points.Value(0, i, surf_0.points[i].X() - mid.X());
  points.Value(1, i, surf_0.points[i].Y() - mid.Y());
  points.Value(2, i, surf_0.points[i].Z() - mid.Z());
  }
  for (int i = 0; i < size_1; ++i)
  {
  points.Value(0, i + size_0, surf_1.points[i].X() - mid.X());
  points.Value(1, i + size_0, surf_1.points[i].Y() - mid.Y());
  points.Value(2, i + size_0, surf_1.points[i].Z() - mid.Z());
  }

  geo::CMatrix matrix_3x3 = points * points.GetTransposed();
  geo::CMatrixPair pair = matrix_3x3.GetEigenPairs(true);

  rotation_mid.Set(mid.X(), mid.Y(), mid.Z());
  rotation_normal.Set(pair.second.Value(0, 0), pair.second.Value(1, 0), pair.second.Value(2, 0));
}

void DSFSurfaceCombiner::getCorrectionSurface(const IntermediateSurfaceInfo& surf_rot_0, const IntermediateSurfaceInfo& surf_flat_0,
  const IntermediateSurfaceInfo& surf_rot_1, const IntermediateSurfaceInfo& surf_flat_1,
  IntermediateSurfaceInfo& surf_correction_0, bool direction) const
{
  TyingInfoMap correction_map_0;
  getTyingInformation(correction_map_0, surf_flat_0, surf_flat_1, 0.000001);

  surf_correction_0 = surf_rot_0;

  int sign = direction ? 1 : -1;

  for (int i = 0; i < surf_rot_0.points.size(); ++i)
  {
  const geo::CPoint& p = surf_rot_0.points[i];

  double z = 0;

  TyingInfoMap::iterator it = correction_map_0.find(i);
  if (it != correction_map_0.end())
  {
      const TyingInfo tying_info = it->second;

      if (tying_info.triangle >= 0)
      {
    const std::vector<geo::CPoint>& points_rot = surf_rot_1.points;
    const std::vector<int>& tri = surf_rot_1.triangles[tying_info.triangle];

    geo::CTriangle triangle(points_rot[tri[0]], points_rot[tri[1]], points_rot[tri[2]]);

    geo::CPlane plane(triangle.Line(0).First(), triangle.Normal());

    geo::CPoint pt0(p.X(), p.Y(), -1000);
    geo::CPoint pt1(p.X(), p.Y(), 10000);

    geo::CLine line(pt0, pt1);

    geo::CPoint inter = plane.Intersection(line);

    if (!inter.Empty())
    {
          z = sign * (surf_rot_0.points[i].Z() - inter.Z()) / 2;
    }
      }
  }

  surf_correction_0.points[i].Z(z);
  }
}




double DSFSurfaceCombiner::TriangleInfo::SquaredDistance(const geo::IPoint& p) const
{
  return (p.X() - x) * (p.X() - x) + (p.Y() - y) * (p.Y() - y) + (p.Z() - z) * (p.Z() - z);
}

// Determines whether a point lies more or less inside a triangle
// Coming from SKUA/Gocad, this can be a bit imprecise
// First we detect if the point is inside a circle of 1.3 * radius
// Then we move the point SignedDistance closer along the normal, and 20% closer to the midpoint
double DSFSurfaceCombiner::TriangleInfo::Contains(const geo::IPoint& p) const
{
  if (SquaredDistance(p) < r2)
  {
  const std::vector<geo::CPoint>& pts = surf.points;
  const std::vector<int>& tri = surf.triangles[triangle];

  geo::CTriangle face(pts[tri[0]], pts[tri[1]], pts[tri[2]]);

  geo::CVector normal = face.Normal();
  double signedDist = face.SignedDistance(p, true);

  geo::CPoint trans(p.X() - signedDist * normal.X(), p.Y() - signedDist * normal.Y(), p.Z() - signedDist * normal.Z());

  geo::CVector v(trans, face.MidPoint());
  //v = v * 0.2;

  trans.X(trans.X() + v.X());
  trans.Y(trans.Y() + v.Y());
  trans.Z(trans.Z() + v.Z());

  // see IFace's Contains
  double angle = 0;

  for (int i = 0; i < 3; ++i)
  {
      geo::CVector v1(trans, face.Point(i));
      geo::CVector v2(trans, face.Point((i + 1) % 3));

      angle += v1.AngleRad(v2);
  }

  angle = fabs(fabs(angle) - TWOPI);

  return angle < 0.1;
  }
  return false;
}

DSFSurfaceCombiner::TriangleInfo::TriangleInfo(const IntermediateSurfaceInfo& surf, int triangle)
  : surf(surf)
  , triangle(triangle)
  , x(0), y(0), z(), r2(0)
{
  const std::vector<geo::CPoint>& pts = surf.points;
  const std::vector<int>& tri = surf.triangles[triangle];

  for (int i = 0; i < 3; ++i)
  {
  x += pts[tri[i]].X();
  y += pts[tri[i]].Y();
  z += pts[tri[i]].Z();
  }

  x /= 3;
  y /= 3;
  z /= 3;

  for (int i = 0; i < 3; ++i)
  {
  double r2candidate = SquaredDistance(pts[tri[i]]) * 1.3;

  if (r2candidate > r2)
      r2 = r2candidate;
  }
}

void DSFSurfaceCombiner::getTyingInformation(TyingInfoMap& mpNodeInfo, const IntermediateSurfaceInfo& surf_src_input, const IntermediateSurfaceInfo& surf_dst_input, double eps) const
{
  double min_eps = -eps;
  double max_eps = 1 + eps;

  // retrieve triangles from destination surface
  std::vector<TriangleInfo> vcTriangles;
  vcTriangles.reserve(surf_dst_input.triangles.size());

  for (int i = 0; i < surf_dst_input.triangles.size(); ++i)
  {
  vcTriangles.push_back(TriangleInfo(surf_dst_input, i));
  }

  typedef std::map<int, std::vector<int> > TNode2Faces;
  TNode2Faces node2Faces;

  std::vector<geo::CVector> normals;

  for (int i = 0; i < surf_src_input.triangles.size(); ++i)
  {
  const std::vector<int>& face = surf_src_input.triangles[i];

  for (int m = 0; m < 3; ++m)
  {
      const geo::IPoint& point = surf_src_input.points[face[m]];

      std::pair<TyingInfoMap::iterator, bool> retval = mpNodeInfo.insert(std::make_pair(face[m], TyingInfo()));
      if (retval.second)
      {
    // find triangle
    for (int n = 0; n < vcTriangles.size(); ++n)
    {
          if (vcTriangles[n].Contains(point))
          {
      const std::vector<geo::CPoint>& pts = surf_dst_input.points;
      const std::vector<int>& tri = surf_dst_input.triangles[n];

      geo::CTriangle face1(pts[tri[0]], pts[tri[1]], pts[tri[2]]);

      if (face1.Contains(point, true))
      {
              retval.first->second = TyingInfo(n, point);
              break;
      }
          }
    }
      }
  }
  }
}

} // end namespace gm_dsf
