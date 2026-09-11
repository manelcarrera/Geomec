#ifndef _cora_ResponseTypeFailureAreaPercMax_h_
#define _cora_ResponseTypeFailureAreaPercMax_h_

#include "ResponseTypeBaseWithValue.h"

namespace cora {

class CResponseTypeFailureAreaPercMax : public CResponseTypeBaseWithValue {
public:
  CResponseTypeFailureAreaPercMax(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);
  virtual ~CResponseTypeFailureAreaPercMax();

  virtual double calculate(const TObject &object, const TFailureMode &failureMode);

private:
  CResponseTypeFailureAreaPercMax(const CResponseTypeFailureAreaPercMax &rhs);
  CResponseTypeFailureAreaPercMax &operator=(CResponseTypeFailureAreaPercMax rhs);
};

} // namespace cora

#endif // _cora_ResponseTypeFailureAreaPercMax_h_
