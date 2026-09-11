// TetraSubHorizon.cpp: implementation of the CTetraSubHorizon class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraSubHorizon.h"
#include "BranchState.h"
#include "FemAppModel.h"
#include "GeomecStringTable.h"
#include "GlobalMessage.h"
#include "ModelBase.h"
#include "StreamVersion.h"
#include "TetraEntryTypes.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSurface.h"
#include "ValueTypes.h"
#include "resourceIDI.h"
#include "resourceIDP.h"
#include "resourceIDS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CTetraSubHorizon::AttachToEntry() {
  if (Slip()) {
    assert(Model().GraphEntry(MD_TETRA_SUB_FAULT));
    create(Model().GraphEntry(MD_TETRA_SUB_FAULT));
  } else {
    assert(Model().GraphEntry(MD_TETRA_SUB_HORIZON));
    create(Model().GraphEntry(MD_TETRA_SUB_HORIZON));
  }

  // Link to submodel entry
  assert(Model().GraphEntry(MD_TETRA_SUB_ALL));
  Model().GraphEntry(MD_TETRA_SUB_ALL)->LinkTo(*this);
}

// ##ModelId=3E3E5F010002
CTetraSubHorizon::CTetraSubHorizon(CFemAppModel &model) : CTetraHorizonBase(model) {}

// ##ModelId=3E3E5F010004
CTetraSubHorizon::CTetraSubHorizon(const QString &sName, bool bSlip, CFemAppModel &model, bool bAttachToEntry)
    : CTetraHorizonBase(sName, model, bAttachToEntry) {
  init();

  Slip(bSlip);

  if (bAttachToEntry)
    AttachToEntry();
}

// ##ModelId=3E3E5F010010
CTetraSubHorizon::CTetraSubHorizon(CSurfaceBase &surface, CFemAppModel &model, bool bSlip, bool bAttachToEntry)
    : CTetraHorizonBase(surface, model, bSlip) {
  init();

  if (bAttachToEntry)
    AttachToEntry();
}

CTetraSubHorizon::CTetraSubHorizon(const CTetraSubHorizon &rhs) : CTetraHorizonBase((const C3DHorizon &)(rhs)) {}

void CTetraSubHorizon::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  CTetraHorizonBase::SaveStream(stream, progress);
}

// ##ModelId=3E3E5F01001C
void CTetraSubHorizon::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  CTetraHorizonBase::LoadStream(stream, version, progress);

  if (Slip()) {
    assert(Model().GraphEntry(MD_TETRA_SUB_FAULT));
    create(Model().GraphEntry(MD_TETRA_SUB_FAULT));
  } else {
    assert(Model().GraphEntry(MD_TETRA_SUB_HORIZON));
    create(Model().GraphEntry(MD_TETRA_SUB_HORIZON));
  }

  // Link to submodel entry
  assert(Model().GraphEntry(MD_TETRA_SUB_ALL));
  Model().GraphEntry(MD_TETRA_SUB_ALL)->LinkTo(*this);

  if (version > CStreamVersion(3, 0, 70) && version < CStreamVersion(3, 0, 76)) {
    int nSlipType;
    stream >> nSlipType;
    if (nSlipType == 0)
      SlipType(STICK);
    else if (nSlipType == 1)
      SlipType(SLIP);
    else {
      assert(nSlipType == 2);
      SlipType(USER);
    }
  }

  if (version > CStreamVersion(3, 0, 71) && version < CStreamVersion(3, 0, 76)) {
    double dCohesion, dFrictionAngle;
    stream >> dCohesion;
    stream >> dFrictionAngle;
    //		StickQuantities(dCohesion, dFrictionAngle, CDoubleQuantity::SI_UNIT);
  }
}

// ##ModelId=3E3E5F010013
CTetraSubHorizon::~CTetraSubHorizon() {}

// ##ModelId=3E3E5F010015
unsigned int CTetraSubHorizon::IconId() const {
  if (Slip())
    return IDI_FAULT;

  return IDI_HORIZON;
}

