#include "stdafx.h"

#include "FaultFracture.h"
#include "HandleDefaultFaultParameters.h"

CHandleDefaultFaultParameters::CHandleDefaultFaultParameters(CFaultFracture *faultFracture)
    : m_faultFracture(faultFracture), m_cohesionUnit(), m_frictionAngleUnit(), m_cohesion(), m_frictionAngle() {
  if (m_faultFracture->Slip() && m_faultFracture->hasFaultParametersForInitialDepletionStage()) {
    m_cohesionUnit = m_faultFracture->CohesionUnitName().toStdString().c_str();
    m_frictionAngleUnit = m_faultFracture->FrictionAngleUnitName().toStdString().c_str();

    m_cohesion = m_faultFracture->Cohesion();
    m_frictionAngle = m_faultFracture->FrictionAngle();
  }
}

void CHandleDefaultFaultParameters::DoDataExchange(CDataExchange *pDX) {
  DDX_Text(pDX, IDC_UN_COHESION, m_cohesionUnit);
  DDX_Text(pDX, IDC_UN_FRICTION, m_frictionAngleUnit);

  if (m_faultFracture->Slip() && m_faultFracture->hasFaultParametersForInitialDepletionStage()) {
    DDX_Text(pDX, IDC_ED_COHESION, m_cohesion);
    DDX_Text(pDX, IDC_ED_FRICTION, m_frictionAngle);

    if (m_faultFracture->Slip() && m_faultFracture->hasFaultParametersForInitialDepletionStage()) {
      m_faultFracture->Cohesion(m_cohesion);
      m_faultFracture->FrictionAngle(m_frictionAngle);
    }
  }
}
