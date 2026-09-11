#ifndef _cora_UndefinedOrInvalidValueVector_h_
#define _cora_UndefinedOrInvalidValueVector_h_

#include <stdexcept>

#include <QString>

namespace cora
{

class CUndefinedOrInvalidValueVector : public std::runtime_error
{
  public:
    CUndefinedOrInvalidValueVector(const QString& prefix);
};

} // namespace cora

#endif  // _cora_UndefinedOrInvalidValueVector_h_
