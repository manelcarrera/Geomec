// TetraSubBoundary.cpp: implementation of the CTetraSubBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraSubBoundary.h"
#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "FemAppEntryTypes.h"
#include "GeomecStringTable.h"
#include "ModelBase.h"
#include "StreamVersion.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "TetraEntryTypes.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubHorizon.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraSubBoundary::CHorizonPlaceHolder::CHorizonPlaceHolder(unsigned int uName, CTetraSubBoundary &boundary)
    : COpenGLNode(uName, boundary.Model()), m_pHorizon(0), m_boundary(boundary), m_model(boundary.Model()) {
  LinkTo(boundary);
}

bool CTetraSubBoundary::CHorizonPlaceHolder::Less(const CGraphNode &node) const {
  if (this == &node)
    return false;

  if (&m_boundary.TopHorizon() == &node) // We're the bottom
    return false;

  if (&m_boundary.BottomHorizon() == &node) // We're the top
    return true;

  const CHorizonPlaceHolder *pHolder = dynamic_cast<const CHorizonPlaceHolder *>(&node);
  if (pHolder) {
    if (&m_boundary.TopHorizon() == this)
      return false;
    if (&m_boundary.BottomHorizon() == this)
      return true;
  }

  return CGraphNode::Less(node);
}

const CTetraSubHorizon *CTetraSubBoundary::CHorizonPlaceHolder::Horizon() const { return m_pHorizon; }

CTetraSubHorizon *CTetraSubBoundary::CHorizonPlaceHolder::Horizon() { return m_pHorizon; }

bool CTetraSubBoundary::CHorizonPlaceHolder::Empty() const { return true; }

long CTetraSubBoundary::CHorizonPlaceHolder::SavedItems() const { return 0l; }

CTetraSubBoundary::CHorizonPlaceHolder::TColor CTetraSubBoundary::CHorizonPlaceHolder::Color() const {
  return Horizon() ? Horizon()->Color() : 0;
}

int CTetraSubBoundary::CHorizonPlaceHolder::DisplayListSize() const {
  return Horizon() ? Horizon()->DisplayListSize() : 0;
}

const geo::IObject &CTetraSubBoundary::CHorizonPlaceHolder::DisplayList(int nIndex) const {
  assert(Horizon());
  return Horizon()->DisplayList(nIndex);
}

bool CTetraSubBoundary::CHorizonPlaceHolder::CanDisconnectItem(const CGraphNode &item) const {
  const CModelBase &model = dynamic_cast<const CModelBase &>(m_model);
  const CTetraMesh &mesh = dynamic_cast<const CTetraMesh &>(model.Mesh());
  return (&item != m_pHorizon) || mesh.CanInvalidateMesh();
}

void CTetraSubBoundary::CHorizonPlaceHolder::OnNewNeighbour(const CGraphNode &node) {
  const CTetraSubHorizon *pHorizon = dynamic_cast<const CTetraSubHorizon *>(&node);
  if (pHorizon) {
    if (m_pHorizon)
      UnLink(*m_pHorizon);
    assert(m_pHorizon == 0);
    m_pHorizon = const_cast<CTetraSubHorizon *>(pHorizon);
    ///		m_boundary.UpdateSurfaces();
    CModelBase *pModel = (CModelBase *)(&m_model);
    pModel->InvalidateMesh();
    Modified();
  }

  CGraphNode::OnNewNeighbour(node);
}

unsigned int CTetraSubBoundary::CHorizonPlaceHolder::IconId() const {
  if (m_pHorizon) {
    if (&m_boundary.TopHorizon() == this)
      return IDI_TOP_HORIZON;

    return IDI_BOTTOM_HORIZON;
  }

  if (&m_boundary.TopHorizon() == this)
    return IDI_TOP_HORIZON_DISABLED;

  return IDI_BOTTOM_HORIZON_DISABLED;
}

unsigned int CTetraSubBoundary::CHorizonPlaceHolder::TypeId() const { return 0; }

void CTetraSubBoundary::CHorizonPlaceHolder::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (&node == m_pHorizon) {
    //		m_boundary.UpdateSurfaces();
    Modified();
  }

  CGraphNode::OnNeighbourModified(node, uHint);
}

