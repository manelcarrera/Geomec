#ifndef _cora_ResponseTypefn_h_
#define _cora_ResponseTypefn_h_

#include "ResponseTypeBase.h"

namespace cora {

class CResponseTypefn : public CResponseTypeBase {
public:
  CResponseTypefn(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);
  virtual ~CResponseTypefn();

  virtual double calculate(const TObject &object, const TFailureMode &failureMode);

private:
  CResponseTypefn(const CResponseTypefn &rhs);
  CResponseTypefn &operator=(CResponseTypefn rhs);
};

} // namespace cora

#endif // _cora_ResponseTypefn_h_
