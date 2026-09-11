#pragma once

#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include "TriSurface.h"

#include <set>
#include <vector>

class CModelBase;
class CHorizonBase;
class CSurfaceBase;
class CTriangleSurface;
class IProgressBase;

namespace geo
{
class CTetMeshBase;
class ISurface;
}

//#define DEBUG_WELL_TETRA_MESHER

class CWellTetraMesher
{
  struct GridPoint
  {
  geo::CPoint point;

  int surface;

  int index;
  int pointIndex;

  GridPoint();
  GridPoint(const geo::CPoint& point, int surface = -1);
  };

  struct SurfaceInfo
  {
  size_t surfaceIndex;

  const CHorizonBase *horizon;
  const CSurfaceBase *surface;

  geo::CPoint min;
  geo::CPoint max;

  std::vector<int> indices;
  std::set<int> usedIndices;
  std::vector<std::vector<geo::CPoint> > points;

  std::vector<GridPoint> surfacePoints;

  std::vector<int> bottomIntersections;
  std::vector<geo::CPoint> bottomIntersectionPoints;

  bool empty;

  SurfaceInfo(const CHorizonBase *horizon, const CSurfaceBase *surface, const geo::CPoint& restrict_min, const geo::CPoint& restrict_max, int wellSize);

  bool continuous() const;
  };

  struct PointComparer
  {
  typedef long long TLongLong;
  public:
  PointComparer();
  bool operator()(const geo::IPoint& p1, const geo::IPoint& p2) const;
  bool operator()(const CWellTetraMesher::GridPoint& p1, const CWellTetraMesher::GridPoint& p2) const;
  };


  typedef std::pair<int, int> TEdge;
  typedef std::vector<TEdge> TEdges;
  typedef std::vector<int> TTriangles;
  typedef std::map<TEdge, TTriangles> TEdgeTriangles;

  struct AdjustmentData
  {
  geo::CVector line;
  double length;
  double length2;
  double length3;
  };
  typedef std::vector<AdjustmentData> TAdjustmentLine;


  typedef std::vector<geo::CPoint> T2DGrid;
  typedef std::vector<geo::CPoint> TPolyline;
  typedef std::vector<double> TAHD;
  typedef std::vector<TAHD> TAHDGrid;
  typedef std::vector<TPolyline> T3DGrid;
  typedef std::vector<GridPoint> TGridPoints;
  typedef std::vector<TGridPoints> T3DSurf;
  typedef std::vector<SurfaceInfo> TSurfaces;
  typedef std::map<geo::CPoint, std::set<int>, PointComparer> TTrianglePointMap;

public:
  CWellTetraMesher(const CModelBase& model, const std::vector<geo::CPoint>& well, double radius, double idealElementLength);
  ~CWellTetraMesher();

  const CModelBase& Model() const;

  double Radius() const;
  double IdealElementLength() const;

  geo::CTetMeshBase *CreateMesh(IProgressBase& prog);

  int NrOfJobs() const;

private:
  void CreatePolyLine(const std::vector<geo::CPoint>& well, double refineFactor = 1.3);
  void CreateAdjustmentData();
  int PolyLineSize() const;
  int PolyLineLength() const;
  int OriginalPolyLineLength() const;
  geo::CMatrix Create3DGridTransformationMatrix(const geo::IPoint& point, const geo::IVector& vecNormal) const;
  void Create2DGrid();
  void Create3DGrid(TPolyline& well, T3DGrid& grid);
  void CreateAHDGrid();
  void CollectParentSurfaces();
  void CollectSurface(SurfaceInfo& surfInfo);
  void Map3DGridTo2DSideSurface();
  double CalculateAHD(int column, const geo::IPoint& point) const;
  void Map2DSideSurfaceToSurface(CTriangleSurface& surface);
  void MeshSurface(CTriangleSurface& surface);
  int AHDGridIndex(int sizeIndex, double length) const;
  void MapAngleAHDTo3DPoint(const geo::IPoint& inputPoint, geo::CPoint& outputPoint) const;
  void Convert2DSurfaceToVectors(const CTriangleSurface& inputSurface, std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles);
  void Check2DSideSurfaceArea(std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles);
  void Map2DSideSurfaceTo3DSideSurface(const std::vector<geo::CPoint>& points, const std::vector<std::vector<int> >& triangles, geo::CTriSurface& outputSurface);
  void Create2DGridSurface(CTriangleSurface& inputSurface, bool addIntersections = false);
  void Map2DGridSurfaceToWellPoint(int wellPointIndex, const CTriangleSurface& inputSurface, geo::CTriSurface& outputSurface);
  void Map2DIntersectionSurfaceBoundaryTo3DSurface(const SurfaceInfo& surfInfo, TPolyline& outputBoundary);
  void Map3DIntersectionSurfaceBoundaryTo2DSurface(const SurfaceInfo& surfInfo, const TPolyline& inputBoundary, CTriangleSurface& outputSurface);
  void Map2DIntersectionSurfaceTo3DData(const CTriangleSurface& inputSurface, const TPolyline& inputBoundary, std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles);
  void AdjustPointDistance(geo::CPoint& point);
  void CreateSideSurfacePointMap(const geo::CTriSurface& sideSurface, TTrianglePointMap& pointMap);
  bool TriangleInPointMap(const TTrianglePointMap& pointMap, const geo::IPoint& point1, const geo::IPoint& point2, const geo::IPoint& point3);
  void FilterCoincidingTriangles(const TTrianglePointMap& pointMap, const std::vector<geo::CPoint>& inputPoints, const std::vector<std::vector<int> >& inputTriangles, std::vector<geo::CPoint>& points, std::vector<std::vector<int> >& triangles);
  void CreateSurface(const std::vector<geo::CPoint>& points, const std::vector<std::vector<int> >& triangles, geo::CTriSurface& outputSurface);
  void FindBoundary(const geo::ISurface& surface, TPolyline& boundary);
  void CreateIntersectionSurfaces(std::vector<geo::CTriSurface *>& outputSurfaces);
  geo::CTetMeshBase *MeshVolume(const std::vector<geo::CTriSurface *>& surfaces, IProgressBase& prog);


private:
  const CModelBase& m_model;

  double m_radius;
  double m_idealElementLength;

  TPolyline m_originalWell;
  TPolyline m_well;
  T2DGrid   m_2DGrid;
  T3DGrid   m_original3DGrid;
  T3DGrid   m_3DGrid;
  TAHDGrid  m_AHDGrid;
  T3DSurf   m_2DSideSurface;
  TSurfaces m_surfaces;
  TPolyline m_2DTo3DMap;
  TAdjustmentLine m_adjustmentLine;

#ifdef DEBUG_WELL_TETRA_MESHER
  void Debug_WritePointset(const std::string& fileName, const std::vector<geo::CPoint>& points);
  void Debug_WritePointset(const std::string& fileName, const std::vector<GridPoint>& points);
  void Debug_WritePointset(const std::string& fileName, const std::vector<std::vector<geo::CPoint> >& points);
  void Debug_WritePointset(const std::string& fileName, const std::vector<std::vector<GridPoint> >& points);
  void Debug_WritePointset(const std::string& fileName, const CTriangleSurface& surface);
  void Debug_WritePointset(const std::string& fileName, const geo::CTriSurface& surface);
  void Debug_WritePointset(const std::string& fileName, const TSurfaces& surfaces);
  void Debug_WriteSurface(const std::string& fileName, const geo::ISurface& surface);
  void Debug_WriteSurface(const std::string& fileName, const std::vector<geo::CPoint>& points, const std::vector<std::vector<int> >& triangles);
#endif
};
