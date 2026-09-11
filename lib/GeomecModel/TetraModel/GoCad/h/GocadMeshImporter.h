#ifdef SKUA_NEW

#include <vector>

namespace geo
{
class CTetMeshBase;
class CSurfaceDesc;
class CBodyTriangle;
class IElementSet;
}

class CTetraModel;
class CTetraMesh;
class CSurfaceBase;

class IProgressBase;

namespace gm_skua
{
struct SKUAParseData;
}

#include "Point.h"
#include "GocadData.h"
#include "IFace.h"

geo::CPoint Node2Point(const CGocadData::CNode& node);


class CGocadMeshImporter
{
public:
  CGocadMeshImporter(const gm_skua::SKUAParseData *solid, CTetraMesh& tetramesh);
  CGocadMeshImporter(const std::vector<const gm_skua::SKUAParseData *>& solids, CTetraMesh& tetramesh);
  virtual ~CGocadMeshImporter();

  bool Import();

private:
  const std::vector<const gm_skua::SKUAParseData *> m_inputSolids;
  const gm_skua::SKUAParseData *m_solid;
  CTetraMesh& m_tetramesh;

  CTetraModel& Model();
  const CTetraModel& Model() const;

  const geo::CTetMeshBase& TetMesh() const;
  geo::CTetMeshBase& TetMesh();

  bool CreateNodes(IProgressBase& progress);
  bool CreateTetrahedrons(IProgressBase& progress, std::vector<int>& re_index_tetras);
  bool CreateVolumes(IProgressBase& progress, const std::vector<int>& re_index_tetras);
  bool CreateFormations(IProgressBase& progress);
  void SortFormations(IProgressBase& progress);
  bool CreateSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, std::vector<CSurfaceBase *>& mesh_surfaces, const std::vector<int>& re_index_tetras);
  bool CreateInterfaceElements(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, const std::vector<int>& re_index_tetras);

  bool CreateIntermediateSurfaces(IProgressBase& progress, const std::vector<geo::CSurfaceDesc *> mesh_surfaces);

  // helpers
  bool _createFormationsFromTetraRegions(IProgressBase& progress);
  bool _createFormationsFromVolumes(IProgressBase& progress);

  bool _createDataSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions, const std::vector<int>& re_index_tetras,
  std::vector<CSurfaceBase *>& mesh_surfaces, std::vector<std::string>& mesh_surface_names, std::vector<std::map<size_t, std::vector<const geo::IFace *> > >& surface2bodies);
  bool _createModelSurfaces(IProgressBase& progress, std::vector<geo::CSurfaceDesc *>& surface_descriptions,
  std::vector<CSurfaceBase *>& mesh_surfaces, std::vector<std::string>& mesh_surface_names, std::vector<std::map<size_t, std::vector<const geo::IFace *> > >& surface2bodies);

  bool canHandleVolume(int type);

// LEGACY
public:
  class CPositionTriangle
  {
  public:
  CPositionTriangle(const geo::IFace& face)
  {
      assert(face.NrOfPoints() == 3);

      m_stPoints.insert(face.Point(0));
      m_stPoints.insert(face.Point(1));
      m_stPoints.insert(face.Point(2));

      assert(m_stPoints.size() == 3);
  }

  CPositionTriangle(const CGocadData::CTriangle& triangle)
  {
      m_stPoints.insert(Node2Point(triangle.Node(0)));
      m_stPoints.insert(Node2Point(triangle.Node(1)));
      m_stPoints.insert(Node2Point(triangle.Node(2)));

      assert(m_stPoints.size() == 3);
  }

  bool operator<(const CPositionTriangle& rhs) const
  {
      return m_stPoints < rhs.m_stPoints;
  }

  private:
  std::set<geo::CPoint> m_stPoints;
  };

  // each face belongs to a bodygroup
  // we want to be able to get the faces at a certain position (defined by a CPositionTriangle)
  typedef std::pair<const geo::IFace*, const geo::CBodyGroup*> TFaceBodyPair;
  typedef std::set<TFaceBodyPair> TFaceBodyPairSet;
  typedef std::map<CPositionTriangle, TFaceBodyPairSet> TPositionFacesMap;

};

#else
// GocadMeshImporter.h: interface for the CGocadMeshImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOCADMESHIMPORTER_H__0343DDCC_B1D5_4021_9B79_E1BCC254E3B0__INCLUDED_)
#define AFX_GOCADMESHIMPORTER_H__0343DDCC_B1D5_4021_9B79_E1BCC254E3B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProgressBase;

#include "GocadImport.h"
#include "GocadData.h"

#include "Point.h"

geo::CPoint Node2Point(const CGocadData::CNode& node);

class CTetraMesh;
class CTetraModel;
class CSurfaceBase;
class COpenGLNode;
class CTSSurface;

namespace geo
{
class CSurfaceDesc;
}

namespace geo {
  class CTetMeshBase;
  class IFace;
  class CBodyGroup;
  class CBodyTriangle;
}

