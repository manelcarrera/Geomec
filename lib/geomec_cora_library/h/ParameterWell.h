#ifndef _cora_ParameterWell_h_
#define _cora_ParameterWell_h_

#include "ParameterLimit.h"

namespace cora
{

class CParameterWell : public CParameterLimit
{
  public:
    CParameterWell();
    virtual ~CParameterWell();

  private:
    CParameterWell(const CParameterWell& rhs);
    CParameterWell& operator = (const CParameterWell& rhs);
};

} // namespace cora

#endif  // _cora_ParameterWell_h_
