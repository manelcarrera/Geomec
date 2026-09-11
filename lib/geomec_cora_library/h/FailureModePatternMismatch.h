#ifndef _cora_FailureModePatternMismatch_h_
#define _cora_FailureModePatternMismatch_h_

#include <stdexcept>

#include <QString>

namespace cora
{

class CFailureModePatternMismatch : public std::runtime_error
{
  public:
  CFailureModePatternMismatch(const QString& failureMode,
      const QString& pattern);
};

} // namespace cora

#endif  // _cora_FailureModePatternMismatch_h_
