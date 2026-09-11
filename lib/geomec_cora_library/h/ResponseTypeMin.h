#ifndef _cora_ResponseTypeMin_h_
#define _cora_ResponseTypeMin_h_

#include "ResponseTypeBase.h"

namespace cora
{

class CResponseTypeMin : public CResponseTypeBase
{
  public:
  CResponseTypeMin(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CResponseTypeMin();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CResponseTypeMin(const CResponseTypeMin& rhs);
  CResponseTypeMin& operator = (CResponseTypeMin rhs);
};

} // namespace cora

#endif  // _cora_ResponseTypeMin_h_
