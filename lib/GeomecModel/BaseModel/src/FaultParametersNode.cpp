
#include "FaultParametersNode.h"
#include "BranchState.h"
#include "DepletionStage.h"
#include "FemAppEntryTypes.h"
#include "HorizonBase.h"
#include "IPointSet.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "StreamVersion.h"
#include "ValueComponent.h"
#include "ValueType.h"
#include "ValueTypes.h"
#include "resourceIDI.h"

namespace {

const double DEF_USER_DSTIF1 = 500;
const double DEF_USER_DSTIF2 = 50;

const double DEF_FRACTURE_DSTIF1 = 1000;
const double DEF_FRACTURE_DSTIF2 = 1000;

} // namespace

CFaultParametersNode::CFaultParametersNode(CHorizonBase &fault, CDepletionStage &stage)
    : CStorageNode(stage.Name(), fault.Model()), m_fault(fault), m_stage(stage), m_pBranchStage(0),
      m_bCohesionSameAsPrevious(!stage.Initial()), m_bFrictionSameAsPrevious(!stage.Initial()),
      m_bDStiffSameAsPrevious(!stage.Initial()), m_bCohesionExtrapolated(true), m_bFrictionExtrapolated(true),
      m_Cohesion(10), m_FrictionAngle(15), m_dUserDStiff1(DEF_USER_DSTIF1), m_dUserDStiff2(DEF_USER_DSTIF2),
      m_dFractureDStiff1(DEF_FRACTURE_DSTIF1), m_dFractureDStiff2(DEF_FRACTURE_DSTIF2), m_pValueTypeCohesion(0),
      m_pValueTypeFriction(0) {
  LinkTo(stage);
  reParent(&fault);
  if (!stage.Initial()) {
    m_pBranchStage = &stage.Previous();
    LinkTo(*m_pBranchStage);
  }
  LinkTo(((CModelBase &)fault.Model()).DepletionStageEntry());
}

CFaultParametersNode::CFaultParametersNode(const CFaultParametersNode &rhs)
    : CStorageNode(rhs), m_fault(rhs.m_fault), m_stage(rhs.m_stage), m_pBranchStage(rhs.m_pBranchStage),
      m_bCohesionSameAsPrevious(rhs.m_bCohesionSameAsPrevious),
      m_bFrictionSameAsPrevious(rhs.m_bFrictionSameAsPrevious), m_bDStiffSameAsPrevious(rhs.m_bDStiffSameAsPrevious),
      m_bCohesionExtrapolated(rhs.m_bCohesionExtrapolated), m_bFrictionExtrapolated(rhs.m_bFrictionExtrapolated),
      m_Cohesion(rhs.m_Cohesion), m_FrictionAngle(rhs.m_FrictionAngle), m_dUserDStiff1(rhs.m_dUserDStiff1),
      m_dUserDStiff2(rhs.m_dUserDStiff2), m_dFractureDStiff1(rhs.m_dFractureDStiff1),
      m_dFractureDStiff2(rhs.m_dFractureDStiff2), m_pValueTypeCohesion(rhs.m_pValueTypeCohesion),
      m_pValueTypeFriction(rhs.m_pValueTypeFriction) {}

