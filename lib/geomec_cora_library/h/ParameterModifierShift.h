#ifndef _cora_ParameterModifierShift_h_
#define _cora_ParameterModifierShift_h_

namespace geo
{

class CValue;

} // namespace geo

#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

class CParameterModifierShift : public CParameterModifierBase
{
  public:
  CParameterModifierShift(std::vector <double>& value2Shift);
  virtual ~CParameterModifierShift();

  virtual geo::CValue modify(const geo::CValue& value) const;

  private:
  CParameterModifierShift(const CParameterModifierShift& rhs);
  CParameterModifierShift& operator = (CParameterModifierShift rhs);

  const geo::CValue m_value2Shift;
};

} // namespace cora

#endif  // _cora_ParameterModifierShift_h_
