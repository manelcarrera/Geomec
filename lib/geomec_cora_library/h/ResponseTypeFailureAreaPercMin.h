#ifndef _cora_ResponseTypeFailureAreaPercMin_h_
#define _cora_ResponseTypeFailureAreaPercMin_h_

#include "ResponseTypeBaseWithValue.h"

namespace cora
{

class CResponseTypeFailureAreaPercMin : public CResponseTypeBaseWithValue
{
  public:
  CResponseTypeFailureAreaPercMin(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CResponseTypeFailureAreaPercMin();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CResponseTypeFailureAreaPercMin(const CResponseTypeFailureAreaPercMin& rhs);
  CResponseTypeFailureAreaPercMin& operator = (
      CResponseTypeFailureAreaPercMin rhs);
};

} // namespace cora

#endif  // _cora_ResponseTypeFailureAreaPercMin_h_
