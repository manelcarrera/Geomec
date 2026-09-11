#ifndef _cora_LimitStateFunctionSurface_h_
#define _cora_LimitStateFunctionSurface_h_

#include "LimitStateFunctionBase.h"

namespace cora
{

class CLimitStateFunctionSurface : public CLimitStateFunctionBase
{
  public:
  CLimitStateFunctionSurface(CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber);
  virtual ~CLimitStateFunctionSurface();

  private:
  CLimitStateFunctionSurface(const CLimitStateFunctionSurface& rhs);
  CLimitStateFunctionSurface& operator = (CLimitStateFunctionSurface rhs);
};

} // namespace cora

#endif  // _cora_LimitStateFunctionSurface_h_
