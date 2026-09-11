// HexaBoundary.cpp: implementation of the CHexaBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "HexaBoundary.h"
#include "BoundaryInterfaceDef.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaMesh.h"
#include "HexaModel.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "hexameshregion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHexaBoundary::CHexaBoundary(const geo::IPoint &ptMin, const geo::IPoint &ptMax, CFemAppModel &model,
                             BOUNDARY_STATE state)
    : CInterfaceBoundary(ptMin, ptMax, model, state) {}

CHexaBoundary::~CHexaBoundary() {}

CHexaBoundary::TMinMax CHexaBoundary::BestFit() const {
  TMinMax ret;

  // Calculate horizons ....
  std::vector<const CHexaHorizon *> vcConstantDepth;
  for (size_t i = 0; i < referenceSize(); i++) {
    const CHexaHorizon *pHorizon = dynamic_cast<const CHexaHorizon *>(&referenceAt(i));
    if (pHorizon) {
      if (!pHorizon->ConstantDepth()) {
        // We always have one surface in the hexa model
        assert(pHorizon->SurfaceSize() == 1);
        ret.first = ret.first.Min(pHorizon->Min());
        ret.second = ret.second.Max(pHorizon->Max());
      } else
        vcConstantDepth.push_back(pHorizon);
    }
  }

  if (ret.first.Empty() && ret.second.Empty()) {
    // Initialize x and y default ....
    ret.first = Min();
    ret.second = Max();

    if (vcConstantDepth.size() != 0) {
      // Initialize z with first horizon
      ret.first.Z(vcConstantDepth[0]->Depth().Value(CQuantity::SI_UNIT));
      ret.second.Z(vcConstantDepth[0]->Depth().Value(CQuantity::SI_UNIT));
    }
  }

  // When we have a mesh, the min, max of the Z is fully depending on the mesh else we use the remaining
  // constant depths
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  if (model.IsMesh()) {
    ret.first.Z(model.Mesh().Min().Z());
    ret.second.Z(model.Mesh().Max().Z());
  } else {
    // Adjust to constant depth
    for (size_t i = 0; i < vcConstantDepth.size(); i++) {
      // A constant depth surface could push into the depth by the mesher so if there is a mesh
      // take the min / max of the generated surface by the mesher
      const CHexaHorizon *pConstantDepth = vcConstantDepth[i];
      assert(pConstantDepth->ConstantDepth());
      ret.first.Z(std::min(ret.first.Z(), pConstantDepth->Depth().Value(CQuantity::SI_UNIT)));
      ret.second.Z(std::max(ret.second.Z(), pConstantDepth->Depth().Value(CQuantity::SI_UNIT)));
    }
  }

  // Snap to the grid ...
  return SnapToGrid(ret);
}

void CHexaBoundary::OnNewNeighbour(const CGraphNode &node) {
  // Update boundaries ...
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  const CHexaHorizon *pHorizon = dynamic_cast<const CHexaHorizon *>(&node);
  if (pHorizon && !model.Loading())
    OnUpdateBoundary();

  CInterfaceBoundary::OnNewNeighbour(node);
}

bool CHexaBoundary::OnSet(const TMinMax &minmax) {
  bool bRet = CInterfaceBoundary::OnSet(SnapToGrid(minmax));
  if (bRet)
    MainMeshRegion()->Invalidate();

  return bRet;
}

bool CHexaBoundary::OnSet(const TRotated &rotated) {
  bool bRet = CInterfaceBoundary::OnSet(SnapToGrid(rotated));
  if (bRet)
    MainMeshRegion()->Invalidate();

  return bRet;
}

void CHexaBoundary::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  const CModelBase &model = dynamic_cast<const CModelBase &>(Model());
  const CHexaHorizon *pHorizon = dynamic_cast<const CHexaHorizon *>(&node);

  if (pHorizon && !model.Loading() && uHint != MeshCleared)
    OnUpdateBoundary();

  CInterfaceBoundary::OnNeighbourModified(node, uHint);
}

void CHexaBoundary::OnUpdateBoundary() {
  TMinMax bestfit = BestFit();

  switch (State()) {
  case ROTATED:
    if (bestfit.first.Empty())
      return;
    Set(Mid(), Azimuth(), Length(), Width(), bestfit.first.Z(), bestfit.second.Z());
    break;
  case USER_DEFINED:
    if (bestfit.first.Empty())
      return;

    {
      geo::CPoint ptMin(MinPoint());
      geo::CPoint ptMax(MaxPoint());
      Set(geo::CPoint(ptMin.X(), ptMin.Y(), bestfit.first.Z()), geo::CPoint(ptMax.X(), ptMax.Y(), bestfit.second.Z()));
    }

    break;
  case DEFAULT_DEFINED:
    if (bestfit.first.Empty())
      return;
    State(BEST_FIT);
    break;
  case BEST_FIT:
    if (!bestfit.first.Empty() && !bestfit.second.Empty())
      Set(bestfit.first, bestfit.second);
    else
      State(DEFAULT_DEFINED);
    break;
  default:
    assert(false);
    break;
  };
}

