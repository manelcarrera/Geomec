
#include "ValidateModel.h"
#include "DisplacementSupportNode.h"
#include "FormationBase.h"
#include "ModelBase.h"
#include "PressureSupportNode.h"
#include "SurfaceDesc.h"
#include "TetSurface.h"
#include "TetraBoundary.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraHorizonBase.h"
#include "TetraModel.h"

CValidateModel::CValidateModel(CModelBase *modelBase) : m_ModelBase(modelBase) {}

bool CValidateModel::checkModel(bool rockMech) const {
  bool valid = true;

  valid = valid && (rockMech ? true : checkMesh());
  valid = valid && (rockMech ? true : checkReservoir());
  valid = valid && checkExistenceDepletionStage();
  valid = valid && checkMaterial();

  // valid = valid && checkValues();

  CTetraModel *tetraModel = dynamic_cast<CTetraModel *>(m_ModelBase);

  if (tetraModel) {
    valid = valid && (rockMech ? checkMesh() : true);
    valid = valid && checkBoundaryConditions();
    valid = valid && checkNoEmptyFormations();
    valid = valid && checkNoEmptyFaults();
  }

  return valid;
}

// Check if a mesh exists.

bool CValidateModel::checkMesh() const { return m_ModelBase->IsMesh(); }

// Check if at least one reservoir is defined.

