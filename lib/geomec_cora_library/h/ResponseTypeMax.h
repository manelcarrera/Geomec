#ifndef _cora_ResponseTypeMax_h_
#define _cora_ResponseTypeMax_h_

#include "ResponseTypeBase.h"

namespace cora
{

class CResponseTypeMax : public CResponseTypeBase
{
  public:
    CResponseTypeMax(CSummaryResultFile& summaryResultFile,
      const std::vector <QString>& function);
    virtual ~CResponseTypeMax();

    virtual double calculate(const TObject& object,
      const TFailureMode& failureMode);

  private:
    CResponseTypeMax(const CResponseTypeMax& rhs);
    CResponseTypeMax& operator = (CResponseTypeMax rhs);
};

} // namespace cora

#endif  // _cora_ResponseTypeMax_h_
