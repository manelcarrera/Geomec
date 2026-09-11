#ifndef _cora_ResponseTypeBaseWithValue_h_
#define _cora_ResponseTypeBaseWithValue_h_

#include "ResponseTypeBaseValidate.h"

namespace cora {

class CResponseTypeBaseWithValue : public CResponseTypeBaseValidate {
public:
  CResponseTypeBaseWithValue(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function);

  virtual const QString getResponseType() const;

protected:
  double m_value;
  bool m_tooHigh;
  bool m_tooLow;

private:
  CResponseTypeBaseWithValue(const CResponseTypeBaseWithValue &rhs);
  CResponseTypeBaseWithValue &operator=(const CResponseTypeBaseWithValue &rhs);

  static bool extractInternalFailsWhen(CSummaryResultFile &summaryResultFile, const std::vector<QString> &function,
                                       const QString &internalFailsWhen);
};

} // namespace cora

#endif // _cora_ResponseTypeBaseWithValue_h_
