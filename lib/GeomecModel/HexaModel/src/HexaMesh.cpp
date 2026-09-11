// HexaMesh.cpp: implementation of the CHexaMesh class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "BaseEntryTypes.h"
#include "ElementGroup.h"
#include "Global.h"
#include "HexaBoundary.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaHorizon.h"
#include "HexaMesh.h"
#include "HexaMesher.h"
#include "HexaMesherNG.h"
#include "HexaModel.h"
#include "Hexahedron.h"
#include "IHexaMesher.h"
#include "IPlatform.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "ISupport.h"
#include "InterfaceElement.h"
#include "ModifiedHint.h"
#include "PressureSupportNode.h"
#include "RotatedSystem.h"
#include "hexameshregion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHexaMesh::CHexaMesh(CHexaModel &model) : CMeshBase("Mesh", model.DepletionStageEntry()) {
  m_pMesher = new CHexaMesher(*this);
  // Link to point-set entry
  LinkTo(*model.GraphEntry(MD_BASE_POINTSET));
}
/*
CHexaMesh::CHexaMesh(CHexaModel& model)
: CMeshBase(model)
{
}
*/

CHexaMesh::~CHexaMesh() {
  InvalidateMesh();
  delete m_pMesher;
}

CHexaFormation &CHexaMesh::GetTopFormation() const {
  CHexaFormationEntry *pEntry = (CHexaFormationEntry *)Model().GraphEntry(MD_HEXA_FORMATION);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  assert(stNode.size() > 0);
  for (CHexaFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    if ((*it)->UpperFormation() == 0)
      return **it;
  }

  assert(false);

  CHexaFormation *pFormation = 0;
  return *pFormation;
}

//=============================================================================
// The main routine for creating a complete mesh model.
void CHexaMesh::CreateMesh() {
  // If the mesher is locked on register that the client want to mesh ....
  if (Locked()) {
    m_bReMesh = true;
    return;
  }

  assert(!IsMesh());
  bool bDidLock = false;
  if (!Locked()) {
    Lock();
    bDidLock = true;
  }
  std::auto_ptr<IProgressBase> dlg(_g->prog()->create(eProgress::Dual, "Create new mesh data...", false, 8));

  // Invalidate all mesh regions
  CHexaMeshRegionEntry *pRegionEntry = (CHexaMeshRegionEntry *)(Model().GraphEntry(MD_HEXA_MESH_ZONE));
  CHexaMeshRegionEntry::TNodeSet stNode2 = pRegionEntry->EntryNodes();
  for (CHexaMeshRegionEntry::TNodeSet::iterator itMeshRegion = stNode2.begin(); itMeshRegion != stNode2.end();
       itMeshRegion++) {
    (*itMeshRegion)->Invalidate();
  }
  //	Model().Document().UpdateAllViews(0, ON_UPDATE_CURRENT_SCENE);

#if 0
  if (m_pMesher->MesherType() == 0)
  {
  delete m_pMesher;
  m_pMesher = new CHexaMesherNG(*this);
  }
#endif

  bool success = m_pMesher->CreateMesh(*dlg);

  if (bDidLock)
    Unlock();

  if (!success)
    return;

  m_mpMeshNode2BoundaryNode.clear();
  m_mpBoundaryNode2MeshNode.clear();

  // Is the mesh pushed down? Correct mesh depth
  CModelBase &model = dynamic_cast<CModelBase &>(Model());
  model.Boundary().CorrectMeshDepth(m_mesh.Min().Z(), m_mesh.Max().Z());

  Modified(MeshCreated);
}

// Create the supportlists for the X, Y and Z directions.
// Nodes on an edge, which connects 2 boundaries, will be supported in both directions.
// The lower 4 corner points will be supported in all 3 directions.
// The topnodes, except for the edges, will not be supported.
void CHexaMesh::CreateSupport() {
  std::auto_ptr<IProgressBase> progress(_g->prog()->create(eProgress::Geo, "Create supports.."));

  CreateSupport(*progress);
}

