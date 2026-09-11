#include "ParameterModifierFactor.h"

namespace cora
{

CParameterModifierFactor::CParameterModifierFactor(
  std::vector <double>& factor)
: CParameterModifierBase()
, m_factor(assignSingleValue(factor))
{
}

CParameterModifierFactor::~CParameterModifierFactor()
{
}

geo::CValue CParameterModifierFactor::modify(const geo::CValue& value) const
{
  geo::CValue newValue = m_factor * value;

  return newValue;
}

} // namespace cora
