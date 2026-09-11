#ifndef _cora_ResponseTypeFailureAreaMax_h_
#define _cora_ResponseTypeFailureAreaMax_h_

#include "ResponseTypeBaseWithValue.h"

namespace cora
{

class CResponseTypeFailureAreaMax : public CResponseTypeBaseWithValue
{
  public:
  CResponseTypeFailureAreaMax(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CResponseTypeFailureAreaMax();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CResponseTypeFailureAreaMax(const CResponseTypeFailureAreaMax& rhs);
  CResponseTypeFailureAreaMax& operator = (CResponseTypeFailureAreaMax rhs);
};

} // namespace cora

#endif  // _cora_ResponseTypeFailureAreaMax_h_
