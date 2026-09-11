#ifndef _cora_LimitStateFunctionBase_h_
#define _cora_LimitStateFunctionBase_h_

#include <vector>

#include "safeQSharedPointer.h"

namespace cora
{

class CSummaryResultFile;
class CGetModelInfo;
class CResponseParameterFile;

} // namespace cora

class IResult;

#include "Object.h"
#include "GetFailureModeInfo.h"
#include "ResponseType.h"

namespace cora
{

class CLimitStateFunctionBase
{
  public:
  CLimitStateFunctionBase(CSummaryResultFile& summaryResultFile,
      std::vector <QString>& function, CGetModelInfo& modelInfo,
      const QString& versionNumber, const TObjects& objects,
      const QString& objectType);
  virtual ~CLimitStateFunctionBase() = 0;

  void calculate(CResponseParameterFile& responseParameterFile);

  int getDepletionStage() const;
  CAnalysisType::TAnalysisType getAnalysisType() const;

  protected:
  bool m_validFunctionSize; // dummy for validation purposes
  TObject m_object;
  TResponseType m_responseType;
  CSummaryResultFile& m_summaryResultFile;
  CGetModelInfo& m_modelInfo;
  int m_depletionStage;
  const QString m_failureMode;
  const QString& m_objectType;
  CAnalysisType::TAnalysisType m_analysisType;

  private:
  CLimitStateFunctionBase(const CLimitStateFunctionBase& rhs);
  CLimitStateFunctionBase& operator = (const CLimitStateFunctionBase& rhs);

  static TObject findObject(CSummaryResultFile& summaryResultFile,
      CGetModelInfo& modelInfo, const QString& object,
      const TObjects& objects, const QString& objectType);
  static TFailureMode findFailureMode(CSummaryResultFile& summaryResultFile,
      CGetModelInfo& modelInfo, const QString& failureModeString,
      TObject object, const QString& objectType);
  static CAnalysisType::TAnalysisType selectAnalysisType(
      const QString& failureMode);
  static int captureDepletionStage(const QString& failureMode);

  typedef std::map <char, CAnalysisType::TAnalysisType> TAnalysisTypes;

  static std::vector <QString> m_failureModeIsNotAvailable;
  static TAnalysisTypes m_analysisTypes;
};

typedef QSharedPointer <CLimitStateFunctionBase> TLimitStateFunctionBase;

} // namespace cora

#endif  // _cora_LimitStateFunctionBase_h_
