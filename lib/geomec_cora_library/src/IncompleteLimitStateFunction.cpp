#include "IncompleteLimitStateFunction.h"

namespace cora
{

CIncompleteLimitStateFunction::CIncompleteLimitStateFunction(
  const std::string& what)
: std::runtime_error(what)
{
}

} // namespace cora
