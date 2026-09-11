#ifndef _LoadGridFromRGI_h_
#define _LoadGridFromRGI_h_

class CModelBase;
class RGInterface;
class CMeshSurface;
class RGSurface;
class CFormationBase;

namespace geo {
class CTetMeshBase;
}

namespace GeomecRGI {

class CRockMechProcessor;

} // namespace GeomecRGI

#include "IFace.h"
#include "SharedBodyTriangle.h"

#include <fstream>

namespace GeomecRGI {

class CRGIGridDump {
  std::ofstream m_output;
  std::vector<std::vector<int>> m_points;
  bool valid;

public:
  CRGIGridDump(bool bDoDump);
  ~CRGIGridDump();

  void section(const char *title);
  void size(size_t sz);
  void node(size_t index, geo::CPoint &point);
  void _nodes(std::vector<int> &nodes);
  void element(size_t index, std::vector<int> &nodes);
  void polygon(size_t index, std::vector<int> &nodes);
  void surface(const RGSurface &surface);
};

class CLoadGridFromRGI {
public:
  struct TCompareIFace {
    bool operator()(const geo::IFace *lhs, const geo::IFace *rhs) const { return *lhs < *rhs; }
  };

  typedef std::map<const geo::IFace *, int, TCompareIFace> TFaceDescriptorMap;
  typedef std::map<int, std::set<const geo::IFace *>> TSurfaceCollection;
  typedef std::pair<TSharedBodyTriangle, TSharedBodyTriangle> TTrianglePair;
  typedef std::map<QString, TTrianglePair> TNodes2Triangles;
  typedef std::vector<int> TIndices;

  struct PolygonInfo {
    TIndices nodes;
    geo::IFace *face;

    int ifElement;

    PolygonInfo();

    void setPolygon(TIndices &polygon);
  };
  typedef std::vector<PolygonInfo> TPolygonInfo;
  typedef std::map<TIndices, int> TPolygonMap;

  CLoadGridFromRGI(CModelBase &model, RGInterface &rgi, const CRockMechProcessor &rmp);

  bool operator()();

  std::vector<CFormationBase *> getGeomecFormations() const;

private:
  CLoadGridFromRGI(const CLoadGridFromRGI &rhs);
  CLoadGridFromRGI &operator=(const CLoadGridFromRGI &rhs);

  void insertNodes(TIndices &nodeIndices);
  void insertPolygons(TPolygonInfo &polygonInfo, TPolygonMap &polygonMap);
  void insertElements(TIndices &elementIndices, TIndices &nodeIndices, TPolygonInfo &polygonInfo,
                      TPolygonMap &polygonMap);

  void constructFormations(std::vector<QString> &formationNames, std::map<int, int> &formationVolume2VolumeMapping,
                           const TIndices &elementIndices);

  std::vector<TSharedBodyTriangleMap> getTrianglesPerVolume();
  TNodes2Triangles createNodes2TrianglesMapping(std::vector<TSharedBodyTriangleMap> trianglesPerVolume) const;

  void constructFaults(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                       const TNodes2Triangles &nodes2Triangles, const TIndices &nodeIndices,
                       const TPolygonInfo &polygonInfo);
  void constructHorizons(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                         const TNodes2Triangles &nodes2Triangles, const TIndices &nodeIndices);
  void constructSideSurfaces(TFaceDescriptorMap &linkFace2SurfaceDescriptor, const TNodes2Triangles &nodes2Triangles,
                             const TIndices &nodeIndices);
  void constructFormationVolumes(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                                 std::map<int, int> &formationVolume2VolumeMapping,
                                 const std::vector<TSharedBodyTriangleMap> &trianglesPerVolume,
                                 std::vector<QString> &formationNames);

  void constructFaceDescriptorMap(TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                                  const CMeshSurface *meshSurface) const;
  void constructFault(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                      const TNodes2Triangles &nodes2Triangles, const TIndices &nodeIndices,
                      const TPolygonInfo &polygonInfo, const RGSurface &back, const RGSurface &front) const;
  CMeshSurface *constructMeshSurface(const RGSurface &surface, const TNodes2Triangles &nodes2Triangles,
                                     const TIndices &nodeIndices) const;
  void constructRegularHorizon(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                               const TNodes2Triangles &nodes2Triangles, const TIndices &nodeIndices,
                               const RGSurface &horizon) const;
  void constructTopBottomHorizon(TFaceDescriptorMap &linkFace2Horizon, TFaceDescriptorMap &linkFace2SurfaceDescriptor,
                                 const TNodes2Triangles &nodes2Triangles, const TIndices &nodeIndices,
                                 const RGSurface &horizon) const;
  std::vector<CMeshSurface *> extrapolateMeshSurfaces(const TSharedBodyTriangleMap &triangles,
                                                      const TFaceDescriptorMap &linkFace2SurfaceDescriptor) const;

  CModelBase &m_model;
  geo::CTetMeshBase &m_mesh;
  RGInterface &m_rgi;
  const CRockMechProcessor &m_rmp;
  std::vector<CFormationBase *> m_geomecFormations;

  CRGIGridDump m_dump;
};

} // namespace GeomecRGI

#endif // _LoadGridFromRGI_h_
