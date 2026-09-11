#ifndef _cora_Pre20ResponseTypeFailureAreaPercMax_h_
#define _cora_Pre20ResponseTypeFailureAreaPercMax_h_

#include "Pre.2.0.ResponseTypeBaseWithValue.h"

namespace cora {

class CPre20ResponseTypeFailureAreaPercMax : public CPre20ResponseTypeBaseWithValue {
public:
  CPre20ResponseTypeFailureAreaPercMax(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);
  virtual ~CPre20ResponseTypeFailureAreaPercMax();

  virtual double calculate(const TObject &object, const TFailureMode &failureMode);

private:
  CPre20ResponseTypeFailureAreaPercMax(const CPre20ResponseTypeFailureAreaPercMax &rhs);
  CPre20ResponseTypeFailureAreaPercMax &operator=(CPre20ResponseTypeFailureAreaPercMax rhs);
};

} // namespace cora

#endif // _cora_Pre20ResponseTypeFailureAreaPercMax_h_
