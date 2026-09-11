#ifndef _TSSURFACE_H_
#define _TSSURFACE_H_

#include "Box.h"
#include "ISurface.h"
#include "MeshZone.h"
#include "Polygon.h"
#include "TSNode.h"
#include "TSProgressBase.h"
#include "Triangle.h"
#include <fstream>

#ifndef BUILDING_TSMESH
#define GtsFace void
#define GtsVertex void
#define GtsSurface void
#define GSList void
#define GtsBBox void
#define GtsEdge void
#endif

// ##ModelId=3BBD944902B2
class CTSSurface : public geo::ISurface {
  typedef std::set<CMeshZone> TMeshSet;
  typedef TMeshSet::iterator TMeshSetIt;
  typedef std::map<std::pair<int, int>, GtsEdge *> TIndexToEdgeMap;
  GtsEdge *InsertEdge(TIndexToEdgeMap &mpEdge, int nFirst, int nSecond);

public:
  typedef std::map<const geo::IPoint *, size_t, geo::ICoordinate::CCoordinateLess> TPointMap;
  typedef std::map<GtsFace *, size_t> TFaceMap;
  // This inner class is used to return some statistics about the surface
  class CTSSurfaceStat {
  private:
    double m_dSurfaceArea;
    double m_dFaceAreaMean;
    double m_dEdgeLengthMean;
    double m_dEdgeAngleMean;

  public:
    CTSSurfaceStat(const double &SurfaceArea, const double &FaceAreaMean, const double &EdgeLengthMean,
                   const double &EdgeAngleMean)
        : m_dSurfaceArea(SurfaceArea), m_dFaceAreaMean(FaceAreaMean), m_dEdgeLengthMean(EdgeLengthMean),
          m_dEdgeAngleMean(EdgeAngleMean) {}
    CTSSurfaceStat() : m_dSurfaceArea(0), m_dFaceAreaMean(0), m_dEdgeLengthMean(0), m_dEdgeAngleMean(0) {}

    double SurfaceArea() const { return m_dSurfaceArea; }
    double FaceAreaMean() const { return m_dFaceAreaMean; }
    double EdgeLengthMean() const { return m_dEdgeLengthMean; }
    double EdgeAngleMean() const { return m_dEdgeAngleMean; }
  };

  typedef std::map<GtsVertex *, std::vector<GtsEdge *>> TVertexMap;
  enum eTypeOfDetailChange { COARSEN, REFINE, ZONES };
  // use an existing surface
  // ##ModelId=3BBD944902C1
  CTSSurface(const geo::ISurface &surface, ITSProgressBase &progress);

  // use a point cloud
  CTSSurface(const geo::CPtrArray<geo::IPoint> &points, ITSProgressBase &progress, const geo::CPolygon &Polygon,
             bool UsePolyPoints = false);
  CTSSurface(const geo::CPtrArray<geo::IPoint> &points, ITSProgressBase &progress);
  CTSSurface(const geo::CArray<geo::CPoint> &points, ITSProgressBase &progress);
  CTSSurface(const CTSSurface &surface);
  CTSSurface(const std::vector<const geo::IFace *> &vcFace, ITSProgressBase &progress);
  CTSSurface(const std::vector<geo::CPoint> &vcPoint, const std::vector<std::vector<int>> &vcTriangle,
             ITSProgressBase &progress);

  // ##ModelId=3BBD944902C2
  ~CTSSurface();

