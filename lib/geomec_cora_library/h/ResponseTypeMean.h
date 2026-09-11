#ifndef _cora_ResponseTypeMean_h_
#define _cora_ResponseTypeMean_h_

#include "ResponseTypeBase.h"

namespace cora {

class CResponseTypeMean : public CResponseTypeBase {
public:
  CResponseTypeMean(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);
  virtual ~CResponseTypeMean();

  virtual double calculate(const TObject &object, const TFailureMode &failureMode);

private:
  CResponseTypeMean(const CResponseTypeMean &rhs);
  CResponseTypeMean &operator=(CResponseTypeMean rhs);
};

} // namespace cora

#endif // _cora_ResponseTypeMean_h_
