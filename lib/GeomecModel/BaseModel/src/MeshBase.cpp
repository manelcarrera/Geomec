// MeshBase.cpp: implementation of the CMeshBase class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshBase.h"
#include "BaseEntryTypes.h"
#include "BoundaryBase.h"
#include "BranchState.h"
#include "DepletionStage.h"
#include "DisplacementSupportNode.h"
#include "FormationBase.h"
#include "IPlatform.h"
#include "ISupport.h"
#include "InterfaceBoundary.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "StreamVersion.h"

/*!
\class CMeshBase
\brief The base class of the mesh type in Geomec.

The CMeshBase class represents the finite element mesh in the Geomec model. The CMeshBase
class supplies a generic interface for the Geomec application to handle the mesh. The
CMeshBase object reprensents a geo::IMesh from the Geometry library.
The CMeshBase::CreateMesh() creates a mesh when the CMeshBase::CanCreateMesh() returns true.
The mesh is destroyed when the function CMeshBase::InvalidateMesh() is called. The function
CMeshBase::IsMesh() returns true when a mesh is available.
The mesh owns a result register (CResultRegister) to store the results associated the with the
mesh. The mesh could also look up the formation when a mesh element is given with the function
CMeshBase::Formation().
\sa CHexaMesh
\sa CTetraMesh
\sa C2DMesh
*/

CMeshBase::CMeshBase(const QString &sName, CDepletionStageEntry &dep_entry)
    : IElementSet(sName, dep_entry.Model(), CQuantity::SI_UNIT, CQuantity::SI_UNIT), m_bLocked(false),
      m_bInvalidateAtUnlock(false), m_bGenerateSupports(true) { // Connect to boundary
  CModelBase *pModel = dynamic_cast<CModelBase *>(&dep_entry.Model());

  LinkTo(pModel->Boundary());
  assert(IsLinkedTo(pModel->Boundary()));

  // Create result register
  m_pResultRegister = new CResultRegister(*this, dep_entry);
}

CMeshBase::~CMeshBase() {}

void CMeshBase::BuildFormationElementSetMap() const {
  assert(IsMesh());
  assert(m_vcElementToFormationSet.size() == 0);

  //	int nTetSize = 0;

  m_vcElementToFormationSet.resize(Mesh().ElementSize(), 0);

  const TFormationBaseEntry *pEntry = dynamic_cast<const TFormationBaseEntry *>(Model().GraphEntry(MD_BASE_FORMATION));
  assert(pEntry);
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();
  TFormationBaseEntry::TNodeSet::iterator it;
  /*
    for(it = stFormation.begin(); it != stFormation.end(); it++)
    { // first calculate the number of tetrahedrons (so without the boundaryinterface elements)
      CFormationBase& formation = **it;
      for(int i = 0; i < formation.ElementSetSize(); i++)
      {
        const IFormationElementSet &elementset = formation.ElementSet(i);
        nTetSize += elementset.ElementSet().ElementSize();
      }
    }
    m_vcElementToFormationSet.resize(nTetSize, 0);
  */
  for (it = stFormation.begin(); it != stFormation.end(); it++) {
    CFormationBase &formation = **it;
    for (int i = 0; i < formation.ElementSetSize(); i++) {
      const IFormationElementSet &elementset = formation.ElementSet(i);
      for (int j = 0; j < elementset.ElementSet().ElementSize(); j++) {
        const geo::IElement &element = elementset.ElementSet().Element(j);
        assert(element.IndexingElementSet() == &Mesh());
        assert(element.Index() >= 0);
        // assert(element.Index() < nTetSize);
        assert(element.Index() < Mesh().ElementSize());
        m_vcElementToFormationSet[element.Index()] = &elementset;
      }
    }
  }
}

/*!
  Returns the result register owned by the mesh
*/
const CResultRegister &CMeshBase::ResultRegister() const {
  assert(m_pResultRegister);
  return *m_pResultRegister;
}

/*!
  Returns the result register owned by the mesh
*/
CResultRegister &CMeshBase::ResultRegister() {
  assert(m_pResultRegister);
  return *m_pResultRegister;
}

bool CMeshBase::CanInvalidateMesh() const { return true; }

// Interface of elementset always returning the current deformed mesh ...
const geo::IElementSet &CMeshBase::ElementSet() const { return Mesh(); }

geo::IElementSet &CMeshBase::ElementSet() { return Mesh(); }

bool CMeshBase::PointInConvexHull(const geo::IPoint &pt) const { return Mesh().Contains(pt, true); }

int CMeshBase::DisplayListSize() const { return 1; }

const geo::IObject &CMeshBase::DisplayList(int /*nIndex*/) const { return Mesh(); }

void CMeshBase::InvalidateAtUnLock() {
  assert(Locked());
  m_bInvalidateAtUnlock = true;
}

void CMeshBase::Lock() const { m_bLocked = true; }

void CMeshBase::Unlock() {
  if (m_bLocked) {
    m_bLocked = false;
    if (m_bInvalidateAtUnlock) {
      m_bInvalidateAtUnlock = false;
      if (!ImportedMesh())
        InvalidateMesh();
    }
  }
}

bool CMeshBase::Locked() const { return m_bLocked; }

