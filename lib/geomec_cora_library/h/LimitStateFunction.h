#ifndef _cora_LimitStateFunction_h_
#define _cora_LimitStateFunction_h_

#include <vector>
#include <string>

#include "safeQSharedPointer.h"

namespace cora
{

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "LimitStateFunctionBase.h"

namespace cora
{

class CLimitStateFunction
{
  public:
  CLimitStateFunction(CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber);

  void calculate(CResponseParameterFile& responseParameterFile);

  int getDepletionStage() const;
  CAnalysisType::TAnalysisType getAnalysisType() const;

  private:
  CLimitStateFunction(const CLimitStateFunction& rhs);
  CLimitStateFunction& operator = (CLimitStateFunction rhs);

  static TLimitStateFunctionBase selectLimitStateFunction(
      CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber);

  TLimitStateFunctionBase m_limitStateFunctionBase;
};

typedef QSharedPointer <CLimitStateFunction> TLimitStateFunction;

} // namespace cora

#endif  // _cora_LimitStateFunction_h_