// Create the supportlists for the X, Y and Z directions.
// Nodes on an edge, which connects 2 boundaries, will be supported in both directions.
// The lower 4 corner points will be supported in all 3 directions.
// The topnodes, except for the edges, will not be supported.
void CHexaMesh::CreateSupport(IProgressBase &progress) {
  geo::CPoint minPt;
  geo::CPoint maxPt;
  minPt = Mesh().Min();
  maxPt = Mesh().Max();

  assert(Mesh().PointSize() > 0);

  CHexaModel &model = static_cast<CHexaModel &>(Model());
  CHexaBoundary &boundary = static_cast<CHexaBoundary &>(model.Boundary());

  std::vector<const geo::ISurface *> vcSurfaces = boundary.GetSideMeshSurfaces();

  // Do progress measurement and create set
  for (size_t i = 0; i < vcSurfaces.size(); ++i) {
    for (int j = 0; j < vcSurfaces[i]->FaceSize(); ++j)
      progress.AddSteps(vcSurfaces[i]->Face(j).NrOfNodes());
  }

  const C3DHorizon &bottom = static_cast<const C3DHorizon &>(boundary.GetBottomHorizon());
  const C3DHorizon &top = static_cast<const C3DHorizon &>(boundary.GetTopHorizon());

  for (int i = 0; i < bottom.MeshedSurfaceSize(); i++) {
    for (int j = 0; j < bottom.MeshedSurface(i).FaceSize(); ++j) {
      progress.AddSteps(bottom.MeshedSurface(i).Face(j).NrOfNodes());
    }
  }

  if (boundary.CreateInterfaces()) {
    for (int i = 0; i < top.MeshedSurfaceSize(); ++i) {
      for (int j = 0; j < top.MeshedSurface(i).FaceSize(); ++j) {
        progress.AddSteps(top.MeshedSurface(i).Face(j).NrOfNodes());
      }
    }
  }
  /*
    typedef std::set<const geo::INode *> TMeshNodeSet;
    TMeshNodeSet HorizonNodes;
    TMeshNodeSet BoundaryNodes;
  */
  TSupportMap mpSupportMap;

  for (size_t i = 0; i < vcSurfaces.size(); ++i) {
    const geo::ISurface &surface = *vcSurfaces[i];
    if (boundary.CreateInterfaces()) {
      CreateBoundaryInterfaceElements(surface, mpSupportMap, progress);
    } else {
      CreateStandardSupports(surface, mpSupportMap, progress);
    }
  }
  /*
    // Create support
    for(TMeshNodeSet::iterator it = BoundaryNodes.begin(); it != BoundaryNodes.end(); ++it)
    {
    const geo::INode& node = **it;
      assert(fabs(node.X() - minPt.X()) < EPS || fabs(node.X() - maxPt.X()) < EPS ||
             fabs(node.Y() - minPt.Y()) < EPS || fabs(node.Y() - maxPt.Y()) < EPS);

      m_mesh.CreateTranslationSupport(node.Index(), geo::CVector::Xaxis);
      m_mesh.CreateTranslationSupport(node.Index(), geo::CVector::Yaxis);
    }
  */
  for (int i = 0; i < top.MeshedSurfaceSize(); ++i) {
    const geo::ISurface &surface = top.MeshedSurface(i);
    if (boundary.CreateInterfaces()) {
      CreateBoundaryInterfaceElements(surface, mpSupportMap, progress);
    } else {
      CreateStandardSupports(surface, mpSupportMap, progress);
    }
  }

  for (int i = 0; i < bottom.MeshedSurfaceSize(); i++) {
    const geo::ISurface &surface = bottom.MeshedSurface(i);
    if (boundary.CreateInterfaces()) {
      CreateBoundaryInterfaceElements(surface, mpSupportMap, progress);
    } else {
      CreateStandardSupports(surface, mpSupportMap, progress);
    }
  }

  // create the actual supports
  for (TSupportMap::iterator it = mpSupportMap.begin(); it != mpSupportMap.end(); ++it) {
    int nIndex = it->first;
    for (geo::CCoordinateSet<geo::CVector>::iterator itv = it->second.begin(); itv != it->second.end(); ++itv)
      m_mesh.CreateTranslationSupport(nIndex, *itv);
  }

  // fully support model's reference point
  // find the reference point in mesh node terms
  int iRefNode = ReferencePointIndex();
  assert(iRefNode >= 0);
  m_mesh.CreateTranslationSupport(iRefNode, geo::CVector::Xaxis);
  m_mesh.CreateTranslationSupport(iRefNode, geo::CVector::Yaxis);
  m_mesh.CreateTranslationSupport(iRefNode, geo::CVector::Zaxis);
}

void CHexaMesh::CreateStandardSupports(const geo::ISurface &surface, TSupportMap &mpSupportMap,
                                       IProgressBase &progress) {
  CHexaModel &model = static_cast<CHexaModel &>(Model());
  CHexaBoundary &boundary = static_cast<CHexaBoundary &>(model.Boundary());

  CPressureSupportNode::CFaceSupportDef::TSupportType suptype =
      boundary.PressureSupportNode().FaceSupportDef(CPressureSupportNode::CSurfaceDef(surface)).SupportType();

  for (int j = 0; j < surface.FaceSize(); ++j) {
    for (int n = 0; n < surface.Face(j).NrOfNodes(); ++n) {
      TSupportMap::iterator itm =
          mpSupportMap
              .insert(TSupportMap::value_type(surface.Face(j).Node(n).Index(), geo::CCoordinateSet<geo::CVector>()))
              .first;

      switch (suptype) {
      case CPressureSupportNode::CFaceSupportDef::ST_NORMAL:
        itm->second.insert(surface.AverageNormalInPoint(surface.Face(j).Node(n)));
        break;
      case CPressureSupportNode::CFaceSupportDef::ST_FULL:
        itm->second.insert(geo::CVector::Xaxis);
        itm->second.insert(geo::CVector::Yaxis);
        itm->second.insert(geo::CVector::Zaxis);
        break;
      case CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL:
        itm->second.insert(geo::CVector::Xaxis);
        itm->second.insert(geo::CVector::Yaxis);
        break;
      case CPressureSupportNode::CFaceSupportDef::ST_VERTICAL:
        itm->second.insert(geo::CVector::Zaxis);
        break;
      case CPressureSupportNode::CFaceSupportDef::ST_STRESS:
      case CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED:
        break;
      }

      progress.Step();
    }
  }
}