CFaultParametersNode &CFaultParametersNode::operator=(const CFaultParametersNode &rhs) {
  assert(!IsCopy());
  assert(rhs.IsCopy());

  if (!(*this == rhs)) {
    CModelBase &model = static_cast<CModelBase &>(Model());
    model.ResultRegister().ClearLinear();
    model.ResultRegister().ClearNonLinear();
    model.ResultRegister().ClearMixture();
  }

  CStorageNode::operator=(rhs);

  assert(&m_fault == &rhs.m_fault);
  assert(&m_stage == &rhs.m_stage);

  m_pBranchStage = rhs.m_pBranchStage;
  m_bCohesionSameAsPrevious = rhs.m_bCohesionSameAsPrevious;
  m_bFrictionSameAsPrevious = rhs.m_bFrictionSameAsPrevious;
  m_bDStiffSameAsPrevious = rhs.m_bDStiffSameAsPrevious;
  m_bCohesionExtrapolated = rhs.m_bCohesionExtrapolated;
  m_bFrictionExtrapolated = rhs.m_bFrictionExtrapolated;
  m_Cohesion = rhs.m_Cohesion;
  m_FrictionAngle = rhs.m_FrictionAngle;
  m_dUserDStiff1 = rhs.m_dUserDStiff1;
  m_dUserDStiff2 = rhs.m_dUserDStiff2;
  m_dFractureDStiff1 = rhs.m_dFractureDStiff1;
  m_dFractureDStiff2 = rhs.m_dFractureDStiff2;
  m_pValueTypeCohesion = rhs.m_pValueTypeCohesion;
  m_pValueTypeFriction = rhs.m_pValueTypeFriction;

  return *this;
}

bool CFaultParametersNode::operator==(const CFaultParametersNode &rhs) const {
  return (&m_fault == &rhs.m_fault && &m_stage == &rhs.m_stage && m_pBranchStage == rhs.m_pBranchStage &&
          m_bCohesionSameAsPrevious == rhs.m_bCohesionSameAsPrevious &&
          m_bFrictionSameAsPrevious == rhs.m_bFrictionSameAsPrevious &&
          m_bDStiffSameAsPrevious == rhs.m_bDStiffSameAsPrevious &&
          m_bCohesionExtrapolated == rhs.m_bCohesionExtrapolated &&
          m_bFrictionExtrapolated == rhs.m_bFrictionExtrapolated && m_Cohesion == rhs.m_Cohesion &&
          m_FrictionAngle == rhs.m_FrictionAngle && m_dUserDStiff1 == rhs.m_dUserDStiff1 &&
          m_dUserDStiff2 == rhs.m_dUserDStiff2 && m_dFractureDStiff1 == rhs.m_dFractureDStiff1 &&
          m_dFractureDStiff2 == rhs.m_dFractureDStiff2 && m_pValueTypeCohesion == rhs.m_pValueTypeCohesion &&
          m_pValueTypeFriction == rhs.m_pValueTypeFriction);
}

void CFaultParametersNode::OnNewNeighbour(const CGraphNode &node) {
  const CValueType *pValueType = dynamic_cast<const CValueType *>(&node);
  if (pValueType) {
    assert((pValueType->TypeId() == IDT_VALUETYPE_COHESION) || (pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE));

    // Invalidate results
    CModelBase &model = dynamic_cast<CModelBase &>(Model());
    model.ResultRegister().ClearLinear(false);
    model.ResultRegister().ClearNonLinear(false);
    model.ResultRegister().ClearMixture();

    if (pValueType->TypeId() == IDT_VALUETYPE_COHESION) {
      assert(m_pValueTypeCohesion == 0);
      m_pValueTypeCohesion = pValueType;
    } else if (pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE) {
      assert(m_pValueTypeFriction == 0);
      m_pValueTypeFriction = pValueType;
    }

    Modified();
  }

  CStorageNode::OnNewNeighbour(node);
}

void CFaultParametersNode::OnNeighbourDeleted(const CGraphNode &node) {
  if (m_pValueTypeCohesion == &node) {
    m_pValueTypeCohesion = 0;
    Modified();
  } else if (m_pValueTypeFriction == &node) {
    m_pValueTypeFriction = 0;
    Modified();
  }

  bool bDelete = false;

  if (&node == m_pBranchStage || &node == &m_stage)
    bDelete = true;

  CStorageNode::OnNeighbourDeleted(node);

  if (bDelete)
    delete this;
}

void CFaultParametersNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint /*uHint*/) {
  if (m_pValueTypeCohesion == &node || m_pValueTypeFriction == &node)
    Modified();

  // for now use 'canHaveFaultParameters' (the original 'HasFaultParameters'),
  // perhaps the new 'hasFaultParameters' is more appropriate

  if (m_pBranchStage && &node == &((CModelBase &)Model()).DepletionStageEntry() &&
      !m_fault.canHaveFaultParameters(m_stage))
    delete this;
}

bool CFaultParametersNode::CanConnectItem(const CGraphNode &item) const {
  if (IsLinkedTo(item))
    return false;

  const CValueType *pValueType = dynamic_cast<const CValueType *>(&item);
  if (pValueType) {
    return ((pValueType->TypeId() == IDT_VALUETYPE_COHESION && !m_pValueTypeCohesion) ||
            (pValueType->TypeId() == IDT_VALUETYPE_FRICTION_ANGLE && !m_pValueTypeFriction)) &&
           !(static_cast<const CModelBase &>(Model())).BranchState().IsFixedStage(m_stage);
  }

  return CStorageNode::CanConnectItem(item);
}

bool CFaultParametersNode::CanDisconnectItem(const CGraphNode &item) const {
  const CValueType *pValueType = dynamic_cast<const CValueType *>(&item);
  if (pValueType)
    return !(static_cast<const CModelBase &>(Model())).BranchState().IsFixedStage(m_stage);

  return CStorageNode::CanDisconnectItem(item);
}

unsigned int CFaultParametersNode::IconId() const {
  if (CohesionSameAsPrevious() && FrictionSameAsPrevious() && DStiffSameAsPrevious() && !DistributedCohesion() &&
      !DistributedFriction())
    return IDI_FAULTPARAMETERS_REPEAT;

  return IDI_FAULTPARAMETERS;
}

unsigned int CFaultParametersNode::TypeId() const { return IDT_VALUETYPE_FAULTPARAMETERS; }

bool CFaultParametersNode::Empty() const { return false; }

long CFaultParametersNode::SavedItems() const { return 1; }

void CFaultParametersNode::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  int n;

  stream >> n;
  m_bCohesionSameAsPrevious = (n != 0);

  stream >> n;
  m_bFrictionSameAsPrevious = (n != 0);

  if (CStreamVersion(3, 0, 118) < version) {
    stream >> n;
    m_bDStiffSameAsPrevious = (n != 0);
  }

  stream >> n;
  m_bCohesionExtrapolated = (n != 0);

  stream >> n;
  m_bFrictionExtrapolated = (n != 0);

  double dCohesion;
  stream >> dCohesion;
  m_Cohesion = dCohesion;

  double dFriction;
  stream >> dFriction;
  m_FrictionAngle = dFriction;

  if (CStreamVersion(3, 0, 118) < version) {
    stream >> m_dUserDStiff1;
    stream >> m_dUserDStiff2;
  }

  if (version >= CStreamVersion(3, 7, 28)) {
    stream >> m_dFractureDStiff1;
    stream >> m_dFractureDStiff2;
  }

  int i;
  for (i = 0; i < 2; ++i) {
    TValueCompositeEntry &composite_entry = (TValueCompositeEntry &)*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE);
    stream >> n;
    if (n >= 0) {
      composite_entry.LinkNodeToIndex(*this, n);
    }
  }

  progress.Step();
}

void CFaultParametersNode::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  int n;

  n = m_bCohesionSameAsPrevious ? 1 : 0;
  stream << n;

  n = m_bFrictionSameAsPrevious ? 1 : 0;
  stream << n;

  n = m_bDStiffSameAsPrevious ? 1 : 0;
  stream << n;

  n = m_bCohesionExtrapolated ? 1 : 0;
  stream << n;

  n = m_bFrictionExtrapolated ? 1 : 0;
  stream << n;

  double dCohesion = m_Cohesion.Value();
  stream << dCohesion;

  double dFriction = m_FrictionAngle.Value();
  stream << dFriction;

  stream << m_dUserDStiff1;
  stream << m_dUserDStiff2;
  stream << m_dFractureDStiff1;
  stream << m_dFractureDStiff2;

  if (m_pValueTypeCohesion)
    stream << m_pValueTypeCohesion->Index();
  else
    stream << -1;

  if (m_pValueTypeFriction)
    stream << m_pValueTypeFriction->Index();
  else
    stream << -1;

  progress.Step();
}

