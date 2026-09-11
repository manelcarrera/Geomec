#ifndef _cora_LimitStateFunctionFormation_h_
#define _cora_LimitStateFunctionFormation_h_

#include "LimitStateFunctionBase.h"

namespace cora
{

class CLimitStateFunctionFormation : public CLimitStateFunctionBase
{
  public:
  CLimitStateFunctionFormation(CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber);
  virtual ~CLimitStateFunctionFormation();

  private:
  CLimitStateFunctionFormation(const CLimitStateFunctionFormation& rhs);
  CLimitStateFunctionFormation& operator = (CLimitStateFunctionFormation rhs);
};

} // namespace cora

#endif  // _cora_LimitStateFunctionFormation_h_
