#ifndef _cora_LimitStateFunctionWell_h_
#define _cora_LimitStateFunctionWell_h_

#include "LimitStateFunctionBase.h"

namespace cora {

class CLimitStateFunctionWell : public CLimitStateFunctionBase {
public:
  CLimitStateFunctionWell(CSummaryResultFile &summaryResultFile, std::vector<QString> &function,
                          CGetModelInfo &modelInfo, const QString &versionNumber);
  virtual ~CLimitStateFunctionWell();

private:
  CLimitStateFunctionWell(const CLimitStateFunctionWell &rhs);
  CLimitStateFunctionWell &operator=(CLimitStateFunctionWell rhs);
};

} // namespace cora

#endif // _cora_LimitStateFunctionWell_h_