void CTetraSubBoundary::CHorizonPlaceHolder::OnNeighbourDeleted(const CGraphNode &node) {
  if (&node == m_pHorizon) {
    // Clear mesh
    CModelBase *pModel = (CModelBase *)(&m_model);
    m_pHorizon = 0;
    //		m_boundary.UpdateSurfaces();
    pModel->InvalidateMesh();
    Modified();
  }

  CGraphNode::OnNeighbourDeleted(node);
}

bool CTetraSubBoundary::CHorizonPlaceHolder::CanConnectItem(const CGraphNode &item) const {
  const CTetraSubHorizon *pHorizon = dynamic_cast<const CTetraSubHorizon *>(&item);
  if (pHorizon == 0)
    return false;

  if ((static_cast<const CModelBase &>(Model())).BranchState().IsBranch())
    return false;

  // Reject faults
  if (pHorizon->Slip())
    return false;

  if (m_boundary.TopHorizon().Horizon() == pHorizon)
    return false;

  if (m_boundary.BottomHorizon().Horizon() == pHorizon)
    return false;

  return true;
}

CTetraSubBoundary::CTetraSubBoundary(CTetraModel &model) : CColorNode("Boundary", model), m_drawmode(DRAW_INIT) {
  m_pTop = new CHorizonPlaceHolder(IDS_TOP_HORIZON, *this);
  m_pBottom = new CHorizonPlaceHolder(IDS_BOTTOM_HORIZON, *this);
  // Link to submodel entry
  assert(Model().GraphEntry(MD_TETRA_SUB_ALL));
  Model().GraphEntry(MD_TETRA_SUB_ALL)->LinkTo(*this);
}

CTetraSubBoundary::~CTetraSubBoundary() {
  delete m_pTop;
  delete m_pBottom;
}

void CTetraSubBoundary::setDrawMode(TDrawMode drawmode) {
  if (drawmode != m_drawmode) {
    m_vcDisplayList.clear();
    m_drawmode = drawmode;
    Modified(); // force redraw
  }
}

int CTetraSubBoundary::DisplayListSize() const { return GenerateDisplayList().size(); }

CTetraSubBoundary::CDrawDef *CTetraSubBoundary::OnCreateDrawDef(const geo::IObject &object) const {
  // Top horizon DD
  if (m_pTop->Horizon()) {
    for (int i = 0; i < m_pTop->Horizon()->DisplayListSize(); i++) {
      if (&object == &m_pTop->Horizon()->DisplayList(i))
        return m_pTop->Horizon()->OnCreateDrawDef(object);
    }
  }

  // Bottom horizon DD
  if (m_pBottom->Horizon()) {
    for (int i = 0; i < m_pBottom->Horizon()->DisplayListSize(); i++) {
      if (&object == &m_pBottom->Horizon()->DisplayList(i))
        return m_pBottom->Horizon()->OnCreateDrawDef(object);
    }
  }

  return new CBoundaryDrawDef(*this);
}

bool CTetraSubBoundary::CanDisconnectItem(const CGraphNode &item) const {
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  const CTetraMesh &mesh = dynamic_cast<const CTetraMesh &>(model.Mesh());
  if (mesh.CanInvalidateMesh())
    return true;

  for (int i = 0; i < SideSurfaceSize(); i++) {
    if (&SideSurface(i) == &item)
      return false;
  }

  return true;
}

const CTetraSubBoundary::TDisplayVec &CTetraSubBoundary::GenerateDisplayList() const {
  if (m_vcDisplayList.empty()) {

    // Get the mesh ...
    const CModelBase *pModel = dynamic_cast<const CModelBase *>(&Model());
    assert(pModel);
    const CTetraMesh *pMesh = dynamic_cast<const CTetraMesh *>(&pModel->Mesh());
    assert(pMesh);

    // Consider top
    if (m_pTop->Horizon() && m_drawmode == DRAW_ALL) {
      for (int i = 0; i < m_pTop->Horizon()->DisplayListSize(); i++)
        m_vcDisplayList.push_back(&m_pTop->Horizon()->DisplayList(i));
    }

    // Consider sides
    for (int i = 0; i < SideSurfaceSize(); i++) {
      if (pMesh->IsMesh()) {
        const geo::CSurfaceDesc &surf_desc = SideSurfaceDesc(i);
        for (int j = 0; j < surf_desc.TetSurfaceSize(); j++)
          m_vcDisplayList.push_back(&surf_desc.TetSurface(j));
      } else {
        for (int j = 0; j < SideSurface(i).DisplayListSize(); j++)
          m_vcDisplayList.push_back(&SideSurface(i).DisplayList(j));
      }
    }

    // Consider bottom
    if (m_pBottom->Horizon() && m_drawmode == DRAW_ALL) {
      for (int i = 0; i < m_pBottom->Horizon()->DisplayListSize(); i++)
        m_vcDisplayList.push_back(&m_pBottom->Horizon()->DisplayList(i));
    }
  }

  return m_vcDisplayList;
}

