#include "ParameterModifierFactorShift.h"

namespace cora {

CParameterModifierFactorShift::CParameterModifierFactorShift(std::vector<double> &value)
    : CParameterModifierBase(), m_factor(assignFirstValue(value)), m_value2Shift(assignSecondValue(value)) {}

CParameterModifierFactorShift::~CParameterModifierFactorShift() {}

geo::CValue CParameterModifierFactorShift::modify(const geo::CValue &value) const {
  geo::CValue newValue = (m_factor * value) + m_value2Shift;

  return newValue;
}

} // namespace cora
