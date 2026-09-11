#ifndef _cora_ParameterModifierShiftFactor_h_
#define _cora_ParameterModifierShiftFactor_h_

namespace geo
{

class CValue;

} // namespace geo

#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

class CParameterModifierShiftFactor : public CParameterModifierBase
{
  public:
    CParameterModifierShiftFactor(std::vector <double>& value);
    virtual ~CParameterModifierShiftFactor();

    virtual geo::CValue modify(const geo::CValue& value) const;

  private:
    CParameterModifierShiftFactor(
      const CParameterModifierShiftFactor& rhs);
    CParameterModifierShiftFactor& operator = (
      CParameterModifierShiftFactor rhs);

    const geo::CValue m_value2Shift;
    const geo::CValue m_factor;
};

} // namespace cora

#endif  // _cora_ParameterModifierShiftFactor_h_