const geo::IObject &CTetraSubBoundary::DisplayList(int nIndex) const { return *GenerateDisplayList()[nIndex]; }

int CTetraSubBoundary::SideSurfaceSize() const { return m_vcSideSurface.size(); }

const CSurfaceBase &CTetraSubBoundary::SideSurface(int nIndex) const { return *m_vcSideSurface[nIndex]; }

CSurfaceBase &CTetraSubBoundary::SideSurface(int nIndex) { return *m_vcSideSurface[nIndex]; }

const geo::CSurfaceDesc &CTetraSubBoundary::SideSurfaceDesc(int nIndex) const {
  // Get the mesh ...
  const CModelBase *pModel = dynamic_cast<const CModelBase *>(&Model());
  assert(pModel);
  const CTetraMesh *pMesh = dynamic_cast<const CTetraMesh *>(&pModel->Mesh());
  assert(pMesh && pMesh->IsMesh());

  // Search for side surface
  for (int i = 0; i < pMesh->InputSurfaceSize(); i++) {
    if (!pMesh->InputSurface(i).first->Slip()) {
      if (&pMesh->InputSurface(i).first->Surface() == &SideSurface(nIndex).Surface())
        return *pMesh->InputSurface(i).first;
    }
  }

  assert(false);

  const geo::CSurfaceDesc *pBogus = 0;
  return *pBogus;
}

const CTetraSubBoundary::CHorizonPlaceHolder &CTetraSubBoundary::TopHorizon() const { return *m_pTop; }

CTetraSubBoundary::CHorizonPlaceHolder &CTetraSubBoundary::TopHorizon() { return *m_pTop; }

const CTetraSubBoundary::CHorizonPlaceHolder &CTetraSubBoundary::BottomHorizon() const { return *m_pBottom; }

CTetraSubBoundary::CHorizonPlaceHolder &CTetraSubBoundary::BottomHorizon() { return *m_pBottom; }

unsigned int CTetraSubBoundary::IconId() const { return IDI_BOUNDARY; }

unsigned int CTetraSubBoundary::TypeId() const { return IDT_BOUNDARY; }

QString CTetraSubBoundary::TypeName() const { return getStringTableEntry(IDS_BOUNDARY); }

bool CTetraSubBoundary::Empty() const { return false; }

long CTetraSubBoundary::SavedItems() const { return CColorNode::SavedItems() + 2 + SideSurfaceSize(); }
void CTetraSubBoundary::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CTetraSubHorizonEntry &horizon_entry = (CTetraSubHorizonEntry &)*Model().GraphEntry(MD_TETRA_SUB_HORIZON);
  CSurfaceEntry &surface_entry = (CSurfaceEntry &)*Model().GraphEntry(MD_BASE_SURFACE);
  if (CStreamVersion(3, 0, 30) < version) {
    UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
    UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));
    CColorNode::LoadStream(stream, version, progress);
  }

  // some older files have 'SubBoundary' set as name in the file. Just reset to 'Boundary'.
  if (Name() == "SubBoundary")
    Name("Boundary");

  // Just load'in the top horizon
  int nTopHorizon;
  stream >> nTopHorizon;
  if (nTopHorizon != -1)
    TopHorizon().LinkTo(*horizon_entry.FindIndex(nTopHorizon));

  progress.Step();

  // Just load'in the side sufaces
  int nSideSurfaceSize;
  stream >> nSideSurfaceSize;
  for (int i = 0; i < nSideSurfaceSize; i++) {
    int nIndex;
    stream >> nIndex;
    assert(surface_entry.FindIndex(nIndex));
    LinkTo(*surface_entry.FindIndex(nIndex));
    progress.Step();
  }

  // Just load'in the top horizon
  int nBottomHorizon;
  stream >> nBottomHorizon;
  if (nBottomHorizon != -1)
    BottomHorizon().LinkTo(*horizon_entry.FindIndex(nBottomHorizon));

  progress.Step();
}

