#ifndef _cora_FailureTypeParameterSurface_h_
#define _cora_FailureTypeParameterSurface_h_

namespace cora {

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "FailureTypeParameterBase.h"

namespace cora {

class CFailureTypeParameterSurface : public CFailureTypeParameterBase {
public:
  CFailureTypeParameterSurface(CSummaryResultFile &summaryResultFile, const std::string &object,
                               const std::string &option, const std::string &parameter, std::vector<double> &value,
                               const CGetModelInfo &modelInfo);
  virtual ~CFailureTypeParameterSurface();

private:
  CFailureTypeParameterSurface(const CFailureTypeParameterSurface &rhs);
  CFailureTypeParameterSurface &operator=(CFailureTypeParameterSurface rhs);
};

} // namespace cora

#endif // _cora_FailureTypeParameterSurface_h_
