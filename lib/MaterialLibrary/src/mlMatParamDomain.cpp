#include "mlMatParamDomain.h"

#include <QtCore/QObject>

namespace ml {

CMatParamDomain::CMatParamDomain(bool bMinIsInf, double dMin, bool bIncludeMin, bool bMaxIsInf, double dMax, bool bIncludeMax)
: m_bMinIsInf(bMinIsInf),
  m_dMin(dMin),
  m_bIncludeMin(bIncludeMin),
  m_bMaxIsInf(bMaxIsInf),
  m_dMax(dMax),
  m_bIncludeMax(bIncludeMax)
{
}

CMatParamDomain::CMatParamDomain(const CMatParamDomain& rhs)
: m_bMinIsInf(rhs.m_bMinIsInf),
  m_dMin(rhs.m_dMin),
  m_bIncludeMin(rhs.m_bIncludeMin),
  m_bMaxIsInf(rhs.m_bMaxIsInf),
  m_dMax(rhs.m_dMax),
  m_bIncludeMax(rhs.m_bIncludeMax)
{
}

CMatParamDomain* CMatParamDomain::Clone() const
{
  return new CMatParamDomain(*this);
}

bool CMatParamDomain::operator()(double dValue, const CMatParam& param, QString& strErrorMsg, int nUnitDef) const
{
  dValue = param.UnitConversion().FromUserUnit(dValue, nUnitDef);

  if((m_bMinIsInf || dValue > m_dMin || (m_bIncludeMin && dValue == m_dMin)) &&
     (m_bMaxIsInf || dValue < m_dMax || (m_bIncludeMax && dValue == m_dMax)))
     return true;

  double dMin = param.UnitConversion().ToUserUnit(m_dMin, nUnitDef);
  double dMax = param.UnitConversion().ToUserUnit(m_dMax, nUnitDef);

  if(!m_bMinIsInf && !m_bMaxIsInf)
  {
  if(m_bIncludeMax)
  {
      if(m_bIncludeMin)
    strErrorMsg = QObject::tr("Value must be at least %1 and at most %2").arg(dMin).arg(dMax);
      else
    strErrorMsg = QObject::tr("Value must be greater than %1 and at most %2").arg(dMin).arg(dMax);
  }
  else
  {
      if(m_bIncludeMin)
    strErrorMsg = QObject::tr("Value must be at least %1 and less than %2").arg(dMin).arg(dMax);
      else
    strErrorMsg = QObject::tr("Value must be greater than %1 and less than %2").arg(dMin).arg(dMax);
  }
  }
  else if(m_bMaxIsInf)
  {
  if(m_bIncludeMin)
      strErrorMsg = QObject::tr("Value must be at least %1").arg(dMin);
  else
      strErrorMsg = QObject::tr("Value must be greater than %1").arg(dMin);
  }
  else
  {
  if(m_bIncludeMax)
      strErrorMsg = QObject::tr("Value must be at most %1").arg(dMax);
  else
      strErrorMsg = QObject::tr("Value must be less than %1").arg(dMax);
  }

  return false;
}

bool CMatParamDomain::IsDomainCheck() const
{ 
  return true;
}


} // namespace ml
