#include "ParameterModifierShift.h"

namespace cora {

CParameterModifierShift::CParameterModifierShift(std::vector<double> &value2Shift)
    : CParameterModifierBase(), m_value2Shift(assignSingleValue(value2Shift)) {}

CParameterModifierShift::~CParameterModifierShift() {}

geo::CValue CParameterModifierShift::modify(const geo::CValue &value) const {
  geo::CValue newValue = value + m_value2Shift;

  return newValue;
}

} // namespace cora