void CHexaMesh::CreateBoundaryInterfaceElements(const geo::ISurface &surface, TSupportMap &mpSupportMap,
                                                IProgressBase &progress) {
  for (int j = 0; j < surface.FaceSize(); ++j) {
    const geo::IFace &face = surface.Face(j);
    assert(dynamic_cast<const geo::CBodyQuadrilateral *>(&face));
    CreateBoundaryInterfaceElement(face, mpSupportMap, progress);
  }
}

void CHexaMesh::CreateBoundaryInterfaceElement(const geo::IFace &face, TSupportMap &mpSupportMap,
                                               IProgressBase &progress) {
  std::vector<int> vcNodeIndex(2 * face.NrOfNodes());
  for (int n = 0; n < face.NrOfNodes(); ++n) {
    const geo::INode &node = face.Node(n);
    vcNodeIndex[n] = node.Index();
    TBoundaryNodeMap::iterator it = m_mpMeshNode2BoundaryNode.find(&node);
    if (it == m_mpMeshNode2BoundaryNode.end()) {
      int nIndex = m_mesh.RegisterNode(node, false);
      const geo::INode &boundary_node = m_mesh.Node(nIndex);
      std::pair<TBoundaryNodeMap::iterator, bool> prInsert =
          m_mpMeshNode2BoundaryNode.insert(TBoundaryNodeMap::value_type(&node, &boundary_node));
      assert(prInsert.second);
      it = prInsert.first;
      VERIFY(m_mpBoundaryNode2MeshNode.insert(TBoundaryNodeMap::value_type(&boundary_node, &node)).second);
    }

    assert(it != m_mpMeshNode2BoundaryNode.end());
    TSupportMap::iterator itm =
        mpSupportMap.insert(TSupportMap::value_type(it->second->Index(), geo::CCoordinateSet<geo::CVector>())).first;

    itm->second.insert(geo::CVector::Xaxis);
    itm->second.insert(geo::CVector::Yaxis);
    itm->second.insert(geo::CVector::Zaxis);

    vcNodeIndex[face.NrOfNodes() + n] = it->second->Index();
    progress.Step();
  }

  CModelBase &model = static_cast<CModelBase &>(Model());
  CInterfaceBoundary &bound = static_cast<CInterfaceBoundary &>(model.Boundary());
  geo::CInterfaceElement *pIface = new geo::CInterfaceElement(m_mesh, &face, &face, vcNodeIndex);
  bound.AddInterfaceElement(*pIface);
  assert(bound.IsBoundaryInterface(*pIface));
}

//=============================================================================
bool CHexaMesh::CanCreateMesh() const {
  if (Locked())
    return false;

  // Formations?
  CHexaFormationEntry *pEntry = (CHexaFormationEntry *)(((CFemAppModel &)Model()).GraphEntry(MD_HEXA_FORMATION));
  if (pEntry) {
    CHexaFormationEntry::TNodeSet stFormation = pEntry->EntryNodes();

    // No formations -> no mesh.
    if (stFormation.size() == 0)
      return false;

    if (m_pMesher->CanCreateMesh())
      return true;
  }
  return false;
}

bool CHexaMesh::IsMesh() const { return m_mesh.ElementSize() > 0; }

void CHexaMesh::InvalidateMesh() {
  if ((static_cast<const CModelBase &>(Model())).BranchState().IsBranch()) {
    IPlatform *platform = IPlatform::instance();

    platform->trace("WARNING: CHexaMesh::InvalidateMesh called in branched state, ignoring call\n");
    return;
  }

  COperation operation(Model());
  // Do nothing when
  if (!IsMesh()) {
    Modified(MeshCleared);
    return;
  }

  // During the invalidation procudure a lot of model item will automatically
  // ask for remeshing. We collect those calls and set a remesh flag when such
  // a request is done ...
  m_bReMesh = false;
  Lock();

  CMeshBase::InvalidateMesh();

  // Clear point vectors
  m_pMesher->Invalidate();
  m_mesh.Clear();
  m_mpInterfaces.clear();

  CHexaHorizonEntry *pEntry = (CHexaHorizonEntry *)(Model().GraphEntry(MD_HEXA_HORIZON));
  if (pEntry) // When model is destructed during CHexaModel::CloseModel() the horizon entry is not present
  {
    CHexaHorizonEntry::THorizonVec stNode = pEntry->Horizons();
    for (CHexaHorizonEntry::THorizonVec::iterator itHorizon = stNode.begin(); itHorizon != stNode.end(); itHorizon++) {
      (*itHorizon)->ClearMaterials();
      (*itHorizon)->Modified(MeshCleared);
    }
  }

  assert(!IsMesh());

  Modified(MeshCleared);
  assert(Locked());

  // Note : When we go here the mesh could be valid, because in the Modified routine the
  //		  mesh could be recreated ...

  Unlock();

  if (m_bReMesh)
    CreateMesh();
}

