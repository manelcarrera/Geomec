#include "ParameterModifierAbsolute.h"

namespace cora
{

CParameterModifierAbsolute::CParameterModifierAbsolute(
  std::vector <double>& absoluteValue)
: CParameterModifierBase()
, m_absoluteValue(assignSingleValue(absoluteValue))
{
}

CParameterModifierAbsolute::~CParameterModifierAbsolute()
{
}

geo::CValue CParameterModifierAbsolute::modify(const geo::CValue& /*value*/)
  const
{
  geo::CValue newValue = m_absoluteValue;

  return newValue;
}

} // namespace cora
