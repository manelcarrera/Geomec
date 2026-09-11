#include "DSFIntermediateSurfaceGenerator.h"

#include "TetraMesh.h"
#include "IMesh.h"
#include "IInterfaceElement.h"
#include "TetraModel.h"
#include "TetraSubHorizon.h"
#include "BoundaryBase.h"
#include "TetraSurface.h"
#include "RotatedSystem.h"
#include "GeoSurface.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "Triangle.h"
#include "BodyTriangle.h"
#include "TiedBodyTriangle.h"
#include "TiedPointBodyTriangle.h"
#include "TSSurface.h"
#include "TSSurfaceProgress.h"
#include "DSFSurfaceCombiner.h"
#include "DSFHelper.h"


//#define INTERMEDIATE_SURFACE_DEBUG


namespace gm_dsf
{

DSFIntermediateSurfaceGenerator::DSFIntermediateSurfaceGenerator(CTetraMesh& tetramesh)
  : m_mesh(tetramesh)
{
}

CTetraModel& DSFIntermediateSurfaceGenerator::Model()
{
  return static_cast<CTetraModel&>(m_mesh.Model());
}

const CTetraModel& DSFIntermediateSurfaceGenerator::Model() const
{
  return static_cast<const CTetraModel&>(m_mesh.Model());
}

geo::CTetMeshBase& DSFIntermediateSurfaceGenerator::TetMesh()
{
  return static_cast<geo::CTetMeshBase&>(m_mesh.Mesh());
}

const geo::CTetMeshBase& DSFIntermediateSurfaceGenerator::TetMesh() const
{
  return static_cast<const geo::CTetMeshBase&>(m_mesh.Mesh());
}


bool DSFIntermediateSurfaceGenerator::GenerateIntermediateSurface(const QString& name, const TSurface& surf_0, const TSurface& surf_1, bool precise)
{
  // Minus fault
  TNodeInfoMap mpMinus;
  IntermediateSurfaceInfo intsurfMinus;

  getTyingInformation(mpMinus, surf_0, surf_1, precise);
  getIntermediateSurfaceInfo(intsurfMinus, mpMinus, surf_0);

  // Plus fault
  TNodeInfoMap mpPlus;
  IntermediateSurfaceInfo intsurfPlus;

  getTyingInformation(mpPlus, surf_1, surf_0, precise);
  getIntermediateSurfaceInfo(intsurfPlus, mpPlus, surf_1);


  // Generate (or select) intermediate fault
  IntermediateSurfaceInfo surf_selected;
#ifdef INTERMEDIATE_SURFACE_DEBUG
  selectIntermediateSurfaceInfo(name, intsurfMinus, intsurfPlus, surf_selected, precise);
#else
  selectIntermediateSurfaceInfo(intsurfMinus, intsurfPlus, surf_selected, precise);
#endif


  // Add stuff to the model
  CTSSurfaceProgress SurfaceProgress;
  CTSSurface tempSurf(surf_selected.points, surf_selected.triangles, SurfaceProgress);

  geo::CSurfaceDesc *pSurfaceDesc = createSurface(name, surf_selected, tempSurf);

  // TODO: find a better way to deal with the interface for getTyingInformation
  TSurface intermediate_surface(tempSurf.FaceSize());

  for (int i = 0; i < tempSurf.FaceSize(); ++i)
    intermediate_surface[i] = &tempSurf.Face(i);

  // here we create the actual tying info from our new surface back to the original ones (same routine, different direction)
  mpMinus.clear();
  mpPlus.clear();

  getTyingInformation(mpMinus, intermediate_surface, surf_0, precise);
  getTyingInformation(mpPlus, intermediate_surface, surf_1, precise);

  // duplicate the nodes, create interface elements and tie them.
  duplicateNodes(mpMinus, pSurfaceDesc->Surface());
  duplicateNodes(mpPlus, pSurfaceDesc->Surface());

  InterfaceInfo if_info;
  createIFElements(mpMinus, mpPlus, intermediate_surface, *pSurfaceDesc, if_info);
#ifdef SKUA_NEW
  createResultTyings(if_info, surf_0, surf_1);
#endif

  createTyings(mpMinus);
  createTyings(mpPlus);

  return true;
}




double DSFIntermediateSurfaceGenerator::TriangleInfo::SquaredDistance(const geo::IPoint& p) const
{
  return (p.X() - x) * (p.X() - x) + (p.Y() - y) * (p.Y() - y) + (p.Z() - z) * (p.Z() - z);
}

// Determines whether a point lies more or less inside a triangle
// Coming from SKUA/Gocad, this can be a bit imprecise
// First we detect if the point is inside a circle of 1.3 * radius
// Then we move the point SignedDistance closer along the normal, and 20% closer to the midpoint
double DSFIntermediateSurfaceGenerator::TriangleInfo::Contains(const geo::IPoint& p) const
{
  if (SquaredDistance(p) < r2)
  {
    geo::CVector normal = triangle->Normal();
    double signedDist = triangle->SignedDistance(p, true);

    geo::CPoint trans(p.X() - signedDist * normal.X(), p.Y() - signedDist * normal.Y(), p.Z() - signedDist * normal.Z());

    geo::CVector v(trans, triangle->MidPoint());
    v = v * 0.2;

    trans.X(trans.X() + v.X());
    trans.Y(trans.Y() + v.Y());
    trans.Z(trans.Z() + v.Z());

    // see IFace's Contains
    double angle = 0;

    for (int i = 0; i < 3; ++i)
    {
      geo::CVector v1(trans, triangle->Point(i));
      geo::CVector v2(trans, triangle->Point((i + 1) % 3));

      angle += v1.AngleRad(v2);
    }

    angle = fabs(fabs(angle) - TWOPI);

    return angle < 0.1;
  }
  return false;
}

DSFIntermediateSurfaceGenerator::TriangleInfo::TriangleInfo(const geo::IFace *triangle) : triangle(triangle), x(triangle->MidPoint().X()), y(triangle->MidPoint().Y()), z(triangle->MidPoint().Z()), r2(0)
{
  for (int i = 0; i < 3; ++i)
  {
    double r2candidate = SquaredDistance(triangle->Point(i)) * 1.3;

    if (r2candidate > r2)
      r2 = r2candidate;
  }
}

void DSFIntermediateSurfaceGenerator::TNodeInfo::add(const geo::IPoint & point, const geo::IFace * face, bool precise)
{
  if (precise)
    tmp_tying_infos.push_back(TyingInfo(point, face));
  else
  {
    tying_info.point_dst.Set(point.X(), point.Y(), point.Z());
    tying_info.face_dst = face;
  }
}

void DSFIntermediateSurfaceGenerator::TNodeInfo::process()
{
  if (!tmp_tying_infos.empty())
  {
    for (size_t i = 0; i < tmp_tying_infos.size(); ++i)
      tying_info.point_dst += tmp_tying_infos[i].point_dst;
    tying_info.point_dst.Set(tying_info.point_dst.X() / tmp_tying_infos.size(), tying_info.point_dst.Y() / tmp_tying_infos.size(), tying_info.point_dst.Z() / tmp_tying_infos.size());

    for (size_t i = 0; i < tmp_tying_infos.size(); ++i)
    {
      if (tmp_tying_infos[i].face_dst->Contains(tying_info.point_dst, true))
      {
        tying_info.face_dst = tmp_tying_infos[i].face_dst;
        break;
      }
    }
    if (!tying_info.face_dst)
    {
      tying_info.point_dst = tmp_tying_infos.front().point_dst;
      tying_info.face_dst = tmp_tying_infos.front().face_dst;
    }

    if (tmp_reverse_tying_info.face_dst && tying_info.face_dst)
    {
      geo::CPoint dest = (tmp_reverse_tying_info.point_dst + tying_info.point_dst) / 2;
      geo::CVector n = dest - tmp_reverse_point_src;
      geo::CLine line(tmp_reverse_point_src - dest * 100, tmp_reverse_point_src + dest * 100);

      geo::CPoint inter = tmp_reverse_tying_info.face_dst->Intersection(line);
      if (!inter.Empty() && tmp_reverse_tying_info.face_dst->Contains(inter, true))
      {
        tying_info.point_dst.Set(inter.X(), inter.Y(), inter.Z());
        tying_info.face_dst = tmp_reverse_tying_info.face_dst;
      }
      else
      {
        inter = tying_info.face_dst->Intersection(line);
        if (!inter.Empty() && tying_info.face_dst->Contains(inter, true))
          tying_info.point_dst.Set(inter.X(), inter.Y(), inter.Z());
      }
    }
  }
}




// Map each point of surf_src to some triangle on surf_dst.
void DSFIntermediateSurfaceGenerator::getTyingInformation(TNodeInfoMap & mpNodeInfo, const TSurface& surf_src, const TSurface& surf_dst, bool precise, double eps)
{
  double min_eps = -eps;
  double max_eps = 1 + eps;

  // retrieve triangles from destination surface
  std::vector<TriangleInfo> vcTriangles;
  vcTriangles.reserve(surf_dst.size());

  for (int i = 0; i < surf_dst.size(); ++i)
  {
    vcTriangles.push_back(TriangleInfo(surf_dst[i]));
  }

  typedef std::map<int, std::vector<int> > TNode2Faces;
  TNode2Faces node2Faces;

  std::vector<geo::CVector> normals;

  if (precise)
  {
    normals.reserve(surf_src.size());
    for (int i = 0; i < surf_src.size(); ++i)
      normals.push_back(surf_src[i]->Normal());

    for (int i = 0; i < surf_src.size(); ++i)
    {
      const geo::IFace *face = surf_src[i];
      for (int j = 0; j < 3; ++j)
      {
        std::pair<TNode2Faces::iterator, bool> retval = node2Faces.insert(std::make_pair(face->PointIndex(j), std::vector<int>()));
        if (retval.second)
          retval.first->second.reserve(6);
        retval.first->second.push_back(i);
      }
    }
  }

  for (int i = 0; i < surf_src.size(); ++i)
  {
    const geo::IFace* face1 = surf_src[i];

    for (int m = 0; m < face1->NrOfPoints(); ++m)
    {
      const geo::IPoint& point = face1->Point(m);

      std::pair<TNodeInfoMap::iterator, bool> retval = mpNodeInfo.insert(std::make_pair(face1->PointIndex(m), TNodeInfo()));
      if (retval.second)
      {
        // find triangle
        for (int n = 0; n < vcTriangles.size(); ++n)
        {
          if (vcTriangles[n].Contains(point))
          {
            std::vector<double> isoPt = vcTriangles[n].triangle->WorldToIso(point);
            if (isoPt[0] < max_eps && isoPt[0] > min_eps && isoPt[1] < max_eps && isoPt[1] > min_eps)
            {
              geo::CPlane plane(vcTriangles[n].triangle->Line(0).First(), vcTriangles[n].triangle->Normal());

              retval.first->second.add(plane.Project(point), vcTriangles[n].triangle, precise);

              if (!precise)
                break;
            }
          }
        }

        if (precise)
        {
          TNode2Faces::iterator it = node2Faces.find(face1->PointIndex(m));

          if (it != node2Faces.end())
          {
            geo::CVector normal(0, 0, 0);

            for (int j = 0; j < it->second.size(); ++j)
              normal += normals[it->second[j]];

            normal = normal / it->second.size();

            geo::CLine line(point + normal * 100, point - normal * 100);

            normal = point + normal;

            for (int j = 0; j < retval.first->second.tmp_tying_infos.size(); ++j)
            {
              geo::CPoint inter = retval.first->second.tmp_tying_infos[j].face_dst->Intersection(line);

              if (!inter.Empty() && retval.first->second.tmp_tying_infos[j].face_dst->Contains(inter, true))
              {
                retval.first->second.tmp_reverse_tying_info.point_dst = inter;
                retval.first->second.tmp_reverse_tying_info.face_dst = retval.first->second.tmp_tying_infos[j].face_dst;
                retval.first->second.tmp_reverse_point_src.Set(point.X(), point.Y(), point.Z());
                break;
              }
            }

            if (!retval.first->second.tmp_reverse_tying_info.face_dst)
            {
              // try all triangles
            }
          }
        }

        assert(retval.first->second.tying_info.face_dst || (precise && !retval.first->second.tmp_tying_infos.empty()));
      }
    }
  }

  for (TNodeInfoMap::iterator it = mpNodeInfo.begin(); it != mpNodeInfo.end(); ++it)
    it->second.process();
}

// Create a new intermediate surface based on duplication of the surface surf_src.
// The map mpNodeInfo_dst contains links for each point of surf_src to the other surface (surf_dst, which itself is not needed here).
// We create a new point half-way these two points (or by changing distance, somewhere on this vector from point_src to point_dst).
// No duplicates. Triangles are the same topology as in surf_src.
void DSFIntermediateSurfaceGenerator::getIntermediateSurfaceInfo(IntermediateSurfaceInfo& surf_new, const TNodeInfoMap& mpNodeInfo_dst, const TSurface& surf_src, double distance) const
{
  std::map<int, int> mpPointsNew;

  std::vector<int> triangle_new(3);

  for (int i = 0; i < surf_src.size(); ++i)
  {
    const geo::IFace *face_src = surf_src[i];

    for (int j = 0; j < face_src->NrOfPoints(); ++j)
    {
      std::map<int, int>::iterator it = mpPointsNew.find(face_src->PointIndex(j));
      if (it == mpPointsNew.end())
      {
        geo::CPoint point_src = face_src->Point(j);

        TNodeInfoMap::const_iterator nodeInfo = mpNodeInfo_dst.find(face_src->PointIndex(j));

        assert(nodeInfo != mpNodeInfo_dst.end());

        const geo::IFace *face_dst = nodeInfo->second.tying_info.face_dst;

        mpPointsNew[face_src->PointIndex(j)] = surf_new.points.size();

        triangle_new[j] = (int)surf_new.points.size();

        surf_new.points.push_back(/* point_new = */(point_src + nodeInfo->second.tying_info.point_dst) * distance);
      }
      else
        triangle_new[j] = it->second;
    }
    surf_new.triangles.push_back(triangle_new);
  }
}

// Select one of the two intermediate surfaces, or create a new one...
void DSFIntermediateSurfaceGenerator::selectIntermediateSurfaceInfo(IntermediateSurfaceInfo& surf_minus, IntermediateSurfaceInfo& surf_plus, IntermediateSurfaceInfo& surf_select, bool precise)
{
  if (!precise)
  {
    // Select plus or minus based on size: the larger one is the outersurface in a curved fault in our examples; THIS IS NOT A GOOD TEST!
    if (surf_plus.triangles.size() > surf_minus.triangles.size())
      surf_select = surf_minus;
    else
      surf_select = surf_plus;
  }
  else
  {
    DSFSurfaceCombiner combiner(surf_minus, surf_plus);
    combiner.Generate(surf_select);
  }
}

// Select one of the two intermediate surfaces, or create a new one... [DEBUG version: shows helper surfaces created and used during creation of the final intermediate surface]
void DSFIntermediateSurfaceGenerator::selectIntermediateSurfaceInfo(const QString& name, IntermediateSurfaceInfo& surf_minus, IntermediateSurfaceInfo& surf_plus, IntermediateSurfaceInfo& surf_select, bool precise)
{
  if (!precise)
  {
    selectIntermediateSurfaceInfo(surf_minus, surf_plus, surf_select, precise);
  }
  else
  {
    DSFSurfaceCombiner combiner(surf_minus, surf_plus);

    std::vector<IntermediateSurfaceInfo> helpers;
    combiner.Generate(surf_select, &helpers);

    for (int i = 0; i < helpers.size(); ++i)
    {
      createSurface(QString("%1_helper_%2").arg(name).arg(i), helpers[i]);
    }
  }
}


geo::CSurfaceDesc *DSFIntermediateSurfaceGenerator::createSurface(const QString& name, const IntermediateSurfaceInfo& surf, const CTSSurface& tempSurf)
{
  CSurfaceBase *pSurfaceBase = new CTetraSurface(name, *new geo::CTriSurface(tempSurf), Model());

  CTetraSubHorizon *pHor = new CTetraSubHorizon(name, true, Model());
  pHor->LinkTo(*pSurfaceBase);

  geo::CSurfaceDesc *pSurfaceDesc = new geo::CSurfaceDesc(TetMesh(), pSurfaceBase->Surface(), name.toStdString(), true);
  m_mesh.m_vcSurfaceSource.push_back(pSurfaceBase);

  return pSurfaceDesc;
}

geo::CSurfaceDesc *DSFIntermediateSurfaceGenerator::createSurface(const QString& name, const IntermediateSurfaceInfo& surf)
{
  CTSSurfaceProgress SurfaceProgress;
  CTSSurface tempSurf(surf.points, surf.triangles, SurfaceProgress);
  
  return createSurface(name, surf, tempSurf);
}

// Register the new nodes in the mesh.
void DSFIntermediateSurfaceGenerator::duplicateNodes(TNodeInfoMap& mpNodeInfo, const geo::IElementSet& set)
{
  geo::CTetMeshBase& mesh = TetMesh();

  for (TNodeInfoMap::iterator it = mpNodeInfo.begin(); it != mpNodeInfo.end(); ++it)
  {
    it->second.duplicate_node = mesh.RegisterNode(set.Point(it->first), false);
  }
}

// Create interface elements for the surface (and its surface descriptor).
// We create new "TiedBodyTriangles" for this surface (as they are not faces of any body,
// but (will be tied) to actual bodies of the plus and minus surfaces).
// From those TiedBodyTriangles, the interface elements follow in a straight-forward manner.
// Just note that the order of nodes in either side of the interface element is the same.
void DSFIntermediateSurfaceGenerator::createIFElements(TNodeInfoMap& mpMinus, TNodeInfoMap& mpPlus, const TSurface& surface, geo::CSurfaceDesc& surf_desc, InterfaceInfo& if_info)
{
  std::vector<int> nodes_front(3);
  std::vector<const geo::CBodyTriangle *> triangles_front(3);
  std::vector<int> nodes_back(3);
  std::vector<const geo::CBodyTriangle *> triangles_back(3);

  double min_eps = -0.1;
  double max_eps = 1.1;

  if_info.front.reserve(surface.size());
  if_info.back.reserve(surface.size());
  if_info.if_elts.reserve(surface.size());

  for (int i = 0; i < surface.size(); ++i)
  {
    const geo::IFace& face = *surface[i];

    for (int k = 0; k < face.NrOfPoints(); ++k)
    {
      nodes_front[k] = mpMinus[face.PointIndex(k)].duplicate_node;
      triangles_front[k] = static_cast<const geo::CBodyTriangle *>(mpMinus[face.PointIndex(k)].tying_info.face_dst);
    }

    const geo::IFace *face1 = new geo::CTiedBodyTriangle(TetMesh(), nodes_front, triangles_front);

    for (int k = 0; k < face.NrOfPoints(); ++k)
    {
      nodes_back[k] = mpPlus[face.PointIndex(k)].duplicate_node;
      triangles_back[k] = static_cast<const geo::CBodyTriangle *>(mpPlus[face.PointIndex(k)].tying_info.face_dst);
    }

    const geo::IFace *face2 = new geo::CTiedBodyTriangle(TetMesh(), nodes_back, triangles_back);

    const geo::IInterfaceElement *if_elt = TetMesh().AddInterfaceElement(surf_desc, face1, face2);

    if_info.front.push_back(face1);
    if_info.back.push_back(face2);
    if_info.if_elts.push_back(if_elt);
  }
}

// We map the input surfaces (_minus and _plus) back to the intermediate surface for result viewing.
// This uses a variant of the "generateTyingInfo" routine, and this needs to be refactored a bit to remove duplicate code (TODO).
void DSFIntermediateSurfaceGenerator::createResultTyings(const InterfaceInfo& if_info, const TSurface& surf_front_src, const TSurface& surf_back_src)
{
  double min_eps = -0.1;
  double max_eps = 1.1;

  typedef std::map<int, std::pair<int, geo::CPoint> > TDone;
  TDone done;

  // minus / front
  for (int i = 0; i < surf_front_src.size(); ++i)
  {
    const geo::IFace* face1 = surf_front_src[i];

    for (int m = 0; m < face1->NrOfPoints(); ++m)
    {
      const geo::IPoint& point = face1->Point(m);

      std::pair<TDone::iterator, bool> result = done.insert(std::make_pair(face1->PointIndex(m), std::make_pair(0, geo::CPoint())));

      if (result.second)
      {
        for (int n = 0; n < if_info.front.size(); ++n)
        {
          if (if_info.front[n].Contains(point))
          {
            std::vector<double> isoPt = if_info.front[n].triangle->WorldToIso(point);
            if (isoPt[0] < max_eps && isoPt[0] > min_eps && isoPt[1] < max_eps && isoPt[1] > min_eps)
            {
              geo::CPlane plane(if_info.front[n].triangle->Line(0).First(), if_info.front[n].triangle->Normal());

              static_cast<geo::CTiedPointBodyTriangle *>(const_cast<geo::IFace *>(face1))->AddTying(m, *if_info.if_elts[n], plane.Project(point), 0);
              result.first->second.first = n;
              result.first->second.second.Set(point.X(), point.Y(), point.Z());
              break;
            }
          }
        }
      }
      else
      {
        static_cast<geo::CTiedPointBodyTriangle *>(const_cast<geo::IFace *>(face1))->AddTying(m, *if_info.if_elts[result.first->second.first], result.first->second.second, 0);
      }
    }
  }

  done.clear();

  // plus / back
  for (int i = 0; i < surf_back_src.size(); ++i)
  {
    const geo::IFace* face1 = surf_back_src[i];

    for (int m = 0; m < face1->NrOfPoints(); ++m)
    {
      const geo::IPoint& point = face1->Point(m);

      std::pair<TDone::iterator, bool> result = done.insert(std::make_pair(face1->PointIndex(m), std::make_pair(0, geo::CPoint())));

      if (result.second)
      {
        for (int n = 0; n < if_info.back.size(); ++n)
        {
          if (if_info.back[n].Contains(point))
          {
            std::vector<double> isoPt = if_info.back[n].triangle->WorldToIso(point);
            if (isoPt[0] < max_eps && isoPt[0] > min_eps && isoPt[1] < max_eps && isoPt[1] > min_eps)
            {
              geo::CPlane plane(if_info.back[n].triangle->Line(0).First(), if_info.back[n].triangle->Normal());

              static_cast<geo::CTiedPointBodyTriangle *>(const_cast<geo::IFace *>(face1))->AddTying(m, *if_info.if_elts[n], plane.Project(point), 0);
              result.first->second.first = n;
              result.first->second.second.Set(point.X(), point.Y(), point.Z());
              break;
            }
          }
        }
      }
      else
      {
        static_cast<geo::CTiedPointBodyTriangle *>(const_cast<geo::IFace *>(face1))->AddTying(m, *if_info.if_elts[result.first->second.first], result.first->second.second, 0);
      }
    }
  }

}

// The TNodeInfoMap contains the information on how we map a node in one surface
// to a (body)triangle on another surface.
// Here we create the tyings for Diana, following this map straight-forwardly.
// For Diana, the direction of the tyings must be in the main directions of the model
// hence we use CRotatedSystem. Also, because we can tie to an arbitrary point in a
// triangle, we tie in all three directions of the coordinate system.
void DSFIntermediateSurfaceGenerator::createTyings(const TNodeInfoMap& mpNodeInfo)
{
  CRotatedSystem rs(Model().Boundary().BoxAzimuth(), Model().Boundary().Box().MidPoint());

  geo::CVector rsNormal[3];
  rsNormal[0] = rs.ToGlobal(geo::CVector::Xaxis);
  rsNormal[1] = rs.ToGlobal(geo::CVector::Yaxis);
  rsNormal[2] = rs.ToGlobal(geo::CVector::Zaxis); // should be the same

  for (TNodeInfoMap::const_iterator it = mpNodeInfo.begin(); it != mpNodeInfo.end(); ++it)
  {
    int nIndex = it->second.duplicate_node;
    const geo::IPoint& point = TetMesh().Node(nIndex);
    const geo::IFace *triangle = it->second.tying_info.face_dst;

    if (!triangle)
      continue;

    geo::CTriangle localTriangle(rs.ToLocal(triangle->Point(0)), rs.ToLocal(triangle->Point(1)), rs.ToLocal(triangle->Point(2)));

    std::vector<double> isoPt = localTriangle.WorldToIso(rs.ToLocal(point));
    isoPt.push_back(1 - isoPt[0] - isoPt[1]);

    std::vector<geo::CTying::TMasterDef> vcMasters(3);

    for (int axisIndex = 0; axisIndex < 3; ++axisIndex) // three axes
    {
      for (int pointIndex = 0; pointIndex < 3; ++pointIndex) // isoparametric factor for three points
      {
        vcMasters[pointIndex].direction = &rsNormal[axisIndex];
        vcMasters[pointIndex].node = triangle->PointIndex(pointIndex);
        vcMasters[pointIndex].type = geo::CTying::TR;
        vcMasters[pointIndex].factor = isoPt[pointIndex];
      }
      TetMesh().CreateTying(nIndex, rsNormal[axisIndex], geo::CTying::TR, vcMasters);
    }
  }
}

} // end namespace gm_dsf
