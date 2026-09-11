#ifndef _cora_ParameterModifierBase_h_
#define _cora_ParameterModifierBase_h_

#include <vector>

#include "safeQSharedPointer.h"

namespace geo {

class CValue;

} // namespace geo

namespace cora {

class CParameterModifierBase {
public:
  typedef std::vector<geo::CValue> TValueVec;

  CParameterModifierBase();
  virtual ~CParameterModifierBase() = 0;

  virtual geo::CValue modify(const geo::CValue &value) const = 0;
  TValueVec modify(TValueVec &valueVec);

protected:
  static double assignSingleValue(std::vector<double> &value);

  static double assignFirstValue(std::vector<double> &value);
  static double assignSecondValue(std::vector<double> &value);

private:
  CParameterModifierBase(const CParameterModifierBase &rhs);
  CParameterModifierBase &operator=(const CParameterModifierBase &rhs);
};

typedef QSharedPointer<CParameterModifierBase> TParameterModifierBase;

} // namespace cora

#endif // _cora_ParameterModifierBase_h_