std::string CHexaMesh::CurrentMesher() const {
  if (dynamic_cast<CHexaMesherNG *>(m_pMesher))
    return "NextGen";
  return "";
}
void CHexaMesh::SwitchMesher(std::string type) {
  if (type != CurrentMesher()) {
    InvalidateMesh();
    delete m_pMesher;

    if (type == "NextGen")
      m_pMesher = new CHexaMesherNG(*this);
    else
      m_pMesher = new CHexaMesher(*this);

    assert(type == CurrentMesher());
  }
}

void CHexaMesh::InvalidateSupports() { m_mesh.ClearSupports(); }

void CHexaMesh::GenerateSupports() {
  if (m_mesh.SupportSize() == 0)
    CreateSupport();
}

int CHexaMesh::DisplayListSize() const { return 1; }

const geo::IObject &CHexaMesh::DisplayList(int /*nIndex*/) const { return m_mesh; }

CHexaMesh::DIMENSION CHexaMesh::Dimension() const { return CHexaMesh::DIM_3D; }

unsigned int CHexaMesh::IconId() const { return IDI_MESH; }

unsigned int CHexaMesh::TypeId() const { return 0; }

// Stream
void CHexaMesh::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Unlink from the node entries
  UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
  UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));

  if (version > CStreamVersion(4, 1, 62)) {
    int type;
    stream >> type;

    if (type == IHexaMesher::NEXT_GEN) {
      delete m_pMesher;
      m_pMesher = new CHexaMesherNG(*this);
    }
  }

  CMeshBase::LoadStream(stream, version, progress);
  progress.StatusMessage("Loading Mesh");

  bool bInvalidateAtUnlock = false;

  if (version < CStreamVersion(3, 0, 8)) {
    old_load_stream(stream, version, progress);
    // these versions can have wrong meshes, due to surface increase problems
    // remesh after load
    InvalidateAtUnLock();
  } else {
    if (version > CStreamVersion(4, 1, 60) && version < CStreamVersion(4, 1, 63)) {
      int type;
      stream >> type;

      if (type == IHexaMesher::NEXT_GEN) {
        delete m_pMesher;
        m_pMesher = new CHexaMesherNG(*this);
      }
    }

    // From version 3.0.8 load points first ...
    int nPointSize;
    stream >> nPointSize;
    for (int nPoint = 0; nPoint < nPointSize; nPoint++) {
      double X, Y, Z;
      stream >> X;
      stream >> Y;
      stream >> Z;
      VERIFY(m_mesh.RegisterNode(geo::CPoint(X, Y, Z), false) == nPoint);
      progress.Step();
    }

    // Load the point indices for the points, the internal points and slippoints
    m_pMesher->LoadStream(stream, version, progress);

    if (m_pMesher->MesherType() == IHexaMesher::OLD_MESHER) {
      int nMaxPoints = m_pMesher->SavedItems(); // KLUGE
      for (int i = nPointSize - 1; i >= nMaxPoints; --i)
        m_mesh.RemoveNode(i);
    }

    // Load the element grid
    int nElementSize;
    stream >> nElementSize;
    for (int nElement = 0; nElement < nElementSize; nElement++) {
      assert(nElement == m_mesh.ElementSize());
      // Read element type
      int bInterfaceElement;
      stream >> bInterfaceElement;

      // Read indices
      if (bInterfaceElement) {
        int nBody;
        // Read front (top) body and back (bottom) body
        stream >> nBody;
        const geo::IBody &front = dynamic_cast<const geo::IBody &>(m_mesh.Element(nBody));
        stream >> nBody;
        const geo::IBody &back = dynamic_cast<const geo::IBody &>(m_mesh.Element(nBody));

        int nFrontFaceIndex = HEXA_LOWERFACE;
        int nBackFaceIndex = HEXA_UPPERFACE;

        if (version >= CStreamVersion(4, 1, 2)) {
          stream >> nFrontFaceIndex;
          stream >> nBackFaceIndex;
        }

        // new for version 3.0.16: save bodyface start node indices
        int nFrontStartIndex;
        int nBackStartIndex;

        if (version < CStreamVersion(3, 0, 16)) {
          // inconsistent mesh save
          // mesh must be invalidated after load
          // use dummy indices for now
          nFrontStartIndex = 0;
          nBackStartIndex = 0;
        } else {
          stream >> nFrontStartIndex;
          stream >> nBackStartIndex;
        }

        if (version < CStreamVersion(3, 0, 18)) {
          // these versions can have wrong meshes, due to surface increase problems
          // remesh after load
          InvalidateAtUnLock();
        }

        // Read connectivity
        std::vector<int> vcIndex(8);
        for (size_t nPoint = 0; nPoint < vcIndex.size(); nPoint++)
          stream >> vcIndex[nPoint];

        new geo::CInterfaceElement(m_mesh, &front.Face(nFrontFaceIndex), &back.Face(nBackFaceIndex), vcIndex);
      } else {
        std::vector<int> vcIndex(8);
        for (size_t nPoint = 0; nPoint < vcIndex.size(); nPoint++)
          stream >> vcIndex[nPoint];
        new geo::CHexahedron(m_mesh, vcIndex);
      }
    }

    // Load body groups if there is a mesh
    if (IsMesh()) {
      CHexaFormation *pFormation = &GetTopFormation();
      while (pFormation) {
        // Create a body group with 2 side surfaces (upper and lower) and a formation volume
        assert(pFormation->VolumeSize() == 1);
        geo::CBodyGroup *pGroup = new geo::CBodyGroup(m_mesh);
        VERIFY(pFormation->Volume(0).Volume(*pGroup));
        LoadBodyGroup(*pGroup, stream, version, progress);

        // Load slip horizon
        if (!pFormation->UpperFormation()) {
          if (pFormation->UpperHorizon().Slip())
            LoadSlipHorizon((CHexaHorizon &)pFormation->UpperHorizon(), stream, progress);
        }

        if (pFormation->LowerHorizon().Slip())
          LoadSlipHorizon((CHexaHorizon &)pFormation->LowerHorizon(), stream, progress);

        pFormation = const_cast<CHexaFormation *>(pFormation->LowerFormation());
      }
      if (m_pMesher->MesherType() == IHexaMesher::OLD_MESHER) {
        if (!static_cast<CHexaMesher *>(m_pMesher)->CreateHorizonMeshGrid())
          bInvalidateAtUnlock = true;
      } else {
        if (!m_pMesher->ValidRegions())
          bInvalidateAtUnlock = true;
      }
    }
  }

  if (!IsMesh() && !CanCreateMesh()) {
    // create a fake hexa, so IsMesh returns true
    std::vector<const geo::IPoint *> vcPoints(8);
    geo::CPoint pt0(0, 0, 0);
    geo::CPoint pt1(1, 0, 0);
    geo::CPoint pt2(1, 1, 0);
    geo::CPoint pt3(0, 1, 0);
    geo::CPoint pt4(0, 0, 1);
    geo::CPoint pt5(1, 0, 1);
    geo::CPoint pt6(1, 1, 1);
    geo::CPoint pt7(0, 1, 1);
    vcPoints[0] = &pt0;
    vcPoints[1] = &pt1;
    vcPoints[2] = &pt2;
    vcPoints[3] = &pt3;
    vcPoints[4] = &pt4;
    vcPoints[5] = &pt5;
    vcPoints[6] = &pt6;
    vcPoints[7] = &pt7;
    new geo::CHexahedron(m_mesh, vcPoints);
    InvalidateMesh();
  }

  if (version < CStreamVersion(3, 0, 25) || bInvalidateAtUnlock) {
    InvalidateAtUnLock();
  } else {
    if (IsMesh()) {
      CreateSupport();
      m_pMesher->CreateTyings();
      CModelBase &model = dynamic_cast<CModelBase &>(Model());
      model.Boundary().CorrectMeshDepth(m_mesh.Min().Z(), m_mesh.Max().Z());
    }
  }

  if (version >= CStreamVersion(3, 7, 17))
    LoadValueSets(stream, version, progress);

  Modified();
}

