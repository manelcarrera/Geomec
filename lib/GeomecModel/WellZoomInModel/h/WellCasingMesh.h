#ifndef _WELLCASINGMESH_H_
#define _WELLCASINGMESH_H_

#include "IWellMesh.h"
#include "ModelBase.h"
#include "IWellModel.h"

class CWellCasingModel;

namespace geo {
  class CHexahedron;
}

class CWellCasingMesh : public IWellMesh
{
public:
  CWellCasingMesh(const QString& sName, CWellCasingModel& casingmodel);
  CWellCasingMesh(const CWellCasingMesh& rhs);
  virtual ~CWellCasingMesh();

  CWellCasingMesh& operator=(const CWellCasingMesh& rhs);
  bool operator==(const CWellCasingMesh& rhs) const;

  virtual bool CanCreateMesh() const;
  virtual bool IsMesh() const;
  virtual void CreateMesh();
  virtual void InvalidateMesh();

  double CasingDiameter() const;
  void CasingDiameter(double d);

  double CasingThickness() const;
  void CasingThickness(double d);

  double HoleDiameter() const;
  void HoleDiameter(double d);

  int RadialElements() const;
  void RadialElements(int n);

  int TangentialElements() const;
  void TangentialElements(int n);

  double TargetElementSize() const;
  void TargetElementSize(double d);

  int NodesPerSpoke() const;
  int NodesPerGrid() const;
  int NumberOfGrids() const;

  int ElementsPerSegment() const;
  int ElementsPerGrid() const;
  int NumberOfElementGrids() const;

  size_t numberOfSteelElements() const;
  void numberOfSteelElements(size_t numberOfSteelElements);

  size_t numberOfCementElements() const;
  void numberOfCementElements(size_t numberOfCementElements);

  double outerDiameterRock() const;
  void outerDiameterRock(double outerDiameterRock);

  // this does not include cap interface elements
  const geo::IElement* GetElement(int iGrid, int iSegment, int iIndexOnSegment) const;

  const geo::CBodyGroup& CementElements() const;
  const geo::CBodyGroup& SteelElements() const;
  const geo::CPtrArray<geo::IElement>& CementInterfaceElements() const;

  bool IsSteelElement(const geo::IElement& element) const;
  bool IsCementElement(const geo::IElement& element) const;
  bool IsCementInterfaceElement(const geo::IElement& element) const;
  bool IsOuterInterfaceElement(const geo::IElement& element) const;

  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  void CollectBoundaryNodes(CModelBase::TNodeElementsMap& mpNodeElements) const;
  const geo::IElement& GetAdjacentCementElement(const geo::IElement& steel_element) const;

  bool getInvalidateAfterDlg() const;
  void setInvalidateAfterDlg(bool invalidateAfterDlg);

  virtual const CFFMaterial* getCFFMaterial(const geo::IElement& element,
  const CDepletionStage& depletionStage) const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingMesh);

protected:
  virtual void RegisterGridNodes(const T3DGrid& vc3DGrid, std::vector<int>& vcGridNodeIndices, bool bModelStart, bool bModelEnd);
  virtual void CreateSupports(const std::vector<int>& vcGrid, bool bModelStart, bool bModelEnd);
  virtual void CreateHexaElements(const std::vector<int>& vcGrid1, const std::vector<int>& vcGrid2, double uppertmd, double lowertmd, TBoundaryElementVec& vcBoundaryElements, const SegmentInfo& segmentinfo);
  virtual void Create3DGridAt(const geo::IPoint& point, const geo::IVector& vecNormal, T3DGrid& vcGrid3D) const;

