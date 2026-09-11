#ifndef _cora_ResponseType_h_
#define _cora_ResponseType_h_

namespace cora {

class CResponseParameterFile;

} // namespace cora

#include "ResponseTypeBase.h"

namespace cora {

class CResponseType {
public:
  CResponseType(CSummaryResultFile &summaryResultFile, const QString &versionNumber,
                const std::vector<QString> &function);

  void calculate(CResponseParameterFile &responseParameterFile, const TObject &object, const TFailureMode &failureMode);

private:
  CResponseType(const CResponseType &rhs);
  CResponseType &operator=(CResponseType rhs);

  static TResponseTypeBase selectResponseType(CSummaryResultFile &summaryResultFile, const QString &versionNumber,
                                              const std::vector<QString> &function);

  TResponseTypeBase m_responseTypeBase;
  CSummaryResultFile &m_summaryResultFile;
};

typedef QSharedPointer<CResponseType> TResponseType;

} // namespace cora

#endif // _cora_ResponseType_h_
