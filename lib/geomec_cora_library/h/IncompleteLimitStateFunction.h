#ifndef _cora_IncompleteLimitStateFunction_h_
#define _cora_IncompleteLimitStateFunction_h_

#include <stdexcept>

namespace cora
{

class CIncompleteLimitStateFunction : public std::runtime_error
{
  public:
  CIncompleteLimitStateFunction(const std::string& what);
};

} // namespace cora

#endif  // _cora_IncompleteLimitStateFunction_h_