// ##ModelId=3E3E5F010019
bool CTetraSubHorizon::CanConnectItem(const CGraphNode &item) const {
  const CSurfaceBase *pSurface = dynamic_cast<const CSurfaceBase *>(&item);
  if (pSurface) {
    // don't allow modifications in branched state
    if ((static_cast<const CModelBase &>(Model())).BranchState().IsBranch())
      return false;

    // Reject surface linked to ourselves
    if (IsLinkedTo(*pSurface))
      return false;

    // Reject surface linked to sub boundary
    CTetraModel &model = (CTetraModel &)(Model());
    if (model.SubBoundary().IsLinkedTo(*pSurface))
      return false;

    // Reject surface linked to horizons in general ...
    for (size_t i = 0; i < pSurface->referenceSize(); i++) {
      if (dynamic_cast<const CTetraHorizonBase *>(&pSurface->referenceAt(i)))
        return false;
    }

    return true;
  }

  return CTetraHorizonBase::CanConnectItem(item);
}

// ##ModelId=3E3E5F010023
unsigned int CTetraSubHorizon::TypeId() const {
  if (Slip())
    return IDT_TREE_FAULTS;
  return CTetraHorizonBase::TypeId();
}

QString CTetraSubHorizon::TypeName() const {
  if (Slip()) {
    return getStringTableEntry(IDS_TREE_FAULTS_FRACTURES);
  }

  return CTetraHorizonBase::TypeName();
}

bool CTetraSubHorizon::CanSlip() const {
  const CTetraModel &model = dynamic_cast<const CTetraModel &>(Model());
  return (model.SubBoundary().TopHorizon().Horizon() != this) &&
         (model.SubBoundary().BottomHorizon().Horizon() != this);
}

bool CTetraSubHorizon::CanToggleSlip() const {
  const CTetraModel &model = dynamic_cast<const CTetraModel &>(Model());
  const CTetraMesh &mesh = dynamic_cast<const CTetraMesh &>(model.Mesh());
  return !DoubleSidedFault() && (mesh.CanInvalidateMesh() || (mesh.ImportedMesh() && !Slip())) && CanSlip();
}

void CTetraSubHorizon::ToggleSlip() {
  assert(CanSlip());
  if (SurfaceSize() > 1) {
    // don't allow toggle when more surfaces are connected
    // but give warning instead of only disabling the menu item
    _m()->msg(IDP_TOGGLE_TETRAHORIZON_MULTIPLE_SURFACES);
    return;
  }

  CTetraModel &model = dynamic_cast<CTetraModel &>(Model());
  CTetraMesh &mesh = dynamic_cast<CTetraMesh &>(model.Mesh());

  // Unlink from the fault cq. horizon entry
  if (Slip())
    UnLink(*Model().GraphEntry(MD_TETRA_SUB_FAULT));
  else
    UnLink(*Model().GraphEntry(MD_TETRA_SUB_HORIZON));

  // Set slip
  Slip(!Slip());

  assert(mesh.CanInvalidateMesh() || mesh.ImportedMesh());

  if (!mesh.ImportedMesh()) {
    if (SurfaceSize() > 0)
      mesh.InvalidateMesh();
  } else {
    // this may fail (isolated edges of a fault)
    if (!mesh.OnHorizonSlipChanged(*this))
      Slip(!Slip()); // the horizon will be linked back to its original graphentry below
  }

  if (Slip())
    Model().GraphEntry(MD_TETRA_SUB_FAULT)->LinkTo(*this);
  else
    Model().GraphEntry(MD_TETRA_SUB_HORIZON)->LinkTo(*this);

  Modified();
}

bool CTetraSubHorizon::FaultsLess(const CGraphNode &node) const {
  const CStorageNode *storageNode = dynamic_cast<const CStorageNode *>(&node);

  if (storageNode) {
    if (Index() > -1 && storageNode->Index() > -1) // sort by index (when the model has been loaded from file)
      return Index() < storageNode->Index();
    else if (Index() > -1) // otherwise put new faults first
      return false;
    else if (storageNode->Index() > -1)
      return true;
  }

  // sort new faults by name
  if (Name() != node.Name())
    return Name() < node.Name();

  // or if identical names, by pointer (non-deterministic in different sessions, but should not matter once model has
  // been saved)
  return CColorNode::Less(node);
}

