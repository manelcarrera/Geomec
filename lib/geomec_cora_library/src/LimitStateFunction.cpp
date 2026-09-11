#include "LimitStateFunction.h"
#include "LimitStateFunctionFault.h"
#include "LimitStateFunctionFormation.h"
#include "LimitStateFunctionSurface.h"
#include "LimitStateFunctionWell.h"
#include "SummaryResultFile.h"
#include "IncompleteLimitStateFunction.h"

namespace cora
{

CLimitStateFunction::CLimitStateFunction(CSummaryResultFile& summaryResultFile,
  std::vector <QString>& function, CGetModelInfo& modelInfo,
  const QString& versionNumber)
: m_limitStateFunctionBase(selectLimitStateFunction(summaryResultFile, function,
  modelInfo, versionNumber))
{
}

void CLimitStateFunction::calculate(
  CResponseParameterFile& responseParameterFile)
{
  if (m_limitStateFunctionBase != 0)
  {
  m_limitStateFunctionBase->calculate(responseParameterFile);
  }
}

int CLimitStateFunction::getDepletionStage() const
{
  return m_limitStateFunctionBase->getDepletionStage();
}

CAnalysisType::TAnalysisType CLimitStateFunction::getAnalysisType() const
{
  return m_limitStateFunctionBase->getAnalysisType();
}

// private

namespace
{

const QString FAILURE_TYPE_FAULT = "Fault";
const QString FAILURE_TYPE_FORMATION = "Formation";
const QString FAILURE_TYPE_SURFACE = "Surface";
const QString FAILURE_TYPE_WELL = "Well";

const QString FAILURE_TYPE_DOES_NOT_EXIST = "failure type '%1' does not exist";

} // anonymous namespace

// static

TLimitStateFunctionBase CLimitStateFunction::selectLimitStateFunction(
  CSummaryResultFile& summaryResultFile, std::vector <QString>& function,
  CGetModelInfo& modelInfo, const QString& versionNumber)
{
  try
  {
  if (function[0] == FAILURE_TYPE_FAULT)
  {
      return TLimitStateFunctionBase(new CLimitStateFunctionFault(
    summaryResultFile, function, modelInfo, versionNumber));
  }
  else if (function[0] == FAILURE_TYPE_FORMATION)
  {
      return TLimitStateFunctionBase(new CLimitStateFunctionFormation(
    summaryResultFile, function, modelInfo, versionNumber));
  }
  else if (function[0] == FAILURE_TYPE_SURFACE)
  {
      return TLimitStateFunctionBase(new CLimitStateFunctionSurface(
    summaryResultFile, function, modelInfo, versionNumber));
  }
  else if (function[0] == FAILURE_TYPE_WELL)
  {
      return TLimitStateFunctionBase(new CLimitStateFunctionWell(
    summaryResultFile, function, modelInfo, versionNumber));
  }

  summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  summaryResultFile.addAdditionalInformation(
      QString(FAILURE_TYPE_DOES_NOT_EXIST).arg(function[0]));
  }

  catch (const CIncompleteLimitStateFunction&)
  {
  }

  return TLimitStateFunctionBase();
}

} // namespace cora
