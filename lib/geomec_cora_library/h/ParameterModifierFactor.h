#ifndef _cora_ParameterModifierFactor_h_
#define _cora_ParameterModifierFactor_h_

namespace geo
{

class CValue;

} // namespace geo

#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

class CParameterModifierFactor : public CParameterModifierBase
{
  public:
    CParameterModifierFactor(std::vector <double>& factor);
    virtual ~CParameterModifierFactor();

    virtual geo::CValue modify(const geo::CValue& value) const;

  private:
    CParameterModifierFactor(const CParameterModifierFactor& rhs);
    CParameterModifierFactor& operator = (CParameterModifierFactor rhs);

    const geo::CValue m_factor;
};

} // namespace cora

#endif  // _cora_ParameterModifierFactor_h_
