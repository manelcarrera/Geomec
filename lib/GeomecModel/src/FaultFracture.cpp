#include <utility>

#include "BranchState.h"
#include "FaultFracture.h"
#include "FaultParametersNode.h"
#include "HorizonBase.h"
#include "ModelBase.h"
#include "unitnode.h"

CFaultFracture::CFaultFracture(CHorizonBase *horizonBase, CDoubleQuantity::UNIT unit)
    : m_horizonBase(horizonBase), m_slipType(m_horizonBase->SlipType()), m_faultParametersNode(0), m_unit(unit) {
  CDepletionStage &initialDepletionStage = static_cast<CModelBase &>(m_horizonBase->Model()).InitialDepletionStage();

  if (m_horizonBase->Slip() && m_horizonBase->hasFaultParameters(initialDepletionStage)) {
    m_faultParametersNode = &m_horizonBase->FaultParameters(initialDepletionStage);

    m_cohesionUnitName = m_faultParametersNode->Cohesion().UnitName(m_unit).c_str();
    m_frictionAngleUnitName = m_faultParametersNode->FrictionAngle().UnitName(m_unit).c_str();

    // waij TFS 100415 SI/FIELD
    m_cohesion = m_faultParametersNode->Cohesion().Value(m_unit);
    m_frictionAngle = m_faultParametersNode->FrictionAngle().Value(m_unit);
  }
}

CFaultFracture::CFaultFracture(const CFaultFracture &rhs)
    : m_horizonBase(rhs.m_horizonBase), m_slipType(rhs.m_slipType), m_faultParametersNode(rhs.m_faultParametersNode),
      m_cohesionUnitName(rhs.m_cohesionUnitName), m_frictionAngleUnitName(rhs.m_frictionAngleUnitName),
      m_cohesion(rhs.m_cohesion), m_frictionAngle(rhs.m_frictionAngle), m_unit(rhs.m_unit) {}

CFaultFracture &CFaultFracture::operator=(CFaultFracture &rhs) {
  m_horizonBase = rhs.m_horizonBase;
  m_slipType = rhs.m_slipType;

  m_faultParametersNode = rhs.m_faultParametersNode;

  m_cohesionUnitName = rhs.m_cohesionUnitName;
  m_frictionAngleUnitName = rhs.m_frictionAngleUnitName;

  m_cohesion = rhs.m_cohesion;
  m_frictionAngle = rhs.m_frictionAngle;

  return *this;
}

const QString &CFaultFracture::name() const { return m_horizonBase->Name(); }

bool CFaultFracture::HasHorizon(const CHorizonBase *horizonBase) const { return m_horizonBase == horizonBase; }

bool CFaultFracture::Slip() const { return m_horizonBase->Slip(); }

bool CFaultFracture::IsBranch() const {
  return static_cast<CModelBase &>(m_horizonBase->Model()).BranchState().IsBranch();
}

bool CFaultFracture::hasFaultParametersForInitialDepletionStage() const { return m_faultParametersNode != 0; }

QString CFaultFracture::CohesionUnitName() const { return m_cohesionUnitName; }

double CFaultFracture::Cohesion() const { return m_cohesion; }

void CFaultFracture::Cohesion(double value) { m_cohesion = value; }

QString CFaultFracture::FrictionAngleUnitName() const { return m_frictionAngleUnitName; }

double CFaultFracture::FrictionAngle() const { return m_frictionAngle; }

void CFaultFracture::FrictionAngle(double value) { m_frictionAngle = value; }

int CFaultFracture::SlipType() const { return m_slipType; }

void CFaultFracture::SlipType(int slipType) { m_slipType = slipType; }

void CFaultFracture::FinalizeBehaviour() { m_horizonBase->SlipType(static_cast<CHorizonBase::TSlipType>(m_slipType)); }

void CFaultFracture::FinalizeInitD0() {
  if (m_faultParametersNode) {
    CCohesionQuantity cohesionQuantity = m_faultParametersNode->Cohesion();

    double dCohesion = cohesionQuantity.Convert(m_cohesion, CDoubleQuantity::SI_UNIT, m_unit);

    m_faultParametersNode->Cohesion(dCohesion);
    m_faultParametersNode->FrictionAngle(m_frictionAngle);
  }
}