void CHexaMesh::old_load_stream(TSTREAM &stream, CStreamVersion & /*version*/, TPROGRESS &progress) {
  long lPoints;
  long lPointsInternal;
  long lSlipPoints;
  stream >> lPoints;
  stream >> lPointsInternal;
  stream >> lSlipPoints;

  // NOTE: we invalidate this mesh after reading it, so we don't need to do everything
  //       1. We skip creating the horizon mesh grid
  //       2. We skip populating the m_p(Slip)Points(Internal) vectors
  //       All of these have been moved to CHexaMesher and we want to decouple

  // if (lPoints > 0)
  //	m_pMesher->CreateHorizonMeshGrid();  MOVED

  std::map<int, int> mpFileToMesh; // File Index
  double X, Y, Z;
  // Read the points and register

  long lCount;
  for (lCount = 0; lCount < lPoints; lCount++) {
    progress.Step();
    stream >> X >> Y >> Z;
    int nIndex = m_mesh.RegisterNode(geo::CPoint(X, Y, Z), false);
    mpFileToMesh.insert(std::make_pair(lCount, nIndex));
    // m_pPoints.push_back(nIndex);  MOVED
  }

  // Read the internal points
  for (lCount = 0; lCount < lPointsInternal; lCount++) {
    progress.Step();
    stream >> X >> Y >> Z;
    int nIndex = m_mesh.RegisterNode(geo::CPoint(X, Y, Z), false);
    mpFileToMesh.insert(std::make_pair(lCount + lPoints, nIndex));
    // m_pPointsInternal.push_back(nIndex);  MOVED
  }
  for (lCount = 0; lCount < lSlipPoints; lCount++) {
    progress.Step();
    stream >> X >> Y >> Z;
    int nIndex = m_mesh.RegisterNode(geo::CPoint(X, Y, Z), false);
    mpFileToMesh.insert(std::make_pair(lCount + lPoints + lPointsInternal, nIndex));
    // m_pSlipPoints.push_back(nIndex);  MOVED
  }

  // Read the body size
  long lBodySize;
  stream >> lBodySize;
  // If we have bodies create a volume for each formation
  CHexaFormation *pFormation = &GetTopFormation();
  while (pFormation) {
    assert(pFormation->VolumeSize() == 1);
    // Create a body group and a volume
    geo::CBodyGroup *pGroup = new geo::CBodyGroup(m_mesh);
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 0
    new geo::CBodyGroup::CSideSurface(*pGroup); // SideSurface 1
    VERIFY(pFormation->Volume(0).Volume(*pGroup));
    pFormation = (CHexaFormation *)pFormation->LowerFormation();
  }

  std::map<int, geo::CHexahedron *> allElements;
  for (lCount = 0; lCount < lBodySize; lCount++) {
    // Read the hexahedron
    std::vector<int> vcHexahedron(8);
    for (int lPoint = 0; lPoint < 8; lPoint++) {
      // Read indices for the hexahedron and convert to mesh numbering system
      int nFileIndex;
      stream >> nFileIndex;
      std::map<int, int>::const_iterator it = mpFileToMesh.find(nFileIndex);
      assert(it != mpFileToMesh.end());
      vcHexahedron[lPoint] = it->second;
    }

    geo::CHexahedron *hex = new geo::CHexahedron(Mesh(), vcHexahedron);
    allElements.insert(std::make_pair(lCount, hex));

    int FormIndex;
    stream >> FormIndex;
    // Now find the formation
    CHexaFormationEntry &formation_entry = (CHexaFormationEntry &)*Model().GraphEntry(MD_HEXA_FORMATION);
    CHexaFormation *pForm = formation_entry.FindIndex(FormIndex);
    assert(pForm && (pForm->VolumeSize() == 1));
    pForm->Volume(0).Volume().AddBody(*hex);
    progress.Step();
  }

  // Read interface elements from disc ....
  stream >> lBodySize;
  for (lCount = 0; lCount < lBodySize; lCount++) {
    std::vector<int> vcHexahedron(8);
    for (int lPoint = 0; lPoint < 8; lPoint++) {
      // Read file tag
      int nFileIndex;
      stream >> nFileIndex;
      std::map<int, int>::const_iterator it = mpFileToMesh.find(nFileIndex);
      assert(it != mpFileToMesh.end());
      vcHexahedron[lPoint] = it->second;
    }
    int HorIndex;
    stream >> HorIndex;
    int iLower, iUpper;
    stream >> iLower >> iUpper;
    CHexaHorizonEntry &horizon_entry = (CHexaHorizonEntry &)*Model().GraphEntry(MD_HEXA_HORIZON);
    CHexaHorizon *pHorizon = horizon_entry.FindIndex(HorIndex);
    assert(pHorizon);
    geo::CElementGroup *pGroup = const_cast<geo::CElementGroup *>(pHorizon->InterfaceElementGroup());
    if (pGroup == 0) {
      pGroup = new geo::CElementGroup(m_mesh);
      pHorizon->InterfaceElementGroup(*pGroup);
    }

    const geo::CHexahedron *pUpper, *pLower;
    // Find pUpper and pLower according to their indices
    pUpper = allElements.find(iUpper)->second;
    pLower = allElements.find(iLower)->second;

    // mesh must be invalidated after load
    InvalidateAtUnLock();

    geo::CInterfaceElement *ptr =
        new geo::CInterfaceElement(m_mesh, &pUpper->Face(HEXA_LOWERFACE), &pLower->Face(HEXA_UPPERFACE), vcHexahedron);
    pGroup->AddMeshElement(*ptr);
    progress.Step();
  }

  int nrOfHorizons;
  stream >> nrOfHorizons;
  int iIdx;
  for (int iHorCount = 0; iHorCount < nrOfHorizons; iHorCount++) {
    stream >> iIdx;
    CHexaHorizonEntry &horizon_entry = (CHexaHorizonEntry &)*Model().GraphEntry(MD_HEXA_HORIZON);
    CHexaHorizon *pHorizon = horizon_entry.FindIndex(iIdx);
    int iFaceSize;
    stream >> iFaceSize;
    int iIndex;
    const geo::IBody *pBody;
    const geo::CHexahedron *pHex;
    for (int iFaceCount = 0; iFaceCount < iFaceSize; iFaceCount++) {
      // Find pUpper and pLower according to their indices
      stream >> iIndex;
      pBody = allElements.find(iIndex)->second;
      pHex = dynamic_cast<const geo::CHexahedron *>(pBody);
      assert(pHex);
      if (!pHorizon->UpperFormation()) {
        // Most upper horizon. Add the upper hexahedronface to the horizon.
        assert(pHorizon->LowerFormation()->VolumeSize() == 1);
        pHorizon->LowerFormation()->Volume(0).Volume().SideSurface(0).AddFace(pHex->Face(HEXA_UPPERFACE));
      } else {
        // Most lower horizon. Add the lower hexahedronface to the horizon.
        assert(pHorizon->UpperFormation()->VolumeSize() == 1);
        pHorizon->UpperFormation()->Volume(0).Volume().SideSurface(1).AddFace(pHex->Face(HEXA_LOWERFACE));
      }
    }
  }
}