CHexaBoundary::TMinMax CHexaBoundary::SnapToGrid(const TMinMax &minmax) const {
  TMinMax min_max_ret = minmax;
  const CHexaMainMeshRegion *pMain = MainMeshRegion();
  assert(pMain != 0);

  int nX = (int)floor((min_max_ret.second.X() - min_max_ret.first.X()) / pMain->GridSizeX() + 0.5);
  int nY = (int)floor((min_max_ret.second.Y() - min_max_ret.first.Y()) / pMain->GridSizeY() + 0.5);

  // Size as large as minimal the grid element ...
  if (nX == 0)
    nX = 1;
  if (nY == 0)
    nY = 1;

  // Snap ....
  min_max_ret.second.X(min_max_ret.first.X() + nX * pMain->GridSizeX());
  min_max_ret.second.Y(min_max_ret.first.Y() + nY * pMain->GridSizeY());

  // Restore original z coordinates
  min_max_ret.first.Z(minmax.first.Z());
  min_max_ret.second.Z(minmax.second.Z());

  return min_max_ret;
}

CHexaBoundary::TRotated CHexaBoundary::SnapToGrid(const TRotated &rotated) const {
  TRotated ret(rotated);

  const CHexaMainMeshRegion *pMain = MainMeshRegion();
  assert(pMain);

  // adjust length and width around midpoint
  int nL = int(floor(rotated.dLength / pMain->GridSizeX() + 0.5));
  int nW = int(floor(rotated.dWidth / pMain->GridSizeY() + 0.5));

  ret.dLength = nL * pMain->GridSizeX();
  ret.dWidth = nW * pMain->GridSizeY();

  return ret;
}

const CHexaMesh &CHexaBoundary::Mesh() const {
  const CHexaModel &model = dynamic_cast<const CHexaModel &>(Model());
  const CHexaMesh &mesh = dynamic_cast<const CHexaMesh &>(model.Mesh());

  return mesh;
}

CHexaMainMeshRegion *CHexaBoundary::MainMeshRegion() const {
  return CHexaMainMeshRegion::getHexaMainMeshRegion(dynamic_cast<const CModelBase &>(Model()));
}

// Top and bottom horizon
const CHexaHorizon &CHexaBoundary::GetTopHorizon() const {
  const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry *>(Model().GraphEntry(MD_HEXA_FORMATION));
  assert(pEntry);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  assert(!stNode.empty());
  const CHexaFormation *pFor = dynamic_cast<const CHexaFormation *>(*stNode.begin());
  while (pFor->UpperFormation())
    pFor = pFor->UpperFormation();
  return pFor->UpperHorizon();
}

const CHexaHorizon &CHexaBoundary::GetBottomHorizon() const {
  const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry *>(Model().GraphEntry(MD_HEXA_FORMATION));
  assert(pEntry);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  assert(!stNode.empty());
  const CHexaFormation *pFor = dynamic_cast<const CHexaFormation *>(*stNode.begin());
  while (pFor->LowerFormation())
    pFor = pFor->LowerFormation();
  return pFor->LowerHorizon();
}

std::vector<const geo::ISurface *> CHexaBoundary::GetSideMeshSurfaces() const {
  std::vector<const geo::ISurface *> vcSurface;

  const CHexaFormationEntry *pEntry = dynamic_cast<const CHexaFormationEntry *>(Model().GraphEntry(MD_HEXA_FORMATION));
  assert(pEntry);
  CHexaFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  assert(!stNode.empty());

  for (CHexaFormationEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); ++it) {
    const CHexaFormation *pFor = static_cast<const CHexaFormation *>(*it);
    assert(pFor->VolumeSize() == 1);
    const CHexaFormationVolume &vol = static_cast<const CHexaFormationVolume &>(pFor->Volume(0));
    const geo::CPtrArray<geo::IObject> &volSkin = vol.Skin();
    assert(!volSkin.Empty());
    std::set<const geo::IObject *, CMinMaxLess> stObjects;
    for (size_t i = 0; i < volSkin.Size(); ++i)
      stObjects.insert(&volSkin.Object(i));

    for (std::set<const geo::IObject *, CMinMaxLess>::const_iterator it2 = stObjects.begin(); it2 != stObjects.end();
         ++it2)
      vcSurface.push_back(static_cast<const geo::ISurface *>(*it2));
  }

  return vcSurface;
}

void CHexaBoundary::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  if (version < CStreamVersion(4, 1, 2))
    CBoundaryBase::LoadStream(stream, version, progress); // skip CInterfaceBoundary (use defaults)
  else
    CInterfaceBoundary::LoadStream(stream, version, progress);
}

bool CHexaBoundary::CanRotate() const { return true; }
