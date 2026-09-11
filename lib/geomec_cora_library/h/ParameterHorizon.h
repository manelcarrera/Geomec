#ifndef _cora_ParameterHorizon_h_
#define _cora_ParameterHorizon_h_

#include "ParameterLimit.h"

namespace cora {

class CParameterHorizon : public CParameterLimit {
public:
  CParameterHorizon();
  virtual ~CParameterHorizon();

private:
  CParameterHorizon(const CParameterHorizon &rhs);
  CParameterHorizon &operator=(const CParameterHorizon &rhs);
};

} // namespace cora

#endif // _cora_ParameterHorizon_h_
