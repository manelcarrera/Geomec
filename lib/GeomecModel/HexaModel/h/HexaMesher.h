#pragma once

#include "IHexaMesher.h"

#include <QString>
#include <map>

#include "Vector.h"

namespace geo {
class INode;
class IPoint;
class CInterfaceElement;
} // namespace geo

class IProgressBase;
class CHexaMesh;
class CModelBase;
class CHexaMeshRegionBase;
class CHexaHorizon;
class CRotatedSystem;
class CHexaFormation;

#include "CoordinateMap.h"
#include "StorageNode.h"
#include "VectorTempl.h"

class CHexaMesher : public IHexaMesher {
  typedef std::pair<int, int> TMasterPair;
  typedef std::map<int, TMasterPair> TTyingMap;

  geo::CArray<geo::CPoint> m_aMeshGrid; // Upper grid points (2D)
  std::vector<int> m_pPoints;           // All points outside formations for all formations. Unique points
  std::vector<int> m_pPointsInternal;   // All points inside a formations. Unique points
  std::vector<CHexaMeshRegionBase *> m_vcRegions;
  std::vector<int> m_pSlipPoints;

  // typedef std::map<const geo::CInterfaceElement*, CHexaHorizon*> TInterfaceMap;
  // TInterfaceMap m_mpInterfaces;

  bool CreateHorizonMeshGrid(IProgressBase &progress, QString *pErrMsg = 0);
  bool CreateHorizonMeshGrid(CHexaFormation &formation, IProgressBase &progress, QString *pErrMsg = 0);
  void CreateMeshNodes(IProgressBase &progress);
  void InsertSlipLayers(IProgressBase &progress);
  void InsertMeshNodes(IProgressBase &progress);
  void ModifyMeshNodes(IProgressBase &progress);
  void CreateHexahedronMesh(IProgressBase &progress);
  void CreateInterfaceElements(IProgressBase &progress);
  void CreateTyings(IProgressBase &progress);

  virtual bool OnGridLine(const geo::IPoint &pt1, const geo::IPoint &pt2) const;

  template <class VALUE> class C2DPointMap {
  public:
    typedef geo::CCoordinateMap<geo::CPoint, VALUE> TMap;
    typedef typename TMap::iterator iterator;
    typedef std::pair<const geo::IPoint *, VALUE> value_type;
    typedef const geo::IPoint *key_type;
    typedef VALUE mapped_type;

  public:
    C2DPointMap() {}

    std::pair<iterator, bool> insert(value_type &val) {
      return m_map.insert(typename TMap::value_type(geo::CPoint(val.first->X(), val.first->Y()), val.second));
    }

    iterator find(const key_type &key) { return m_map.find(geo::CPoint(key->X(), key->Y())); }

    mapped_type &operator[](const key_type &key) { return m_map[geo::CPoint(key->X(), key->Y())]; }

    iterator end() { return m_map.end(); }

  private:
    TMap m_map;
  };

  void LoadPointIndices(std::vector<int> &vcIndex, CArchiveInterface &stream, IProgressBase &progress);
  void SavePointIndices(const std::vector<int> &vcIndex, CArchiveInterface &stream, IProgressBase &progress);

  typedef C2DPointMap<std::vector<int>> TPointMap;

  typedef struct {
    std::vector<int> vcPoints;

    TPointMap mpPoint;
    TPointMap mpInternalPoint;

    geo::CVector Xaxis;
    geo::CVector Yaxis;

    const geo::CArray<geo::CPoint> *basicpoints;

    TTyingMap mpCreatedTyings;
  } _TyingsInfo;

  void _Tyings_SetupPoints(_TyingsInfo &info);
  void _Tyings_HandleHorizontal(_TyingsInfo &info, CRotatedSystem &rs, geo::CPoint &pp_local);
  void _Tyings_HandleVertical(_TyingsInfo &info, CRotatedSystem &rs, geo::CPoint &pp_local,
                              CHexaMeshRegionBase *pMasterRegion);

  void _Tyings_Create(_TyingsInfo &info, int index, int nMaster1, int nMaster2, double calcdist, bool do_z);
  void _Tyings_CreateForPoints(_TyingsInfo &info, CRotatedSystem &rs, geo::CPoint &pp_local, bool force_z,
                               std::vector<int> &points, TPointMap &mpPoint, int k0, int k1, double calcdist);

public:
  CHexaMesher(CHexaMesh &mesh);

  virtual int MesherType() const;

  bool CreateHorizonMeshGrid();

  virtual bool CreateMesh(IProgressBase &progress);

  virtual bool CanCreateMesh();
  virtual void Invalidate();

  virtual void LoadStream(CArchiveInterface &stream, CStreamVersion &version, IProgressBase &progress);
  virtual void SaveStream(CArchiveInterface &stream, IProgressBase &progress);

  virtual long SavedItems() const;
};