bool CValidateModel::checkReservoir() const {
  TFormationBaseEntry *pEntry = static_cast<TFormationBaseEntry *>(m_ModelBase->GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();
  int nDepleting = 0;
  for (TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++) {
    if ((*it)->Depleting()) {
      nDepleting++;
      const CDepletionStage *pStage = &(m_ModelBase->InitialDepletionStage());
      while (pStage) {
        if ((*it)->InvalidValue(*pStage, CFormationBase::PRESSURE))
          return false;

        if (pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }
    }
  }

  if (nDepleting)
    return true;

  return false;
}

// Check if all formations have a material assigned to them.

bool CValidateModel::checkMaterial() const {
  TFormationBaseEntry *pEntry = static_cast<TFormationBaseEntry *>(m_ModelBase->GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  for (TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++) {
    if (!checkMaterial(**it))
      return false;
  }

  if (stFormation.size())
    return true;

  return false;
}

bool CValidateModel::checkMaterial(const CFormationBase &formation) const {
  const CDepletionStage *pStage = &(m_ModelBase->InitialDepletionStage());
  return formation.Material(*pStage).Valid();
}

bool CValidateModel::checkValues() const {
  TFormationBaseEntry *pEntry = static_cast<TFormationBaseEntry *>(m_ModelBase->GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  for (TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); ++it) {
    if (!checkValues(**it))
      return false;
  }

  return stFormation.size() > 0;
}

bool CValidateModel::checkValues(const CFormationBase &formation) const {
  const CDepletionStage *pStage = &(m_ModelBase->InitialDepletionStage());
  return formation.Material(*pStage).ValidValues();
}

// Check if at least one depletion stage is defined.

bool CValidateModel::checkExistenceDepletionStage() const { return (m_ModelBase->NrOfDepletionStages() - 1 > 0); }

// This check is only performed if the model is a tetrahedron model.
// For all the side surfaces and bottom surface the type of boundary condition
// is checked. If one of the surfaces is fully fixed the model can not move.
// If the surfaces are only supported in the normal direction, a check needs
// to be performed that the model as a whole can not move.

bool CValidateModel::checkBoundaryConditions() const {
  CTetraModel *pMod = dynamic_cast<CTetraModel *>(m_ModelBase);
  assert(pMod);

  if ((pMod == 0) || !checkMesh()) {
    return false;
  }

  CBoundaryBase *pBoundBas = &pMod->Boundary();
  CTetraBoundary *pTetBound = dynamic_cast<CTetraBoundary *>(pBoundBas);
  assert(pTetBound);

  std::vector<geo::CVector> vcNormals;
  vcNormals.clear();
  geo::CVector Normal(0, 0, 0);
  geo::CPoint RefPt = pTetBound->DisplacementSupportNode().ReferencePoint();

  CTetraHorizonBase *pTetHorBas = pTetBound->BottomHorizon();

  int i;
  for (i = 0; i < pTetHorBas->OutputSurfaceSize(); i++) {
    if (collectNormalVectors(pTetHorBas->OutputSurface(i), vcNormals, pTetBound)) {
      return true;
    }
  }

  for (i = 0; i < pTetBound->SideSurfaceSize(); i++) {
    if (collectNormalVectors(pTetBound->SideSurfaceDesc(i), vcNormals, pTetBound)) {
      return true;
    }
  }

  return false;
}

bool CValidateModel::checkNoEmptyFormations() const {
  CTetraModel *tetraModel = dynamic_cast<CTetraModel *>(m_ModelBase);

  assert(tetraModel);

  if (tetraModel == 0) {
    return false;
  }

  // Get the formations
  CTetraFormationEntry *pEntry = static_cast<CTetraFormationEntry *>(m_ModelBase->GraphEntry(MD_TETRA_FORMATION));
  assert(pEntry);
  CTetraFormationEntry::TNodeSet stNode = pEntry->EntryNodes();
  for (CTetraFormationEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++) {
    if ((*it)->ElementSetSize() == 0)
      return false;
  }

  return true;
}

bool CValidateModel::checkNoEmptyFaults() const {
  THorizonBaseEntry::TEntryNodeSet stNode = m_ModelBase->GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();

  THorizonBaseEntry::TEntryNodeSet::iterator it;
  for (it = stNode.begin(); it != stNode.end(); ++it) {
    CHorizonBase &hor = static_cast<CHorizonBase &>(**it);
    if (hor.Slip() && !hor.IgnoreFault()) {
      if (!hor.InterfaceElementGroup())
        return false;
    }
  }

  return true;
}

// private

bool CValidateModel::collectNormalVectors(const geo::CSurfaceDesc &Desc, std::vector<geo::CVector> &vcNormals,
                                          CTetraBoundary *pTetBound) const {
  const CPressureSupportNode &PresSupNod = pTetBound->PressureSupportNode();
  CPressureSupportNode::CFaceSupportDef::_SupportType SupType;

  for (int j = 0; j < Desc.TetSurfaceSize(); j++) {
    CPressureSupportNode::CSurfaceDef Def1(Desc.TetSurface(j));
    SupType = PresSupNod.FaceSupportDef(Def1).SupportType();

    switch (SupType) {
    case CPressureSupportNode::CFaceSupportDef::ST_NORMAL: {
      addNorm(Desc.TetSurface(j), vcNormals);
      if (vcNormals.size() > 2)
        return true;
    } break;
    case CPressureSupportNode::CFaceSupportDef::ST_FULL:
    case CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL:
    case CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED:
    case CPressureSupportNode::CFaceSupportDef::ST_VERTICAL:
      return true;
      break;
    case CPressureSupportNode::CFaceSupportDef::ST_STRESS:
      break;
    default:
      assert(false);
    }
  }

  return false;
}

/*
 *  This private function is used when the model is a tetrahedron model.
 *  If some of the sidesurfaces have a NORMAL support type, the average normal
 *  in the points is used to calculate the average normal for the whole surface.
 *  This average normal of the incoming surface is onyl added to the vector of
 *  vectors if it is not parallel to the vector already present in the vector,
 *  or parallel to the plane that the vectors that are present in the vector
 *  span. (I know this sounds complicated...) If the vector contains three
 *  vectors after this function has been executed, the tetrahedron model has
 *  sufficient boundary conditions set.
 */

void CValidateModel::addNorm(const geo::ISurface &surf, std::vector<geo::CVector> &normvec) const {
  geo::CVector normal(0, 0, 0);
  for (int i = 0; i < surf.PointSize(); i++) {
    normal = normal + surf.AverageNormalInPoint(surf.Point(i));
  }

  int vecsize = normvec.size();
  if (vecsize == 0) {
    normvec.push_back(normal);
    return;
  }

  if (vecsize == 1) {
    if (normal.AngleDeg(normvec[0]) < EPS)
      return; // new normal is pointing in the same direction as the one already present....

    normvec.push_back(normal);
  }

  if (vecsize > 1) { // two vectors present and the reference point lies in the same plane as the two vectors
    assert(vecsize == 2);
    geo::CPlane plane(geo::CPoint(0, 0, 0), normvec[0], normvec[1]);
    if (plane.IsParallel(normal))
      return;

    normvec.push_back(normal);
  }
}