void CHexaMesh::LoadSideSurface(geo::CBodyGroup::CSideSurface &side_surface, bool bTop, TSTREAM &stream,
                                TPROGRESS &progress) {
  assert(side_surface.FaceSize() == 0);
  int nSize;
  stream >> nSize;
  for (int i = 0; i < nSize; i++) {
    int nIndex;
    stream >> nIndex;
    const geo::IBody &body = dynamic_cast<const geo::IBody &>(m_mesh.Element(nIndex));
    if (bTop)
      side_surface.AddFace(body.Face(HEXA_UPPERFACE));
    else
      side_surface.AddFace(body.Face(HEXA_LOWERFACE));
    progress.Step();
  }
}

void CHexaMesh::LoadSlipHorizon(CHexaHorizon &horizon, TSTREAM &stream, TPROGRESS &progress) {
  assert(horizon.InterfaceElementGroup() == 0);
  assert(horizon.Slip());

  // Create a new interface element group
  geo::CElementGroup *pGroup = new geo::CElementGroup(m_mesh);
  horizon.InterfaceElementGroup(*pGroup);

  // Loading elements of the group
  int nElementSize;
  stream >> nElementSize;
  for (int i = 0; i < nElementSize; i++) {
    int nIndex;
    stream >> nIndex;
    const geo::CInterfaceElement &element = dynamic_cast<const geo::CInterfaceElement &>(m_mesh.Element(nIndex));
    pGroup->AddMeshElement(const_cast<geo::CInterfaceElement &>(element));
    // Add the interface element to the map
    VERIFY(m_mpInterfaces.insert(std::make_pair(&element, &horizon)).second);
    progress.Step();
  }
}

