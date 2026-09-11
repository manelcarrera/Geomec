#ifndef _cora_ResponseTypeBaseValidate_h_
#define _cora_ResponseTypeBaseValidate_h_

#include "ResponseTypeBase.h"

namespace cora {

class CResponseTypeBaseValidate : public CResponseTypeBase {
public:
  static bool validateFunctionSize(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function,
                                   size_t validFunctionSize);

protected:
  CResponseTypeBaseValidate(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);

  static QString concatenateFunction(const std::vector<QString> &function);
  static double extractValue(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function,
                             size_t validFunctionSize, size_t valuePosition);

private:
  CResponseTypeBaseValidate(const CResponseTypeBaseValidate &rhs);
  CResponseTypeBaseValidate &operator=(const CResponseTypeBaseValidate &rhs);
};

} // namespace cora

#endif // _cora_ResponseTypeBaseValidate_h_
