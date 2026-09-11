#ifndef _cora_Pre20ResponseTypeFailureAreaPercMin_h_
#define _cora_Pre20ResponseTypeFailureAreaPercMin_h_

#include "Pre.2.0.ResponseTypeBaseWithValue.h"

namespace cora
{

class CPre20ResponseTypeFailureAreaPercMin :
  public CPre20ResponseTypeBaseWithValue
{
  public:
  CPre20ResponseTypeFailureAreaPercMin(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
  virtual ~CPre20ResponseTypeFailureAreaPercMin();

  virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
  CPre20ResponseTypeFailureAreaPercMin(
      const CPre20ResponseTypeFailureAreaPercMin& rhs);
  CPre20ResponseTypeFailureAreaPercMin& operator = (
      CPre20ResponseTypeFailureAreaPercMin rhs);
};

} // namespace cora

#endif  // _cora_Pre20ResponseTypeFailureAreaPercMin_h_
