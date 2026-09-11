#ifndef _IWELLMESH_H_
#define _IWELLMESH_H_

class IProgressBase;

#include "BoundaryInterfaceDef.h"
#include "MeshBase.h"

class IWellModel;
class CWellFormationVolume;
class CWellFormation;
class CNewWellPoint;

namespace geo {
class CInterfaceElement;
class IFace;
class CHexahedron;
} // namespace geo

class CProgressDlg_MFC;

namespace well {
class CWellPoint;
}

#include "IMesh3D.h"

class IWellMesh : public CMeshBase {
public:
  IWellMesh(const QString &sName, IWellModel &model);
  IWellMesh(const IWellMesh &rhs);
  virtual ~IWellMesh();

  IWellMesh &operator=(const IWellMesh &rhs);
  bool operator==(const IWellMesh &rhs) const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  virtual DIMENSION Dimension() const;
  virtual const CHorizonBase *SlipHorizon(const geo::IElement &element) const;

  virtual const geo::IMesh &Mesh() const;
  virtual geo::IMesh &Mesh();

  virtual void InvalidateMesh();

  void SwitchToTetraMesh();
  void SwitchToHexaMesh();

  const CWellFormationVolume &FormationVolume(const CWellFormation &formation) const;
  CWellFormationVolume &FormationVolume(CWellFormation &formation);
  bool HasFormationVolume(const CWellFormation &formation) const;

  virtual geo::CVector WellDirectionAt(const geo::IElement &element) const;

  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  const geo::IFace &BodyFace(const geo::CInterfaceElement &iface) const;

  int DefinitionPointSize() const;
  well::CWellPoint &DefinitionPoint(int i) const;
  // wjrx mantis 3401
  int NewDefinitionPointSize() const;
  const CNewWellPoint &NewDefinitionPoint(int i) const;

  geo::CMatrix Create3DGridTransformationMatrix(const geo::IPoint &point, const geo::IVector &vecNormal) const;

  ACCEPT_GEOMECMODELVISITORS(VisitIWellMesh);

  typedef std::pair<double, std::vector<geo::CPoint>> T3DGrid; // TMD and grid points
  typedef std::set<const CFormationBase *> TFormations;

protected:
  typedef std::vector<int> TIndexVec;

  class CBoundaryElementDef {
  public:
    CBoundaryElementDef(const geo::IFace &face, const std::vector<int> &vcNodes,
                        CBoundaryInterfaceDef::TBoundarySurface surfacedef)
        : m_pFace(&face), m_vcNodes(vcNodes), m_surfacedef(surfacedef) {}

    const geo::IFace &Face() const { return *m_pFace; }
    const std::vector<int> &Nodes() const { return m_vcNodes; }
    CBoundaryInterfaceDef::TBoundarySurface SurfaceDef() const { return m_surfacedef; }

  private:
    const geo::IFace *m_pFace;
    std::vector<int> m_vcNodes;
    CBoundaryInterfaceDef::TBoundarySurface m_surfacedef;
  };
  typedef std::vector<CBoundaryElementDef> TBoundaryElementVec;

  struct SegmentInfo {
    bool bFormationTop;
    bool bFormationBottom;
    bool bModelStart;
    bool bModelEnd;
  };

protected:
  void CreateDefinitionPointList(double dTargetElementSize);
  void ClearDefinitionPointList();
  virtual bool Create2DGrid(IProgressBase &prog) = 0;
  void CreateTetraMesh(CModelBase &model, const std::vector<geo::CPoint> &points, double radius, double eltSize);
  void wellCreateMesh(const QString &primaryTitle);
  bool Create3DMesh(IProgressBase &prog, TFormations &formations);
  bool Create3DGrids(std::vector<T3DGrid> &vc3DGrids, IProgressBase &prog) const;
  void SaveFormationVolumes(TSTREAM &stream, TPROGRESS &progress);
  void LoadFormationVolumes(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);

  geo::CBodyGroup *getBodyGroup4Formation(IWellModel &wellModel, const CFormationBase *formationBase);
  geo::CBodyGroup *findBodyGroupContainingMidPointOfHexahedron(IWellModel &wellModel, geo::CBodyGroup *bodyGroup,
                                                               const CFormationBase *formation,
                                                               const geo::CHexahedron *hexahedron);

  virtual void Create3DGridAt(const geo::IPoint &point, const geo::IVector &vecNormal, T3DGrid &vcGrid3D) const = 0;
  virtual void RegisterGridNodes(const T3DGrid &vc3DGrid, std::vector<int> &vcGridNodeIndices, bool bModelStart,
                                 bool bModelEnd) = 0;
  virtual void CreateSupports(const std::vector<int> &vcGrid, bool bModelStart, bool bModelEnd) = 0;
  virtual void CreateHexaElements(const std::vector<int> &vcGrid1, const std::vector<int> &vcGrid2, double uppertmd,
                                  double lowertmd, TBoundaryElementVec &vcBoundaryElements,
                                  const SegmentInfo &segmentinfo) = 0;

  virtual void RegisterTetraBoundaryNodes(const std::map<int, int> &boundaryNodes);

  bool meshing() const;

  bool m_bMeshing;

private:
  void SwapGrids(TIndexVec **vcFirst, TIndexVec **vcSecond);
  bool CreateBoundaryElements(const TBoundaryElementVec &vcBoundaryElements, IProgressBase &prog);
  bool AssignTetraVolumes(IProgressBase &prog);
  bool AssignTetraBoundaries(std::vector<int> &boundaryNodes, IProgressBase &prog);
  bool AssignTetraSupports(const std::vector<int> &boundaryNodes, IProgressBase &prog);

private:
  geo::CMesh3D *m_mesh;

  std::vector<well::CWellPoint *> m_vcDefPoints;
  std::vector<CNewWellPoint> m_vcNewDefPoints; // wjrx mantis 3401, no *

  typedef std::map<CWellFormation *, CWellFormationVolume *> TFormationVolumeMap;
  TFormationVolumeMap m_mpFormationVolumes;
};

#endif // _IWELLMESH_H_