private:
  geo::CHexahedron* createHexaElement(IWellModel::CBoundary& boundary,
  geo::CBodyGroup* bodyGroup, std::set <const geo::IElement*>* elements,
  const std::vector <int>& grid1, const std::vector <int>& grid2,
  const SegmentInfo& segmentinfo, const CFormationBase* formationBase,
  int firstSpoke, int secondSpoke, bool first, bool last,
  bool top, bool bottom, bool useActualBodyGroup);
  virtual bool Create2DGrid(IProgressBase& prog);
  void Destroy2DGrid();
  void SaveBodyGroup(const geo::CBodyGroup& bodygroup, TSTREAM& stream, TPROGRESS& progress);
  void LoadBodyGroup(geo::CBodyGroup& bodygroup, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  typedef std::pair <geo::CHexahedron*, geo::CBodyGroup*> THexahedronBodyGroupPair;

  THexahedronBodyGroupPair CreateHexahedron(const std::vector<int>& vcGrid1,
                      const std::vector<int>& vcGrid2,
                      int idxStartFirstSpoke,
                      int idxStartSecondSpoke,
                      std::set<const geo::IElement*>* pstElements,
                      geo::CBodyGroup* BodyGroup,
                      const CFormationBase* formationBase);
  geo::CInterfaceElement& CreateCapInterface(const std::vector<int>& vcGrid1,
                                             const std::vector<int>& vcGrid2,
                                             int idxStartFirstSpoke,
                                             int idxStartSecondSpoke,
                                             const geo::IFace* pFront,
                                             const geo::IFace* pBack,
                                             std::set<const geo::IElement*>& stElements);
  geo::CInterfaceElement& CreateSideInterface(const std::vector<int>& vcGrid1,
                                              const std::vector<int>& vcGrid2,
                                              int idxStartFirstSpoke,
                                              int idxStartSecondSpoke,
                                              const geo::IFace* pFront,
                                              const geo::IFace* pBack,
                                              std::set<const geo::IElement*>& stElements);
  bool ElementsAttachedToNode(const std::set<int>& stElements, int nNode) const;
  void MapNodesToElement(const geo::IElement& element);
  void BuildNodeToElementsMap() const;
  geo::CBodyGroup* findBodyGroupContainingMidPointOfHexahedron(
  IWellModel& wellModel, geo::CBodyGroup* bodyGroup,
  const CFormationBase* formation, const geo::CHexahedron* hexahedron);

public:
  typedef enum
  {
  SS_TOP    = 0,
  SS_BOTTOM = 1,
  SS_INNER  = 2,
  SS_OUTER  = 3
  } TSideSurface;

  typedef enum
  {
  HS_BOTTOM       = 0,
  HS_TANGENT_LOW  = 1,
  HS_OUTER        = 2,
  HS_TANGENT_HIGH = 3,
  HS_INNER        = 4,
  HS_TOP          = 5
  } THexaSurface;

private:
  double m_dCasingDiameter;
  double m_dCasingThickness;
  double m_dHoleDiameter;
  int m_nRadialElements;
  int m_nTangentialElements;
  double m_dTargetElementSize;
  std::vector<geo::CPoint*> m_vc2DGrid;

  geo::CBodyGroup* m_pCementGroup;
  std::set<const geo::IElement*> m_stCementElements;
  geo::CBodyGroup* m_pSteelGroup;
  std::set<const geo::IElement*> m_stSteelElements;
  std::set<const geo::IElement*> m_stCementInterfaceElements;
  geo::CPtrArray<geo::IElement>* m_pCementInterfaceGroup;
  std::set<const geo::IElement*> m_stOuterInterfaceElements;

  // the (duplicated) top and bottom grid nodes for the top and bottom interfaces
  std::vector<int> m_vcTopGrid;
  std::vector<int> m_vcBottomGrid;

  bool m_bInvalidateAfterDlg;

  // map from nodes to attached elements
  typedef std::map<int, std::set<int> > TNodeElementsMap;
  mutable TNodeElementsMap m_mpNodeElements;

  // map from steel elements to adjacent cement elements
  typedef std::map<const geo::IElement*, const geo::IElement*> TSteel2CementMap;
  TSteel2CementMap m_mpSteel2Cement;

  size_t m_numberOfSteelElements;
  size_t m_numberOfCementElements;
  double m_outerDiameterRock;
};

class CElementCenterPointCache
{
public:
  CElementCenterPointCache();

  void Create(const geo::CBodyGroup& bodygroup);
  void Create(const geo::CPtrArray<geo::IElement>& elements);
  void Clear();
  bool Empty() const;
  const geo::CArray<geo::CElementPoint>& Get() const;

private:
  geo::CArray<geo::CElementPoint> m_array;
};

#endif // _WELLCASINGMESH_H_
