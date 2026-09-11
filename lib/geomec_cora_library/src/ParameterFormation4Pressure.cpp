#include "ParameterFormation4Pressure.h"

namespace cora
{

CParameterFormation4Pressure::CParameterFormation4Pressure(const QString& name,
  const geo::CValue& minimum, const geo::CValue& maximum, double mean,
  CFormationBase* formationBase, int depletionStage, unsigned int valueTypeID)
: CParameterFormation(name, minimum, maximum, mean, formationBase,
    depletionStage)
, m_valueTypeID(valueTypeID)
{
}

CParameterFormation4Pressure::~CParameterFormation4Pressure()
{
}

unsigned int CParameterFormation4Pressure::valueTypeID() const
{
  return m_valueTypeID;
}

} // namespace cora
