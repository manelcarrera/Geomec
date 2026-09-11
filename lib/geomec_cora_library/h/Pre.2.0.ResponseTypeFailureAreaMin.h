#ifndef _cora_Pre20ResponseTypeFailureAreaMin_h_
#define _cora_Pre20ResponseTypeFailureAreaMin_h_

#include "Pre.2.0.ResponseTypeBaseWithValue.h"

namespace cora
{

class CPre20ResponseTypeFailureAreaMin : public CPre20ResponseTypeBaseWithValue
{
  public:
  CPre20ResponseTypeFailureAreaMin(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CPre20ResponseTypeFailureAreaMin();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CPre20ResponseTypeFailureAreaMin(
      const CPre20ResponseTypeFailureAreaMin& rhs);
  CPre20ResponseTypeFailureAreaMin& operator = (
      CPre20ResponseTypeFailureAreaMin rhs);
};

} // namespace cora

#endif  // _cora_Pre20ResponseTypeFailureAreaMin_h_
