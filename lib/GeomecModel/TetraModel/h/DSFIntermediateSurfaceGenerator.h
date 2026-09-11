#pragma once

#include <map>
#include <vector>
#include <QString>

#include "Point.h"

class CTetraMesh;
class CTetraModel;
class CSurfaceBase;

namespace geo
{
class IElementSet;
class CTetMeshBase;
class IFace;
class CSurfaceDesc;
}

// temp
class CGocadSurface;
class CTSSurface;

namespace gm_dsf
{

struct IntermediateSurfaceInfo;


class DSFIntermediateSurfaceGenerator
{
  CTetraMesh& m_mesh;

public:
  typedef std::vector<const geo::IFace *> TSurface;

  DSFIntermediateSurfaceGenerator(CTetraMesh& tetramesh);

  bool GenerateIntermediateSurface(const QString& name, const TSurface& surf_0, const TSurface& surf_1, bool precise = false);

private:

  CTetraModel& Model();
  const CTetraModel& Model() const;

  geo::CTetMeshBase& TetMesh();
  const geo::CTetMeshBase& TetMesh() const;


  struct TriangleInfo
  {
  double x, y, z;
  double r2;
  const geo::IFace *triangle;

  double SquaredDistance(const geo::IPoint& p) const;
  double Contains(const geo::IPoint& p) const;

  TriangleInfo(const geo::IFace *triangle);
  };

  struct InterfaceInfo
  {
  std::vector<TriangleInfo> front;
  std::vector<TriangleInfo> back;
  std::vector<const geo::IInterfaceElement *> if_elts;
  };

  struct TyingInfo
  {
  geo::CPoint point_dst;
  const geo::IFace *face_dst;

  TyingInfo() : face_dst(nullptr) {}
  TyingInfo(const geo::CPoint& point, const geo::IFace *face) : face_dst(face)
  {
      point_dst.Set(point.X(), point.Y(), point.Z());
  }
  };

  struct TNodeInfo
  {
  int duplicate_node;
  TyingInfo tying_info;

  std::vector<TyingInfo> tmp_tying_infos;
  TyingInfo tmp_reverse_tying_info;

  geo::CPoint tmp_reverse_point_src;

  TNodeInfo() : duplicate_node(-1)
  {
      tmp_tying_infos.reserve(6);
  }

  void add(const geo::IPoint& point, const geo::IFace *face, bool precise = false);
  void process();
  };

  typedef std::map<int, TNodeInfo> TNodeInfoMap;

  void getTyingInformation(TNodeInfoMap& mpNodeInfo, const TSurface& surf_src, const TSurface& surf_dst, bool precise = false, double eps = .1);
  void getIntermediateSurfaceInfo(IntermediateSurfaceInfo& surf_new, const TNodeInfoMap& mpNodeInfo_dst, const TSurface& surf_src, double distance = .5) const;

  void selectIntermediateSurfaceInfo(IntermediateSurfaceInfo& surf_minus, IntermediateSurfaceInfo& surf_plus, IntermediateSurfaceInfo& surf_selected, bool precise);
  void selectIntermediateSurfaceInfo(const QString& name, IntermediateSurfaceInfo& surf_minus, IntermediateSurfaceInfo& surf_plus, IntermediateSurfaceInfo& surf_selected, bool precise);

  geo::CSurfaceDesc *createSurface(const QString& name, const IntermediateSurfaceInfo& surf, const CTSSurface& mesh_surf);
  geo::CSurfaceDesc *createSurface(const QString& name, const IntermediateSurfaceInfo& surf);

  void duplicateNodes(TNodeInfoMap& mpNodeInfo, const geo::IElementSet& set);
  void createIFElements(TNodeInfoMap& mpMinus, TNodeInfoMap& mpPlus, const TSurface& surface, geo::CSurfaceDesc& surf_desc, InterfaceInfo& if_info);
  void createResultTyings(const InterfaceInfo& if_info, const TSurface& surf_front_src, const TSurface& surf_back_src);
  void createTyings(const TNodeInfoMap& mpNodeInfo);
};

}
