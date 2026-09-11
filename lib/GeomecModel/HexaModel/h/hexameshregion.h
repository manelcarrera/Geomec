// MeshRegion.h: interface for the CMeshRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHREGION_H__3ADCC8B8_44AC_41B7_95A1_E449C4CA86FA__INCLUDED_)
#define AFX_MESHREGION_H__3ADCC8B8_44AC_41B7_95A1_E449C4CA86FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecModelVisitor.h"
#include "MeshRegionBase.h"

#define MESH_NO_DANG 0
#define MESH_VERT_DANG 1
#define MESH_HOR_DANG 2
#define MESH_NOT_ON_GRID 3

class CHexaBoundary;
class C3DModel;
class CModelBase;
class CHexaMeshRegionBase : public CMeshRegionBase {
protected:
  // The inner hull, snapped on the grid of the meshregion.
  // Located on the top of the model. Z = Boundary().Min().Z()
  geo::CPolygon m_hull;

  // The gridpoints in the hull of the
  geo::CArray<geo::CPoint> m_vcGridPoints;

  // The displaylist for displaying the mesh region ...
  mutable geo::CPtrArray<geo::IObject> m_displayList;
  mutable geo::CPtrArray<geo::CPoint> m_points;
  mutable geo::CPtrArray<geo::CLine> m_lines;

  // Create the m_hull, m_vcGridPoints and updates the displaylist
  virtual void OnCreateMeshGrid();
  // Returns true when the mesh region invalid. Use OnCreateMeshGrid to validate the meshregion.
  virtual bool IsInvalid() const;

public:
  // Calling invalidate will reset poly
  virtual void Invalidate();

  // The min and max of the mesh region ...
  geo::CPoint Min() const;
  geo::CPoint Max() const;

  // Creates a valid hull, when the region is invalid ...
  const geo::CPolygon &Hull() const;

  virtual const geo::IObject &DisplayList(int nIndex) const;
  virtual int DisplayListSize() const;

  bool InsideRegion(const geo::IPoint &pt, bool bIncludeEdge) const;

  // The min and max calculation ....
  int IsDangling(const CHexaMeshRegionBase &meshzone, const geo::IPoint &point, bool check_region) const;

  // Construction / Destruction
  CHexaMeshRegionBase(CFemAppModel &model);
  CHexaMeshRegionBase(const QString &sName, CFemAppModel &model);
  CHexaMeshRegionBase(const CHexaMeshRegionBase &region);
  virtual ~CHexaMeshRegionBase();

  // Assignment
  CHexaMeshRegionBase &operator=(const CHexaMeshRegionBase &rhs);
  bool operator==(const CHexaMeshRegionBase &rhs) const;

  virtual double GridSizeX() const = 0;
  virtual double GridSizeY() const = 0;

  const geo::CArray<geo::CPoint> &MeshGrid() const;

  // Assignment and equality
  virtual bool Destroy();

  // Load and save
  virtual bool Empty() const;

  void AddZone(geo::CArray<geo::CPoint> points);

  const geo::CArray<geo::CPoint> &GetMeshRegion();

  virtual bool CanDestroy() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaMeshRegionBase);
};

class CHexaMainMeshRegion : public CHexaMeshRegionBase {
  double m_size_x, m_size_y;
  CHexaBoundary *m_pBoundary;

protected:
  virtual void OnCreateMeshGrid();

public:
  // Constructor
  CHexaMainMeshRegion(CModelBase &model, const double &dXValue, const double &dYValue);
  CHexaMainMeshRegion(CModelBase &model);
  CHexaMainMeshRegion(const CHexaMainMeshRegion &rhs);

  // Assignment / Equal
  CHexaMainMeshRegion &operator=(const CHexaMainMeshRegion &rhs);
  bool operator==(const CHexaMainMeshRegion &rhs) const;

  void GetElemPoints(int IndexX, int IndexY, std::vector<geo::CPoint> &vec) const;
  void GetElemPoints(const geo::IPoint &point, std::vector<geo::CPoint> &vec) const;
  bool OnMainGrid(const geo::IPoint &p) const;