/*!
Invalidating the mesh means that the mesh between surfaces of the horizons, faults and boundary of the sub- and super
model is deleted. This happens whenever there is a change in the model’s geometry and / or in the mesh parameters. As a
result all the bodies generated by the mesh procedure are deleted automatically from the model. For example the
following actions result in invalidation of the mesh: -	Inserting  a horizon -	Deleting a horizon -	Refining or
coarsening surfaces of a horizon, a fault or the boundary -	Changing the state of the boundary Invalidating the mesh
automatically results in invalidation of any results.
*/
void CMeshBase::InvalidateMesh() {
  // Clear the current mesh and set current timestep to zero
  CModelBase *pModel = dynamic_cast<CModelBase *>(&Model());
  assert(pModel);

  // block invalidation in branch state
  if (pModel->BranchState().IsBranch()) {
    IPlatform *platform = IPlatform::instance();

    platform->trace("WARNING: CMeshBase::InvalidateMesh called in branched "
                    "state, ignoring call\n");
    return;
  }

  // Clear result register
  pModel->ResultRegister().ClearAll();
  m_vcElementToFormationSet.clear();

  CInterfaceBoundary *pBoundary = dynamic_cast<CInterfaceBoundary *>(&pModel->Boundary());
  if (pBoundary)
    pBoundary->InterfaceDefinition().ClearInterfaceMaterials();
}

const CFormationBase *CMeshBase::Formation(const geo::IElement &element) const {
  const IFormationElementSet *pSet = FormationElementSet(element);
  if (!pSet)
    return 0;

  return &pSet->Formation();
}

const IFormationElementSet *CMeshBase::FormationElementSet(const geo::IElement &element) const {
  assert(IsMesh());
  //	assert(element.IndexingElementSet() == &Mesh());
  if (m_vcElementToFormationSet.size() == 0)
    BuildFormationElementSetMap();

  assert(element.Index() < m_vcElementToFormationSet.size());

  return m_vcElementToFormationSet[element.Index()];
}

void CMeshBase::OnNeighbourDeleted(const CGraphNode &item) {
  IElementSet::OnNeighbourDeleted(item);

  // Forget about the result register if it has been deleted
  if (&item == m_pResultRegister)
    m_pResultRegister = 0;
}

long CMeshBase::SavedItemsValueSets() const { return IElementSet::SavedItems(); }

void CMeshBase::SaveValueSets(TSTREAM &stream, TPROGRESS &progress) {
  // done by baseclass
  IElementSet::SaveStream(stream, progress);
}

void CMeshBase::LoadValueSets(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // done by baseclass
  IElementSet::LoadStream(stream, version, progress);
}

void CMeshBase::SaveStream(TSTREAM &stream, TPROGRESS & /*progress*/) {
  stream << int(m_bGenerateSupports ? 1 : 0);

  if (!m_bGenerateSupports) {
    // save them...
    int nSize = Mesh().SupportSize();
    stream << nSize;
    for (int i = 0; i < nSize; ++i) {
      const geo::ISupport &support = Mesh().Support(i);
      assert(dynamic_cast<const geo::CTranslationSupport *>(&support)); // for now, only translation supports supported
      int nNodeIdx = support.NodeIndex();
      stream << nNodeIdx;

      int nDirSize = support.DirectionSize();
      stream << nDirSize;
      for (int j = 0; j < nDirSize; ++j) {
        const geo::IVector &vecDir = support.Direction(j);
        stream << vecDir.X();
        stream << vecDir.Y();
        stream << vecDir.Z();
      }
    }
  }

  // from version 3.7.17 base class functionality is called elsewhere
}

void CMeshBase::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  if (version < CStreamVersion(3, 7, 17))
    IElementSet::LoadStream(stream, version, progress);

  if (version >= CStreamVersion(4, 1, 11)) {
    int nGenerateSupports;
    stream >> nGenerateSupports;
    m_bGenerateSupports = (nGenerateSupports != 0);

    if (!m_bGenerateSupports) {
      int nSize;
      stream >> nSize;
      for (int i = 0; i < nSize; ++i) {
        int nNodeIdx;
        stream >> nNodeIdx;

        int nDirSize;
        stream >> nDirSize;

        for (int j = 0; j < nDirSize; ++j) {
          double x, y, z;
          stream >> x;
          stream >> y;
          stream >> z;
          geo::CVector vecDir(x, y, z);
          Mesh().CreateTranslationSupport(nNodeIdx, vecDir);
        }
      }
    }
  }
}

const CFFMaterial *CMeshBase::getCFFMaterial(const geo::IElement &element,
                                             const CDepletionStage &depletionStage) const {
  const CFormationBase *formation = Formation(element);

  if (formation && formation->Material(depletionStage).LibraryMaterial())
    return &(formation->Material(depletionStage).Material(element));

  return 0;
}

int CMeshBase::ReferencePointIndex() const {
  assert(IsMesh()); // can only do this if a mesh exists
  const CModelBase &model = static_cast<const CModelBase &>(Model());
  const CInterfaceBoundary *pBoundary = dynamic_cast<const CInterfaceBoundary *>(&model.Boundary());
  if (pBoundary) {
    const CDisplacementSupportNode &dnode = pBoundary->DisplacementSupportNode();

    for (int i = 0; i < Mesh().PointSize(); i++) {
      if (Mesh().Point(i) == dnode.ReferencePoint())
        return i;
    }
  }

  assert(false);
  return -1;
}

bool CMeshBase::ImportedMesh() const { return false; }

bool CMeshBase::getGenerateSupports() const { return m_bGenerateSupports; }

void CMeshBase::setGenerateSupports(bool bGenerateSupports) { m_bGenerateSupports = bGenerateSupports; }