  virtual const geo::IFace &Face(int nIndex) const;
  virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint &p) const;
  virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint &p) const;

  virtual const geo::CVector AverageNormal() const;
  virtual void Rotate(const geo::IVector &vec, const double &dAngleDeg);

  virtual int FaceSize() const;

  virtual const geo::IPoint &Point(int nIndex) const;
  virtual int PointSize() const;

  virtual bool Planar() const { return bPlanar; }

  void SharedPoints(std::set<geo::IPoint *> &OwnPoints, std::set<geo::IPoint *> &OtherPoints,
                    CTSSurface &Surface) const;
  void Planar(bool bValue) { bPlanar = bValue; }

  void AddMeshZone(CMeshZone &meshzone);
  void RemoveMeshZone(CMeshZone &meshzone);
  CTSSurface *ChangeSurfaceDetail(double *DetailData, double &MinimumAngle, eTypeOfDetailChange Type);

  void WriteSurface(FILE *OutFile);

  bool Original();
  bool Increase(geo::CBox box, double dOffset = 0);
  bool Increase(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset, double dOffset = 0);
  //	bool Decrease(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset);

  CTSSurfaceStat Statistics() const;
  double CalcAverageEdgeSize();

  virtual std::vector<int> Nodes(const geo::IElement &element) const;

protected:
  // Modifiers....
  virtual geo::IPoint &PointAt(int nIndex);
  virtual void InvalidateCache();

private:
  CTSSurface(GtsSurface *surface);
  // This function is used by a couple of constructors to construct the surfaces by a vector of faces
  void ConstructUsingFaces(const std::vector<const geo::IFace *> &vcFace, ITSProgressBase &progressbool);
  void BuildInterface();
  void RemoveRegionsAndMeshZones();
  void RemoveRedundantFaces(const geo::CPolygon &Polygon);
  void InsertExtensionPoints(const std::vector<GtsVertex *> &vcBoundary, const std::vector<geo::CVector> &vcVectors,
                             const geo::CBox &box);
  geo::CVector BoundaryIncreaseVector(GtsVertex *p1, GtsVertex *p2, GtsVertex *p3, double length) const;
  void GenerateBoundaryIncreaseVectors(const std::vector<GtsVertex *> &vcBoundary, bool bWindingRight,
                                       std::vector<geo::CVector> &vcVectors, double length) const;
  void InsertBoxCorner(const double &x, const double &y, const std::vector<GtsVertex *> &vcBoxEdgeVertices);
  bool NextInBBox(const GtsVertex *p, const geo::CVector &v, const geo::CBox &box) const;
  GtsVertex *CreateVertexOnBBox(const GtsVertex *p, const geo::CVector &v, const geo::CBox &box) const;
  GtsVertex *GtsVertexFromPoint(const geo::IPoint &point) const;

  double BoundaryArea(const std::vector<GtsVertex *> &vcBoundary) const;
  void ProcessEdgeRing(std::vector<GtsVertex *> &vcVertices, const TVertexMap &mpVertices, GtsVertex *pFirstVertex,
                       GtsEdge *pFirstEdge) const;
  GtsEdge *OtherEdge(GtsVertex *pVertex, GtsEdge *pEdge, const TVertexMap &mpVertices) const;
  GtsVertex *OtherVertex(GtsEdge *pEdge, GtsVertex *pVertex) const;
  void InsertEdge(TVertexMap &mpVertices, GtsEdge *pEdge, GtsVertex *v) const;
  void CreateGtsSurfaceObjectFromPoints(GSList *pPointCloud, ITSProgressBase *pProgress = 0);

  std::vector<GtsVertex *> SurfaceBoundary() const;

  TFaceMap m_mpFaceMap;

  mutable bool m_bIsDirty; // If true, the m_vcAverage vector has not been calculated yet....(in AverageNormal())
  mutable geo::CVector m_vcAverage;

  GtsSurface *m_pGtsSurface;
  GSList *m_pPointCloud;
  GSList *m_pCopyPointCloud;
  std::vector<geo::CTriangle *> m_vcElements;
  std::vector<CTSNode *> m_vcNodes;
  TMeshSet m_stMeshZones;
  GSList *m_pRegions;
  GtsBBox *m_pGtsBBox;
  bool bPlanar; // true is the surface is planar
  TPointMap m_mpPoint;
};

#endif /* _TSSURFACE_H_ */