void CHexaMesh::LoadBodyGroup(geo::CBodyGroup &body_group, TSTREAM &stream, CStreamVersion &version,
                              TPROGRESS &progress) {
  assert(body_group.BodySize() == 0);

  // Save body element indices first
  int nElementSize;
  stream >> nElementSize;
  for (int i = 0; i < nElementSize; i++) {
    int nIndex;
    stream >> nIndex;
    const geo::IBody &body = dynamic_cast<const geo::IBody &>(m_mesh.Element(nIndex));
    body_group.AddBody(body);
    progress.Step();
  }

  if (version < CStreamVersion(3, 0, 25)) {
    // Loading side surfaces
    new geo::CBodyGroup::CSideSurface(body_group); // Surface 0
    new geo::CBodyGroup::CSideSurface(body_group); // Surface 1
    LoadSideSurface(body_group.SideSurface(0), true, stream, progress);
    LoadSideSurface(body_group.SideSurface(1), false, stream, progress);
  } else {
    int nSideSurfaces;
    stream >> nSideSurfaces;
    for (int i = 0; i < nSideSurfaces; i++) {
      new geo::CBodyGroup::CSideSurface(body_group);
      assert(body_group.SideSurface(i).FaceSize() == 0);
      int nFaceSize;
      stream >> nFaceSize;
      for (int j = 0; j < nFaceSize; j++) {
        int nBodyIndex;
        stream >> nBodyIndex;
        int nLocalFaceIndex;
        stream >> nLocalFaceIndex;

        const geo::IBody &body = dynamic_cast<const geo::IBody &>(m_mesh.Element(nBodyIndex));
        body_group.SideSurface(i).AddFace(body.Face(nLocalFaceIndex));
        progress.Step();
      }
    }
  }
}

void CHexaMesh::SaveSlipHorizon(const CHexaHorizon &horizon, TSTREAM &stream, TPROGRESS &progress) {
  assert(horizon.InterfaceElementGroup());
  int nElementSize = horizon.InterfaceElementGroup()->ElementSize();
  stream << nElementSize;
  for (int i = 0; i < horizon.InterfaceElementGroup()->ElementSize(); i++) {
    assert(&m_mesh == horizon.InterfaceElementGroup()->Element(i).IndexingElementSet());
    stream << horizon.InterfaceElementGroup()->Element(i).Index();
    progress.Step();
  }
}

void CHexaMesh::SaveBodyGroup(const geo::CBodyGroup &body_group, TSTREAM &stream, TPROGRESS &progress) {
  assert(body_group.SideSurfaceSize() == 6);

  // Save body element indices first
  stream << body_group.BodySize();
  int i;
  for (i = 0; i < body_group.BodySize(); i++) {
    assert(&m_mesh == body_group.Body(i).IndexingElementSet());
    stream << body_group.Body(i).Index();
    progress.Step();
  }

  stream << body_group.SideSurfaceSize();
  for (int j = 0; j < body_group.SideSurfaceSize(); j++) {
    // Save side surfaces
    stream << body_group.SideSurface(j).FaceSize();
    for (i = 0; i < body_group.SideSurface(j).FaceSize(); i++) {
      const geo::CBodyQuadrilateral &body_face =
          dynamic_cast<const geo::CBodyQuadrilateral &>(body_group.SideSurface(j).Face(i));
      assert(body_face.Parent()->IndexingElementSet() == &m_mesh);
      stream << body_face.Parent()->Index();
      stream << body_face.Index();
      progress.Step();
    }
  }
}

