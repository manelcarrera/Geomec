#ifndef _cora_ResponseTypeFailureAreaMin_h_
#define _cora_ResponseTypeFailureAreaMin_h_

#include "ResponseTypeBaseWithValue.h"

namespace cora
{

class CResponseTypeFailureAreaMin : public CResponseTypeBaseWithValue
{
  public:
  CResponseTypeFailureAreaMin(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CResponseTypeFailureAreaMin();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CResponseTypeFailureAreaMin(const CResponseTypeFailureAreaMin& rhs);
  CResponseTypeFailureAreaMin& operator = (CResponseTypeFailureAreaMin rhs);
};

} // namespace cora

#endif  // _cora_ResponseTypeFailureAreaMin_h_