bool CFaultParametersNode::CohesionSameAsPrevious() const { return !m_stage.Initial() && m_bCohesionSameAsPrevious; }

bool CFaultParametersNode::FrictionSameAsPrevious() const { return !m_stage.Initial() && m_bFrictionSameAsPrevious; }

bool CFaultParametersNode::DStiffSameAsPrevious() const { return !m_stage.Initial() && m_bDStiffSameAsPrevious; }

void CFaultParametersNode::CohesionSameAsPrevious(bool b) { m_bCohesionSameAsPrevious = b; }

void CFaultParametersNode::FrictionSameAsPrevious(bool b) { m_bFrictionSameAsPrevious = b; }

void CFaultParametersNode::DStiffSameAsPrevious(bool b) { m_bDStiffSameAsPrevious = b; }

const CCohesionQuantity &CFaultParametersNode::Cohesion() const { return m_Cohesion; }

const CAngleQuantity &CFaultParametersNode::FrictionAngle() const { return m_FrictionAngle; }

double CFaultParametersNode::UserDStiff1() const {
  if (m_fault.SlipType() == CHorizonBase::FRACTURE)
    return m_dFractureDStiff1;

  return m_dUserDStiff1;
}

double CFaultParametersNode::UserDStiff2() const {
  if (m_fault.SlipType() == CHorizonBase::FRACTURE)
    return m_dFractureDStiff2;

  return m_dUserDStiff2;
}

void CFaultParametersNode::Cohesion(double dCohesion) { m_Cohesion = dCohesion; }

void CFaultParametersNode::FrictionAngle(double dFrictionAngle) { m_FrictionAngle = dFrictionAngle; }

void CFaultParametersNode::UserDStiff1(double dDStiff1) {
  if (m_fault.SlipType() == CHorizonBase::FRACTURE)
    m_dFractureDStiff1 = dDStiff1;
  else
    m_dUserDStiff1 = dDStiff1;
}

void CFaultParametersNode::UserDStiff2(double dDStiff2) {
  if (m_fault.SlipType() == CHorizonBase::FRACTURE)
    m_dFractureDStiff2 = dDStiff2;
  else
    m_dUserDStiff2 = dDStiff2;
}

bool CFaultParametersNode::DistributedCohesion() const { return (m_pValueTypeCohesion != 0); }

bool CFaultParametersNode::DistributedFriction() const { return (m_pValueTypeFriction != 0); }

bool CFaultParametersNode::CohesionExtrapolated() const { return m_bCohesionExtrapolated; }

bool CFaultParametersNode::FrictionExtrapolated() const { return m_bFrictionExtrapolated; }

void CFaultParametersNode::CohesionExtrapolated(bool b) { m_bCohesionExtrapolated = b; }

void CFaultParametersNode::FrictionExtrapolated(bool b) { m_bFrictionExtrapolated = b; }

double CFaultParametersNode::Cohesion(const geo::IPoint &point, const CQuantity::UNIT unit) const {
  if (m_pValueTypeCohesion) {
    if (m_bCohesionExtrapolated || m_pValueTypeCohesion->PointSet().PointInConvexHull(point)) {
      geo::CValue value = m_pValueTypeCohesion->Component().ScalarData().ValuePoint(point, unit);
      if (value.Valid())
        return value.Value();
      return Cohesion().Value(unit);
    }
  }

  if (m_bCohesionSameAsPrevious && !m_stage.Initial())
    return Previous()->Cohesion(point, unit);

  return Cohesion().Value(unit);
}