void CTetraSubBoundary::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  CColorNode::SaveStream(stream, progress);

  // Save'in top horizon
  if (TopHorizon().Horizon())
    stream << TopHorizon().Horizon()->Index();
  else
    stream << -1;

  progress.Step();

  // Save'in side surfaces
  stream << SideSurfaceSize();
  for (int i = 0; i < SideSurfaceSize(); i++) {
    stream << SideSurface(i).Index();
    progress.Step();
  }

  if (BottomHorizon().Horizon())
    stream << BottomHorizon().Horizon()->Index();
  else
    stream << -1;

  progress.Step();
}

void CTetraSubBoundary::OnNewNeighbour(const CGraphNode &node) {
  const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase *>(&node);
  if (pSurface) {
    bool bAddSurface = true;

    if (TopHorizon().Horizon()) {
      if (TopHorizon().Horizon()->IsLinkedTo(*pSurface))
        bAddSurface = false;
    }

    if (BottomHorizon().Horizon()) {
      if (BottomHorizon().Horizon()->IsLinkedTo(*pSurface))
        bAddSurface = false;
    }

    if (bAddSurface) {
      m_vcSideSurface.push_back(const_cast<CSurfaceBase *>(pSurface));
      //			UpdateSurfaces();
      CModelBase *pModel = (CModelBase *)(&Model());
      pModel->InvalidateMesh();
      m_vcDisplayList.clear();
      Modified();
    }
  }

  CColorNode::OnNewNeighbour(node);
}

void CTetraSubBoundary::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  COpenGLNode::OnNeighbourModified(node, uHint);
}

void CTetraSubBoundary::OnNeighbourDeleted(const CGraphNode &node) {
  COpenGLNode::OnNeighbourDeleted(node);

  for (TSurfaceVec::iterator it = m_vcSideSurface.begin(); it != m_vcSideSurface.end(); it++) {
    if (*it == &node) {
      // Remove side surface
      COperation operation(Model());
      m_vcSideSurface.erase(it);
      CModelBase *pModel = (CModelBase *)(&Model());
      pModel->InvalidateMesh();
      Modified();
      return;
    }
  }
}

bool CTetraSubBoundary::CanConnectItem(const CGraphNode &item) const {
  const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase *>(&item);
  if (pSurface) {
    if ((static_cast<const CModelBase &>(Model())).BranchState().IsBranch())
      return false;

    if (IsLinkedTo(*pSurface)) // Already linked ...
      return false;

    // Surfaces linked to a horizon are invalid for boundary sides
    for (size_t i = 0; i < pSurface->referenceSize(); i++) {
      if (dynamic_cast<const CHorizonBase *>(&pSurface->referenceAt(i)))
        return false;
    }

    return true;
  }

  return false;
}

bool CTetraSubBoundary::Less(const CGraphNode &node) const {
  if (this == &node)
    return false;

  const CHorizonPlaceHolder *pHolder = dynamic_cast<const CHorizonPlaceHolder *>(&node);
  if (pHolder) {
    if (pHolder == m_pTop)
      return false;
    return true;
  }

  return CColorNode::Less(node);
}

void CTetraSubBoundary::UpdateHorizonSurfaces(CTetraSubBoundary::CHorizonPlaceHolder &horizon) {
  if (horizon.Horizon()) {
    for (int i = 0; i < horizon.Horizon()->SurfaceSize(); i++) {
      if (!IsLinkedTo(horizon.Horizon()->Surface(i)))
        LinkTo(horizon.Horizon()->Surface(i));
    }
  }
}

void CTetraSubBoundary::MeshInvalidated() {
  m_vcDisplayList.clear();
  Modified(); // to prevent drawing of mesh entities
}
