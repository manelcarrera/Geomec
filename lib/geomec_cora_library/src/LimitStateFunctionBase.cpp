#include <numeric>

#include "LimitStateFunctionBase.h"
#include "SummaryResultFile.h"
#include "GetModelInfo.h"
#include "IncompleteLimitStateFunction.h"
#include "FailureModePatternMismatch.h"
#include "ResponseTypeBaseValidate.h"

namespace cora
{

namespace
{

const size_t LSF_LENGTH_WITHOUT_VALUE = 4;

}

CLimitStateFunctionBase::CLimitStateFunctionBase(
  CSummaryResultFile& summaryResultFile, std::vector <QString>& function,
  CGetModelInfo& modelInfo, const QString& versionNumber,
  const TObjects& objects, const QString& objectType)
: m_validFunctionSize(CResponseTypeBaseValidate::validateFunctionSize(
    summaryResultFile, function, LSF_LENGTH_WITHOUT_VALUE))
, m_object(findObject(summaryResultFile, modelInfo, function[2], objects,
    objectType))
, m_responseType(TResponseType(
    new CResponseType(summaryResultFile, versionNumber, function)))
, m_summaryResultFile(summaryResultFile)
, m_modelInfo(modelInfo)
, m_depletionStage(captureDepletionStage(function[1]))
, m_failureMode(function[1])
, m_objectType(objectType)
, m_analysisType(selectAnalysisType(m_failureMode))
{
}

CLimitStateFunctionBase::~CLimitStateFunctionBase()
{
}

namespace
{

const QString RESULT_NOT_AVAILABLE =
  "%1 result is not available for failure mode '%2'";

} // anonymous namespace

void CLimitStateFunctionBase::calculate(
  CResponseParameterFile& responseParameterFile)
{
  if (m_object != 0)
  {
    TFailureMode failureMode = findFailureMode(m_summaryResultFile, m_modelInfo,
      m_failureMode, m_object, m_objectType);

    if (failureMode->getResultComponent() != 0)
    {
      if (failureMode->getResultComponent()->ResultRegister().ResultsAvailable(
        failureMode->getResultComponent()->AnalysisType()))
      {
        m_responseType->calculate(responseParameterFile, m_object, failureMode);
      }
      else
      {
        if (std::find(m_failureModeIsNotAvailable.begin(),
          m_failureModeIsNotAvailable.end(), m_failureMode) ==
            m_failureModeIsNotAvailable.end())
        {
          m_summaryResultFile.setResultValue(
            CSummaryResultFile::RESULT_VALUE_INCONSISTENT);

          QString message = QString(RESULT_NOT_AVAILABLE).
            arg(failureMode->getResultComponent()->AnalysisType().Label()).
            arg(m_failureMode);

          m_summaryResultFile.addAdditionalInformation(message);
          m_failureModeIsNotAvailable.push_back(m_failureMode);
        }
      }
    }
  }
}

int CLimitStateFunctionBase::getDepletionStage() const
{
  return m_depletionStage;
}

CAnalysisType::TAnalysisType CLimitStateFunctionBase::getAnalysisType() const
{
  return m_analysisType;
}

// private

namespace
{

const QString OBJECT_DOES_NOT_EXIST = "%1 '%2' does not exist";
const QString SEPARATOR = ":";

} // anonymous namespace

// static

TObject CLimitStateFunctionBase::findObject(
  CSummaryResultFile& summaryResultFile, CGetModelInfo& /*modelInfo*/,
  const QString& object, const TObjects& objects, const QString& objectType)
{
  for (TObjects::const_iterator o = objects.begin(); o != objects.end(); ++o)
  {
    if (((*o)->name() == object) ||
      ((objectType + SEPARATOR + (*o)->name()) == object))
    {
      return (*o);
    }
  }

  summaryResultFile.setResultValue(
    CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  summaryResultFile.addAdditionalInformation(QString(OBJECT_DOES_NOT_EXIST).
    arg(objectType).arg(object));

  return TObject();
}

namespace
{

const QString FAILURE_MODE_DOES_NOT_EXIST = "failure mode '%1' does not exist";
const QString FAILURE_DOES_NOT_MAP =
  "failure mode '%1' does not map to %2 '%3%4'";

struct TCompareWithFailureModeString
{
  TCompareWithFailureModeString(const QString& failureModeString)
    : m_failureModeString(failureModeString)
  {
  }

  bool operator () (const TFailureMode& result)
  {
    return result->getResultExportLabel() == m_failureModeString;
  }

private:
  const QString& m_failureModeString;
};

} // anonymous namespace

TFailureMode CLimitStateFunctionBase::findFailureMode(
  CSummaryResultFile& summaryResultFile, CGetModelInfo& modelInfo,
  const QString& failureModeString, TObject object, const QString& objectType)
{
  CGetFailureModeInfo& getFailureModeInfo = modelInfo.getFailureModeInfo();
  TFailureModes failureModes = getFailureModeInfo.getFailureModes();

  /*
    Too bad, the C++11 lambda expression is not yet supported on all platforms

    std::vector <CGetFailureModeInfo::TFailureMode>::const_iterator
      failureMode = std::find_if(failureModes.begin(), failureModes.end(),
        [&failureModeString] (const CGetFailureModeInfo::TFailureMode& result) {
          return result.first == failureModeString; });
  */

  TFailureModes::const_iterator failureMode = std::find_if(failureModes.begin(),
    failureModes.end(), TCompareWithFailureModeString(failureModeString));

  if (failureMode == failureModes.end())
  {
    summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    summaryResultFile.addAdditionalInformation(
      QString(FAILURE_MODE_DOES_NOT_EXIST).arg(failureModeString));
  }
  else if (!(*failureMode)->getResultComponent()->CanMap(*(object->object())))
  {
    summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    summaryResultFile.addAdditionalInformation(
      QString(FAILURE_DOES_NOT_MAP).arg(failureModeString).arg(objectType).
      arg(object->prefix()).arg(object->name()));

    failureMode = failureModes.end();
  }

  return (failureMode == failureModes.end() ? TFailureMode() : *failureMode);
}

CAnalysisType::TAnalysisType CLimitStateFunctionBase::selectAnalysisType(
  const QString& failureMode)
{
  if (m_analysisTypes.empty())
  {
    for (CAnalysisType analysisType = CAnalysisType::FirstType();
      analysisType.Valid(); analysisType = analysisType.NextType())
    {
      m_analysisTypes.insert(TAnalysisTypes::value_type(
        analysisType.ExportCharacter(), analysisType.AnalysisType()));
    }
  }

  assert(m_analysisTypes.find(failureMode[failureMode.length() - 1].toLatin1()) != m_analysisTypes.end());

  return m_analysisTypes[failureMode[failureMode.length() - 1].toLatin1()];
}

namespace
{

const QString PATTERN = ".*_D([0-9]*)_.$";

} // anonymous namespace

int CLimitStateFunctionBase::captureDepletionStage(const QString& failureMode)
{
  QRegExp pattern(PATTERN);

  if ((pattern.indexIn(failureMode) < 0) || (pattern.captureCount() != 1))
  {
    throw CFailureModePatternMismatch(failureMode, PATTERN);
  }

  return pattern.cap(1).toInt();
}

std::vector <QString> CLimitStateFunctionBase::m_failureModeIsNotAvailable;
CLimitStateFunctionBase::TAnalysisTypes
  CLimitStateFunctionBase::m_analysisTypes;

} // namespace cora
