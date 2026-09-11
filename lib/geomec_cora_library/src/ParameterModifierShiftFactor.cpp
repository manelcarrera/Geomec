#include "ParameterModifierShiftFactor.h"

namespace cora
{

CParameterModifierShiftFactor::CParameterModifierShiftFactor(
  std::vector <double>& value)
: CParameterModifierBase()
, m_value2Shift(assignFirstValue(value))
, m_factor(assignSecondValue(value))
{
}

CParameterModifierShiftFactor::~CParameterModifierShiftFactor()
{
}

geo::CValue CParameterModifierShiftFactor::modify(const geo::CValue& value)
  const
{
  geo::CValue newValue = (value + m_value2Shift) * m_factor;

  return newValue;
}

} // namespace cora