  virtual double GridSizeX() const;
  virtual double GridSizeY() const;
  void SetMainGrid(const double &iXValue = 1, const double &iYValue = 1);

  virtual void Invalidate();

  int NrOfElemX() const;
  int NrOfElemY() const;

  virtual unsigned int IconId() const;
  virtual bool CanDestroy() const;

  // Save and load

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  CHexaBoundary *getBoundary() const;

  static CHexaMainMeshRegion *getHexaMainMeshRegion(const CModelBase &modelBase);

  ACCEPT_GEOMECMODELVISITORS(VisitHexaMainMeshRegion);
};

class CHexaSubMeshRegion : public CHexaMeshRegionBase {
  int m_exp_x, m_exp_y;
  CHexaMainMeshRegion *m_pMain;
  geo::CPolygon m_polygon;
  geo::CPtrArray<geo::CLine> m_polygonLines;

  typedef std::map<geo::CPoint, std::vector<geo::CLine>> TPointToEdgeMap;
  typedef std::set<geo::CPoint> TPointSet;
  typedef std::set<geo::CLine> TEdgeSet;

  bool IsValidNextPoint(const geo::IPoint &point);
  void RecalcMeshRegion();

  typedef struct _FormationInfo {
    int gridX;
    int gridY;
    int gridZ;
    _FormationInfo(int x, int y, int z) : gridX(x), gridY(y), gridZ(z) {}
    bool operator==(const _FormationInfo &rhs) const {
      return gridX == rhs.gridX && gridY == rhs.gridY && gridZ == rhs.gridZ;
    }
  } TFormationInfo;
  typedef std::map<CHexaFormation *, TFormationInfo> TFormationInfoMap;
  TFormationInfoMap m_formations;

protected:
  virtual void OnCreateMeshGrid();

public:
  // Construction / Destruction
  CHexaSubMeshRegion(CModelBase &model, const geo::CPolygon &poly, int nExpX = 1, int nExpY = 1);
  CHexaSubMeshRegion(CModelBase &model);
  CHexaSubMeshRegion(const CHexaSubMeshRegion &region);
  virtual ~CHexaSubMeshRegion();

  virtual void Invalidate();

  void ClearFormations();
  void Formation(CHexaFormation &formation, int elements);
  int Formation(CHexaFormation &formation);

  bool FullModel() const;

  // Assignment / Equal
  CHexaSubMeshRegion &operator=(const CHexaSubMeshRegion &rhs);
  bool operator==(const CHexaSubMeshRegion &rhs) const;

  const geo::CPolygon &Polygon() const;
  void Polygon(const geo::CPolygon &polygon);

  int ExpX() const;
  int ExpY() const;
  void SetSubGrid(int ExpX, int ExpY);

  virtual double GridSizeX() const;
  virtual double GridSizeY() const;

  void GetElemPoints(const geo::IPoint &point, std::vector<geo::CPoint> &vec) const;
  void DefineMesh(const geo::CPolygon &polygon, bool includeIntersecting = false);

  virtual unsigned int IconId() const;

  // Save and load
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;

  // Main region
  const CHexaMainMeshRegion &Main() const;
  CHexaMainMeshRegion &Main();

  ACCEPT_GEOMECMODELVISITORS(VisitHexaSubMeshRegion);
};

class CHexaMeshRegionEntry : public CGraphEntryTemp<CHexaMeshRegionBase> {
public:
  CHexaMeshRegionEntry(C3DModel &model);
  const CHexaMainMeshRegion &Main() const;
  CHexaMainMeshRegion &Main();

  ACCEPT_GEOMECMODELVISITORS(VisitHexaMeshRegionEntry);
};

typedef CSingleCommandTemplate<CHexaMeshRegionEntry> THexaMeshRegionEntryCommand;

#endif // !defined(AFX_MESHREGION_H__3ADCC8B8_44AC_41B7_95A1_E449C4CA86FA__INCLUDED_)
