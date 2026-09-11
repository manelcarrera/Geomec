#ifndef _cora_Pre20ResponseTypeFailureAreaMax_h_
#define _cora_Pre20ResponseTypeFailureAreaMax_h_

#include "Pre.2.0.ResponseTypeBaseWithValue.h"

namespace cora
{

class CPre20ResponseTypeFailureAreaMax : public CPre20ResponseTypeBaseWithValue
{
  public:
  CPre20ResponseTypeFailureAreaMax(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CPre20ResponseTypeFailureAreaMax();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CPre20ResponseTypeFailureAreaMax(
      const CPre20ResponseTypeFailureAreaMax& rhs);
  CPre20ResponseTypeFailureAreaMax& operator = (
      CPre20ResponseTypeFailureAreaMax rhs);
};

} // namespace cora

#endif  // _cora_Pre20ResponseTypeFailureAreaMax_h_