bool CTetraSubHorizon::Less(const CGraphNode &node) const {
  const CTetraSubHorizon *other = dynamic_cast<const CTetraSubHorizon *>(&node);
  bool other_slip = other && other->Slip();
  bool slip = Slip();

  if (!slip && !other_slip) // sort horizons
    return CTetraHorizonBase::Less(node);
  else if (slip && other_slip) // sort faults
    return FaultsLess(node);
  else // faults before horizons
    return slip;
}

bool CTetraSubHorizon::IsTopHorizon() const {
  const CTetraModel &model = dynamic_cast<const CTetraModel &>(Model());
  return model.SubBoundary().TopHorizon().Horizon() == this;
}

void CTetraSubHorizon::OnSlipToggled() {
  CModelBase &model = static_cast<CModelBase &>(Model());
  if (model.Mesh().IsMesh() && !model.Mesh().ImportedMesh() && SurfaceSize() > 0)
    model.Mesh().InvalidateMesh();
}

bool CTetraSubHorizon::DoubleSidedFault() const {
  return (IntermediateFault() == this && PlusFault() != 0 && MinusFault() != 0) || PlusFault() == this ||
         MinusFault() == this;
}

bool CTetraSubHorizon::IgnoreFault() const {
  if (IntermediateFault())
    return PlusFault() == this || MinusFault() == this;

  return PlusFault() == this;
}

const CTetraSubHorizon *CTetraSubHorizon::IntermediateFault() const { return DSFault(DSFNameBase(Name())); }

const CTetraSubHorizon *CTetraSubHorizon::MinusFault() const { return DSFault(DSFNameBase(Name()) + "_minus"); }

const CTetraSubHorizon *CTetraSubHorizon::PlusFault() const { return DSFault(DSFNameBase(Name()) + "_plus"); }

QString CTetraSubHorizon::DSFNameBase(const QString &name) const {
  if (name.endsWith("fault"))
    return name;

  if (name.endsWith("fault_minus"))
    return name.left(name.length() - 6);

  if (name.endsWith("fault_plus"))
    return name.left(name.length() - 5);

  return QString();
}

const CTetraSubHorizon *CTetraSubHorizon::DSFault(const QString &name) const {
  if (name.isEmpty())
    return 0;

  if (name == Name())
    return this;

  const CTetraSubHorizonEntry *pEntry =
      static_cast<const CTetraSubHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUB_FAULT));
  if (pEntry) {
    CTetraSubHorizonEntry::TNodeSet stNodes = pEntry->EntryNodes();
    for (CTetraSubHorizonEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it) {
      if ((*it)->Name() == name)
        return *it;
    }
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CSubHorizonEntry
//////////////////////////////////////////////////////////////////////
CTetraSubHorizonEntry::CTetraSubHorizonEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId,
                                             CFemAppModel &model)
    : CStorageNodeEntry<CTetraSubHorizon>(nEntryId, uIconId, uNameId, model) {
  assert((nEntryId == MD_TETRA_SUB_FAULT) || (nEntryId == MD_TETRA_SUB_HORIZON));
}

bool CTetraSubHorizonEntry::CanConnectItem(const CGraphNode &item) const {
  const CTetraSurface *pSurface = dynamic_cast<const CTetraSurface *>(&item);
  if (pSurface)
    return !pSurface->Used();

  return false;
}

bool CTetraSubHorizonEntry::ConnectItem(const CGraphNode &item) {
  const CTetraSurface *pSurface = dynamic_cast<const CTetraSurface *>(&item);
  if (pSurface) {
    new CTetraSubHorizon((CTetraSurface &)*pSurface, Model(), EntryId() == MD_TETRA_SUB_FAULT);
    return true;
  }

  return false;
}
