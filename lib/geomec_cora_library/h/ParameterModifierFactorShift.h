#ifndef _cora_ParameterModifierFactorShift_h_
#define _cora_ParameterModifierFactorShift_h_

namespace geo
{

class CValue;

} // namespace geo

#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

class CParameterModifierFactorShift : public CParameterModifierBase
{
  public:
    CParameterModifierFactorShift(std::vector <double>& value);
    virtual ~CParameterModifierFactorShift();

    virtual geo::CValue modify(const geo::CValue& value) const;

  private:
    CParameterModifierFactorShift(
      const CParameterModifierFactorShift& rhs);
    CParameterModifierFactorShift& operator = (
      CParameterModifierFactorShift rhs);

    const geo::CValue m_factor;
    const geo::CValue m_value2Shift;
};

} // namespace cora

#endif  // _cora_ParameterModifierFactorShift_h_
