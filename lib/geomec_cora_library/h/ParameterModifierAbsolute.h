#ifndef _cora_ParameterModifierAbsolute_h_
#define _cora_ParameterModifierAbsolute_h_

namespace geo
{

class CValue;

} // namespace geo

#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

class CParameterModifierAbsolute : public CParameterModifierBase
{
  public:
    CParameterModifierAbsolute(std::vector <double>& absoluteValue);
    virtual ~CParameterModifierAbsolute();

    virtual geo::CValue modify(const geo::CValue& value) const;

  private:
    CParameterModifierAbsolute(const CParameterModifierAbsolute& rhs);
    CParameterModifierAbsolute& operator = (CParameterModifierAbsolute rhs);

    const geo::CValue m_absoluteValue;
};

} // namespace cora

#endif  // _cora_ParameterModifierAbsolute_h_
