#ifndef _cora_LimitStateFunctionFault_h_
#define _cora_LimitStateFunctionFault_h_

#include "LimitStateFunctionBase.h"

namespace cora
{

class CLimitStateFunctionFault : public CLimitStateFunctionBase
{
  public:
  CLimitStateFunctionFault(CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber);
  virtual ~CLimitStateFunctionFault();

  private:
  CLimitStateFunctionFault(const CLimitStateFunctionFault& rhs);
  CLimitStateFunctionFault& operator = (CLimitStateFunctionFault rhs);
};

} // namespace cora

#endif  // _cora_LimitStateFunctionFault_h_