void CHexaMesh::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  stream << m_pMesher->MesherType();

  CMeshBase::SaveStream(stream, progress);
  std::string pr("Saving Mesh");
  progress.StatusMessage(pr.c_str());

  // We save only full meshes now in the 3.0.8 database version ...
  // We save our points first ...
  int nMinSize = m_mesh.PointSize();

  if (m_pMesher->MesherType() == IHexaMesher::OLD_MESHER) {
    int nNodes = m_pMesher->SavedItems();
    if (nNodes < nMinSize)
      nMinSize = nNodes;
  }

  stream << nMinSize;

  for (int nPoint = 0; nPoint < nMinSize; nPoint++) {
    stream << m_mesh.Point(nPoint).X();
    stream << m_mesh.Point(nPoint).Y();
    stream << m_mesh.Point(nPoint).Z();
    progress.Step();
  }

  // We save indices of the points, internal point and the slip points
  m_pMesher->SaveStream(stream, progress);

  // We save the elements in our grid ...
  // but not the boundary interfaces
  CHexaModel &model = static_cast<CHexaModel &>(Model());
  CHexaBoundary &boundary = static_cast<CHexaBoundary &>(model.Boundary());
  int nBoundaryFaces = (boundary.InterfaceElements() ? boundary.InterfaceElements()->ElementSize() : 0);
  stream << m_mesh.ElementSize() - nBoundaryFaces;

  for (int nElement = 0; nElement < m_mesh.ElementSize(); nElement++) {
    // Save element type (Interface element or hexahedron)
    assert(&m_mesh == m_mesh.Element(nElement).IndexingElementSet());
    const geo::CInterfaceElement *pInterface = dynamic_cast<const geo::CInterfaceElement *>(&m_mesh.Element(nElement));

    // boundary interface elements (and supports) are generated when needed,
    // we don't save them
    if (!pInterface || pInterface->FrontFace() != pInterface->BackFace()) {
      int bInterfaceElement = (pInterface != 0);
      stream << bInterfaceElement;

      // If we have an interface
      if (pInterface) {
        assert(pInterface->FrontFace());
        assert(pInterface->BackFace());
        // Save body index // First front and then back
        stream << pInterface->FrontFace()->Parent()->Index();
        stream << pInterface->BackFace()->Parent()->Index();

        // save front and back face indices (version 4.1.2)
        stream << pInterface->FrontFace()->Index();
        stream << pInterface->BackFace()->Index();

        // new for version 3.0.16: save bodyface start node indices
        stream << pInterface->BodyFaceNode(0);
        stream << pInterface->BodyFaceNode(4);

        assert(pInterface->NrOfNodes() == 8);
      }

      assert((m_mesh.Element(nElement).NrOfPoints() == 8) || bInterfaceElement);

      // Save element connectivity (
      for (int n = 0; n < 8; n++) {
        stream << m_mesh.Element(nElement).PointIndex(n);
      }
    }

    progress.Step();
  }

  // Saving the CBodyGroup for each formation ... if we have a mesh ...
  if (IsMesh()) {
    const CHexaFormation *pFormation = &GetTopFormation();
    while (pFormation) {
      // Save the body group
      assert(pFormation->VolumeSize() == 1);
      SaveBodyGroup(pFormation->Volume(0).Volume(), stream, progress);

      // Collect slipping horizons
      if (!pFormation->UpperFormation()) {
        if (pFormation->UpperHorizon().Slip())
          SaveSlipHorizon(pFormation->UpperHorizon(), stream, progress);
      }

      if (pFormation->LowerHorizon().Slip())
        SaveSlipHorizon(pFormation->LowerHorizon(), stream, progress);

      pFormation = pFormation->LowerFormation();
    }
  }

  SaveValueSets(stream, progress);
}

long CHexaMesh::SavedItems() const {
  long lRet = CMeshBase::SavedItems();

  lRet += m_mesh.PointSize();
  lRet += m_mesh.ElementSize();
  lRet += m_pMesher->SavedItems();

  if (IsMesh()) {
    const CHexaFormation *pFormation = &GetTopFormation();
    while (pFormation) {
      assert(pFormation->VolumeSize() == 1);
      lRet += pFormation->Volume(0).Volume().BodySize();
      for (int i = 0; i < pFormation->Volume(0).Volume().SideSurfaceSize(); i++) {
        lRet += pFormation->Volume(0).Volume().SideSurface(i).FaceSize();
      }
      if (!pFormation->UpperFormation()) {
        if (pFormation->UpperHorizon().Slip())
          lRet += pFormation->UpperHorizon().InterfaceElementGroup()->ElementSize();
      }

      if (pFormation->LowerHorizon().Slip())
        lRet += pFormation->LowerHorizon().InterfaceElementGroup()->ElementSize();
      pFormation = pFormation->LowerFormation();
    }
  }

  return lRet;
}

const geo::IMesh &CHexaMesh::Mesh() const {
  // return m_volume;
  return m_mesh;
}

geo::IMesh &CHexaMesh::Mesh() {
  // return m_volume;
  return m_mesh;
}

const CHorizonBase *CHexaMesh::SlipHorizon(const geo::IElement &iface) const {
  const geo::CInterfaceElement *pInterface = dynamic_cast<const geo::CInterfaceElement *>(&iface);
  assert(pInterface);
  TInterfaceMap::const_iterator it = m_mpInterfaces.find(pInterface);
  if (it == m_mpInterfaces.end())
    return 0;

  return it->second;
}
