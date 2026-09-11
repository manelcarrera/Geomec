#include "ParameterFormation4PressureChange.h"

namespace cora
{

CParameterFormation4PressureChange::CParameterFormation4PressureChange(
  const QString& name, const geo::CValue& minimum, const geo::CValue& maximum,
  double mean, CFormationBase* formationBase, int depletionStage,
  unsigned int valueTypeID)
: CParameterFormation(name, minimum, maximum, mean, formationBase,
    depletionStage)
, m_valueTypeID(valueTypeID)
{
}

CParameterFormation4PressureChange::~CParameterFormation4PressureChange()
{
}

unsigned int CParameterFormation4PressureChange::valueTypeID() const
{
  return m_valueTypeID;
}

} // namespace cora
