// GMPressure.cpp: implementation of the CPressure class.
//
//////////////////////////////////////////////////////////////////////
#include "GMPressure.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm {

CPressure::CPressure() : m_type(UNDEFINED), m_dReferenceDepth(0), m_dReferencePressure(0) {
  assert(m_vcGradient.size() == 0);
}

CPressure::~CPressure() {}

const double &CPressure::ReferenceDepth() const {
  assert(Constant() || GWC());
  return m_dReferenceDepth;
}

const double &CPressure::ReferencePressure() const {
  assert(Constant() || GWC());
  return m_dReferencePressure;
}

const double &CPressure::Gradient() const {
  assert(Constant());
  assert(m_vcGradient.size() == 1);
  return m_vcGradient[0];
}

const double &CPressure::UpperGradient() const {
  assert(m_vcGradient.size() > 0);
  return m_vcGradient[0];
}

const double &CPressure::LowerGradient() const {
  assert(m_vcGradient.size() > 0);
  return m_vcGradient[1];
}

CPressure::PressureType CPressure::Type() const { return m_type; }

bool CPressure::UnDefined() const { return m_type == UNDEFINED; }

bool CPressure::GWC() const { return m_type == OWCGWC; }

bool CPressure::Constant() const { return m_type == CONSTANT; }

bool CPressure::Distributed() const { return m_type == DISTRIBUTED; }

const QString &CPressure::SourceFileName() const { return m_strSourceFileName; }

int CPressure::PointSize() const {
  assert(Distributed());
  return m_vcPoint.size();
}

const CPressure::TPressurePoint &CPressure::Point(int nIndex) const {
  assert(Distributed());
  return m_vcPoint[nIndex];
}

int CPressure::IntervalSize() const {
  assert(Distributed());
  return m_vcInterval.size();
}

const geo::IPoint &CPressure::Interval(int nIndex) const {
  assert(Distributed());
  return m_vcInterval[nIndex];
}

} // namespace gm