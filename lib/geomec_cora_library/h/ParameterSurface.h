#ifndef _cora_ParameterSurface_h_
#define _cora_ParameterSurface_h_

#include "ParameterLimit.h"

namespace cora
{

class CParameterSurface : public CParameterLimit
{
  public:
    CParameterSurface();
    virtual ~CParameterSurface();

  private:
    CParameterSurface(const CParameterSurface& rhs);
    CParameterSurface& operator = (const CParameterSurface& rhs);
};

} // namespace cora

#endif  // _cora_ParameterSurface_h_