#include <set>
#include <map>
#include "IFace.h"

//create_gm_objs
//typedef std::map<double, int> TAverageDepthMap;

typedef std::map<const CGocadData::CVertex*, int> TVertex2NodeIndexMap;

geo::CVector TriangleNormal(const CGocadData::CTriangle& triangle);

class CGocadMeshImporter  
{
public:
  CGocadMeshImporter(const CGocadData::CTSolid& tsolid, CTetraMesh& tetramesh);
  CGocadMeshImporter(const std::vector<const CGocadData::CTSolid*>& vcTSolids, CTetraMesh& tetramesh);
  virtual ~CGocadMeshImporter();

  CTetraModel& Model();
  const CTetraModel& Model() const;

  const geo::CTetMeshBase& TetMesh() const;
  geo::CTetMeshBase& TetMesh();

  void set_formations();
  int set_points( int iSolid, int iVolume, TVertex2NodeIndexMap& mpVertex2NodeIndex, CProgressBase& progdlg );
  void set_tetras( int iSolid, int iVolume, TVertex2NodeIndexMap& mpVertex2NodeIndex, std::vector< int >& vol_elems_v, CProgressBase& progdlg );
  

  bool Import();

private:
  enum eProgress{ Surface, ConnectivityMap, PointsPlusTetras };
  int get_steps( eProgress type_ );

private:
  std::vector<const CGocadData::CTSolid*> m_vcTSolids;
  CTetraMesh& m_tetramesh;

public:
  class CPositionTriangle
  {
  public:
  CPositionTriangle(const geo::IFace& face)
  {
      assert(face.NrOfPoints() == 3);

      m_stPoints.insert(face.Point(0));
      m_stPoints.insert(face.Point(1));
      m_stPoints.insert(face.Point(2));

      assert(m_stPoints.size() == 3);
  }

  CPositionTriangle(const CGocadData::CTriangle& triangle)
  {
      m_stPoints.insert(Node2Point(triangle.Node(0)));
      m_stPoints.insert(Node2Point(triangle.Node(1)));
      m_stPoints.insert(Node2Point(triangle.Node(2)));

      assert(m_stPoints.size() == 3);
  }

  bool operator<(const CPositionTriangle& rhs) const
  {
      return m_stPoints < rhs.m_stPoints;
  }

  private:
  std::set<geo::CPoint> m_stPoints;
  };

  // each face belongs to a bodygroup
  // we want to be able to get the faces at a certain position (defined by a CPositionTriangle)
  typedef std::pair<const geo::IFace*, const geo::CBodyGroup*> TFaceBodyPair;
  typedef std::set<TFaceBodyPair> TFaceBodyPairSet;
  typedef std::map<CPositionTriangle, TFaceBodyPairSet> TPositionFacesMap;

  class CGocadTFace;

private:
#if 0
  struct TriangleInfo
  {
  double x, y, z;
  double r2;
  const geo::CBodyTriangle *triangle;

  double SquaredDistance(const geo::IPoint& p);
  double Contains(const geo::IPoint& p);

  TriangleInfo(const geo::CBodyTriangle *triangle);
  };
#endif

  class CGocadSurface
  {
  public:
  CGocadSurface(const CGocadData::CSurface& gocadsurface);
  ~CGocadSurface();

  const CGocadData::CSurface& Surface() const;

  int TFaceSize() const;
  const CGocadTFace& TFace(int i) const;
  CGocadTFace& TFace(int i);

  bool IsFault() const;

  void MarkAsFault();

  void MergeFaces();

  private:
  const CGocadData::CSurface& m_surface;
  std::vector<CGocadTFace*> m_vcTFaces;
  bool m_bFault;
  };

public:
  class CGocadTFace
  {
  public:
  CGocadTFace(const CGocadData::CTFace& tface);

  // - the front and back groups can be the same, in that case
  //    this TFace's parent surface MUST be a fault
  // - the back group can be NULL, then this TFace's parent is a side/top/bottom surface
  const geo::CBodyGroup* FrontGroup() const;
  const geo::CBodyGroup* BackGroup() const;

  // opposite faces, first is front, second is back
  typedef std::pair<const geo::IFace*, const geo::IFace*> TFacePair;
  int FacePairSize() const;
  const TFacePair& FacePair(int i) const;

  bool CollectFaces(const TPositionFacesMap& mpPositionFaces);

  void AttachSurface(CSurfaceBase& surface);
  CSurfaceBase* AttachedSurface();

  private:
  bool CollectFacesSimple(const TPositionFacesMap& mpPositionFaces, bool bTwoSeparateBodies);

  const CGocadData::CTFace& m_tface;
  const geo::CBodyGroup* m_pFrontGroup;
  const geo::CBodyGroup* m_pBackGroup;
  std::vector<TFacePair> m_vcFacePairs;
  CSurfaceBase* m_pSurface;
  };

