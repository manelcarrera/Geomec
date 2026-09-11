// Surface.h: interface for the CSurfaceBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACE_H__F43A985C_7BB1_4CBF_9737_BF08301ACDFD__INCLUDED_)
#define AFX_SURFACE_H__F43A985C_7BB1_4CBF_9737_BF08301ACDFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSurfaceValueType;

#include "ElementSet.h"
#include "TriaIntersecBox.h"

typedef std::vector<STNamedSurface> CNamedSurfaces;

class CSurfaceMeshZone;

#include "Box.h"
#include "Plane.h"
#include "TriSurface.h"

class CUnitNode;

class CSurfaceBase : public IElementSet {
  friend class CSurfaceValueType;

public:
  typedef geo::CPtrArray<geo::ISurface> TSurfaceList;
  typedef geo::CTriSurface TGeoSurface;

private:
  mutable TGeoSurface *m_pSurface; // Geometrical surface
  mutable geo::CPlane m_ProjectionPlane;
  mutable geo::CPtrArray<CSurfaceMeshZone> m_MeshRegions;
  CSurfaceValueType *m_pValueType;
  bool Build() const;
  void Invalidate();
  CDoubleQuantity::UNIT m_unit;

protected:
  // Construction ...
  CSurfaceBase(CSurfaceValueType &value_type);
  CSurfaceBase(const QString &strName, const geo::ISurface &surface, CFemAppModel &model);
  CSurfaceBase(const QString &strName, const std::vector<const geo::IFace *> &vcFace, CFemAppModel &model);
  CSurfaceBase(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel &model);
  CSurfaceBase(const QString &strName, const geo::CArray<geo::CPoint> &vcPoint, CFemAppModel &model);
  virtual CSurfaceBase *OnCreateSurface(const QString &sName, const geo::CPtrArray<geo::IPoint> &vcPoint);
  virtual CSurfaceBase *OnCreateSurface(const QString &sName, const std::vector<const geo::IFace *> &vcFaces);

public:
  CSurfaceBase(CFemAppModel &model);
  CSurfaceBase(const CSurfaceBase &surface);
  ~CSurfaceBase();

  // Display list
  virtual const geo::IObject &DisplayList(int nIndex) const;
  virtual int DisplayListSize() const;

  // Save and loading
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual bool CanDestroy() const;
  // Usual fee
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual bool PointInConvexHull(const geo::IPoint &pt) const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  CSurfaceBase &operator=(const CSurfaceBase &rhs);
  bool operator==(const CSurfaceBase &rhs) const;
  const geo::ISurface &Surface() const;

  virtual int RegisterPoint(const geo::IPoint &point);
  typedef std::vector<int> TNodeVec;
  virtual int CreateElement(const TNodeVec &vcIndex);

  // the plane for the mesh regions
  const geo::CPlane &ProjectionPlane() const;

  // the mesh regions
  geo::CPtrArray<CSurfaceMeshZone> &MeshRegions() { return m_MeshRegions; }
  void DeleteAllMeshregions();

  virtual const CGraphNode *Used() const;

  //	bool Original();
  bool Increase(geo::CBox box, double dOffset = 0);
  bool Increase(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset, double dOffset = 0);
  //	bool Decrease(double dXNOffset, double dYNOffset, double dXMOffset, double dYMOffset);

  //	double CalcAverageEdgeSize();
  geo::CValue InterpolateDepth(const geo::IPoint &point, bool bExtrapolate = false) const;

  virtual DIMENSION Dimension() const { return DIM_3D; }
  const geo::IElementSet &ElementSet() const { return Surface(); }
  geo::IElementSet &ElementSet() { return *m_pSurface; }
  CSurfaceBase *MergeSurface(const CSurfaceBase &surface, bool bReTriangulate);
  CSurfaceBase *MergeSurfaces(std::vector<const CSurfaceBase *> surfaces, bool bReTriangulate);

  void Unit(CDoubleQuantity::UNIT unit);
  CDoubleQuantity::UNIT Unit() const { return m_unit; }
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  void ExportSurface(const QString &sExt, const CUnitNode &unitNode, const QString &fileName);

  bool SurfaceIsVertical();

  ACCEPT_GEOMECMODELVISITORS(VisitSurfaceBase);

private:
  void LoadMesh(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
};

// Define a entry type ...
class CSurfaceEntry : public CStorageNodeEntry<CSurfaceBase> {
private:
  typedef enum { ST_HEXA = 1, ST_TETRA = 2 } TSurfaceType;

public:
  void DoCutSurfaces(CModelBase &model, int cutSurfacesCount, const std::vector<STNamedSurface> &cutSurfaces,
                     const std::map<SIDE_ID, geo::CTriSurface *> &sideSurfaces, const QString &baseName,
                     const std::vector<SIDE_ID> &sides, const geo::CBox *pbox = 0);

public:
  CSurfaceEntry(CFemAppModel &model);
  virtual void LoadStream(CFemAppModel &model, TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  void ExportSurfaces(CNamedSurfaces &slected_surfaces, const QString &fileName, const CUnitNode &unitNode);
  void RemoveUnused();

  CSurfaceBase *InsertSurface(const QString &name, const geo::ISurface &surface);

  // lookup a base surface that contains the given ISurface
  CSurfaceBase *LookUpSurface(const geo::ISurface *surf);

  bool GetSurfaceCount() const;
  bool NonUsedSurface() const;
  int NonUsedSurfaces() const;

  void DoSelectAndCreateHorizons(CNamedSurfaces &slected_surfaces, CModelBase &model);

  bool CanSelectAndCreateSubBoundarySides(const CModelBase &model) const;
  void DoSelectAndCreateSubBoundarySides(CNamedSurfaces &slected_surfaces, CModelBase &model);

  bool CanCreateSubBoundaryTopAndBottom(const CModelBase &model) const;
  void DoCreateSubBoundaryTopAndBottom(CModelBase &model);

  bool CanSelectAndCreateFaults(const CModelBase &model) const;
  void DoSelectAndCreateFaults(CNamedSurfaces &slected_surfaces, CModelBase &model);

  bool CanSelectAndUnassignSurfaces() const;
  void DoSelectAndUnassignSurfaces(CNamedSurfaces &selected_surfaces);

  bool CanSelectAndDeleteSurfaces() const;
  void DoSelectAndDeleteSurfaces(CNamedSurfaces &selected_surfaces);

  bool CanMergeFaultSurfaces(const CModelBase &model) const;
  void DoMergeFaultSurfaces(CModelBase &model);

  void MergeSurfaces(CNamedSurfaces &selected_surfaces);

  ACCEPT_GEOMECMODELVISITORS(VisitSurfaceEntry);
};

class CSurfaceMeshZone : public geo::CPolygon {
private:
  double m_MinAcceptableAngle;
  double m_MaxAcceptableArea;

public:
  CSurfaceMeshZone() : m_MinAcceptableAngle(10), m_MaxAcceptableArea(100000) {}

  CSurfaceMeshZone(const geo::CPolygon &polygon)
      : geo::CPolygon(polygon), m_MinAcceptableAngle(10), m_MaxAcceptableArea(100000) {}

  CSurfaceMeshZone &operator=(const geo::CPolygon &rhs) {
    ((geo::CPolygon *)this)->operator=(rhs);
    return *this;
  }

  double &MinAcceptableAngle() { return m_MinAcceptableAngle; }
  double &MaxAcceptableArea() { return m_MaxAcceptableArea; }
};

#endif // !defined(AFX_SURFACE_H__F43A985C_7BB1_4CBF_9737_BF08301ACDFD__INCLUDED_)