std::vector<double> CFaultParametersNode::Cohesion(const geo::IElement &element, const CQuantity::UNIT unit) const {
  std::vector<double> vcResult(element.NrOfPoints());

  if (m_pValueTypeCohesion) {
    if (m_bCohesionExtrapolated || m_pValueTypeCohesion->PointSet().ElementInConvexHull(element)) {
      IValueDomainScalar::TValueVec vcValue =
          m_pValueTypeCohesion->Component().ScalarData().ValueElement(element, unit);
      assert(vcValue.size() == vcResult.size());
      for (size_t j = 0; j < vcResult.size(); ++j) {
        if (vcValue[j].Valid())
          vcResult[j] = vcValue[j].Value();
        else
          vcResult[j] = Cohesion().Value(unit);
      }

      return vcResult;
    }
  }

  if (m_bCohesionSameAsPrevious && !m_stage.Initial())
    return Previous()->Cohesion(element, unit);

  for (size_t i = 0; i < vcResult.size(); ++i)
    vcResult[i] = Cohesion().Value(unit);

  return vcResult;
}

double CFaultParametersNode::FrictionAngle(const geo::IPoint &point, const CQuantity::UNIT unit) const {
  if (m_pValueTypeFriction) {
    if (m_bFrictionExtrapolated || m_pValueTypeFriction->PointSet().PointInConvexHull(point)) {
      geo::CValue value = m_pValueTypeFriction->Component().ScalarData().ValuePoint(point, unit);
      if (value.Valid())
        return value.Value();
      return FrictionAngle().Value(unit);
    }
  }

  if (m_bFrictionSameAsPrevious && !m_stage.Initial())
    return Previous()->FrictionAngle(point, unit);

  return FrictionAngle().Value(unit);
}

std::vector<double> CFaultParametersNode::FrictionAngle(const geo::IElement &element,
                                                        const CQuantity::UNIT unit) const {
  std::vector<double> vcResult(element.NrOfPoints());

  if (m_pValueTypeFriction) {
    if (m_bFrictionExtrapolated || m_pValueTypeFriction->PointSet().ElementInConvexHull(element)) {
      IValueDomainScalar::TValueVec vcValue =
          m_pValueTypeFriction->Component().ScalarData().ValueElement(element, unit);
      assert(vcValue.size() == vcResult.size());
      for (size_t j = 0; j < vcResult.size(); j++) {
        if (vcValue[j].Valid())
          vcResult[j] = vcValue[j].Value();
        else
          vcResult[j] = FrictionAngle().Value(unit);
      }

      return vcResult;
    }
  }

  if (m_bFrictionSameAsPrevious && !m_stage.Initial())
    return Previous()->FrictionAngle(element, unit);

  for (size_t i = 0; i < vcResult.size(); i++)
    vcResult[i] = FrictionAngle().Value(unit);

  return vcResult;
}

double CFaultParametersNode::DStiffNormal() const {
  if (m_bDStiffSameAsPrevious && !m_stage.Initial())
    return Previous()->DStiffNormal();

  return UserDStiff1();
}

double CFaultParametersNode::DStiffShear() const {
  if (m_bDStiffSameAsPrevious && !m_stage.Initial())
    return Previous()->DStiffShear();

  return UserDStiff2();
}

const CFaultParametersNode *CFaultParametersNode::Previous() const {
  return (const_cast<CFaultParametersNode *>(this))->Previous();
}

CFaultParametersNode *CFaultParametersNode::Previous() {
  if (m_stage.Initial())
    return 0;

  return &m_fault.FaultParameters(m_stage.Previous());
}

const CDepletionStage &CFaultParametersNode::Stage() const { return m_stage; }

const CHorizonBase &CFaultParametersNode::Fault() const { return m_fault; }

bool CFaultParametersNode::Less(const CGraphNode &node) const {
  const CFaultParametersNode *pNode = dynamic_cast<const CFaultParametersNode *>(&node);
  if (pNode)
    return m_stage.Less(pNode->m_stage);

  return CStorageNode::Less(node);
}