  	struct TSurfaceDef
{
  TSurfaceDef(CGocadMeshImporter::CGocadTFace& tface, bool bFrontGroup,
  bool bFault)
  : m_tface(tface),
  m_bFrontGroup(bFrontGroup),
  m_bFault(bFault)
  {
  }

  TSurfaceDef(const TSurfaceDef& rhs)
  : m_tface(rhs.m_tface),
  m_bFrontGroup(rhs.m_bFrontGroup),
  m_bFault(rhs.m_bFault)
  {
  }

  TSurfaceDef& operator=(const TSurfaceDef& rhs)
  {
  assert(&m_tface == &rhs.m_tface);
  m_bFrontGroup = rhs.m_bFrontGroup;
  m_bFault = rhs.m_bFault;

  return *this;
  }

  CGocadMeshImporter::CGocadTFace& m_tface;
  bool m_bFrontGroup;
  bool m_bFault;
};
  
  // create_gm_objs
  typedef std::vector<TSurfaceDef> TSurfaceVec;
  typedef std::map<const geo::CBodyGroup*, TSurfaceVec> TBodyGroup2SurfaceMap;
  typedef std::map<double, int> TAverageDepthMap;

private:
  std::vector<CGocadSurface*> m_vcGocadSurfaces;

  typedef std::set<CPositionTriangle> TPositionTriangleSet;

private:
  bool set_surfaces(IProgressBase& progdlg);
  TPositionFacesMap map_position_faces( TPositionTriangleSet& stGocadSurfaceTriangles, CProgressBase& progdlg );
  CGocadMeshImporter::TPositionTriangleSet gocad_sur_tri_set();
  bool add_surface( const CGocadData::CSurface& gocadsurface, TPositionFacesMap& mpPositionFaces, CProgressBase& progdlg );
  void add_to_gocad_sur_tri_set( const CGocadData::CSurface& sur, TPositionTriangleSet& stGocadSurfaceTriangles );


  void DestroyGocadSurfaces();
  bool create_gm_objs(CsProgressBase& progdlg);

  typedef std::map<const CGocadTFace*, COpenGLNode*> TSurfaceSourceMap;
  typedef std::map<const CSurfaceBase*, geo::CSurfaceDesc*> TSurface2SurfaceDescMap;
  geo::CSurfaceDesc& GetSurfaceDescriptor(const CSurfaceBase& surfacebase, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, bool bSlip, COpenGLNode& surfacesource);

  void CreateInterfaceElements(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc);
  //void CreateInterfaceElementsDSF(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc);
  void CreateIntermediateSurfaces(IProgressBase& progdlg, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc);

  bool create_gm_objs_01( bool& bHaveDoubleSidedFaults, 
              TAverageDepthMap& mpAverageDepth, 
              CProgressBase& progdlg );

  void create_gm_objs_02(	int iTopSurface, 
              TSurfaceSourceMap& mpSurfaceSource, 
              TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
              CProgressBase& progdlg ); // top horizon

  void create_gm_objs_03(	int iBotSurface, 
              TSurfaceSourceMap& mpSurfaceSource, 
              TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
              CProgressBase& progdlg ); // bottom horizon

  void create_gm_objs_04(	TSurfaceSourceMap& mpSurfaceSource, 
              TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
              int iTopSurface,
              int iBotSurface,
              CProgressBase& progdlg );

void create_gm_objs_05( TSurfaceSourceMap& mpSurfaceSource, 
            TBodyGroup2SurfaceMap& mpBodyGroup2Surface, 
            int bHaveDoubleSidedFaults,
            CProgressBase& progdlg );

#if 0
  // helper functions
  typedef std::pair<int, const geo::CBodyTriangle *> TNodeInfo;
  typedef std::map<int, TNodeInfo> TNodeInfoMap;
#endif

  CGocadSurface *findOppositeFault(CGocadSurface& fault);

#if 0
  void getTyingInformation(TNodeInfoMap& mpNodeInfo, CGocadSurface& minusGocadsurface, CGocadSurface& plusGocadsurface, double eps = .1);
  void getTyingInformation(TNodeInfoMap& mpNodeInfo, CSurfaceBase& fault, CGocadSurface& plusGocadsurface, double eps = .1);

  void duplicateNodes(TNodeInfoMap& mpNodeInfo, const geo::IElementSet& set);
  
  void getIntermediateSurfaceInfo(std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles, TNodeInfoMap& mpNodeInfo, CGocadSurface& surface);
  void addMeshZones(CTSSurface& output, CGocadSurface& input);

  void createIFElements(TNodeInfoMap& mpNodeInfo, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, CGocadSurface& fault);
  void createIFElements(TNodeInfoMap& mpMinus, TNodeInfoMap& mpPlus, TSurface2SurfaceDescMap& mpSurface2SurfaceDesc, CSurfaceBase& fault);

  void createTyings(TNodeInfoMap& mpNodeInfo);
#endif
};

#endif // !defined(AFX_GOCADMESHIMPORTER_H__0343DDCC_B1D5_4021_9B79_E1BCC254E3B0__INCLUDED_)
#endif
