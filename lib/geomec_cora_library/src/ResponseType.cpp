#include "ResponseType.h"
#include "ResponseTypeMax.h"
#include "ResponseTypeMin.h"
#include "ResponseTypeMean.h"
#include "ResponseTypeFailureAreaMax.h"
#include "ResponseTypeFailureAreaMin.h"
#include "ResponseTypeFailureAreaPercMax.h"
#include "ResponseTypeFailureAreaPercMin.h"
#include "SummaryResultFile.h"
#include "IncompleteLimitStateFunction.h"
#include "ResponseParameterFile.h"
#include "UndefinedOrInvalidValueVector.h"
#include "ResponseTypefn.h"
#include "Result.h"
#include "Pre.2.0.ResponseTypeFailureAreaMax.h"
#include "Pre.2.0.ResponseTypeFailureAreaMin.h"
#include "Pre.2.0.ResponseTypeFailureAreaPercMax.h"
#include "Pre.2.0.ResponseTypeFailureAreaPercMin.h"
#include "VersionNumber.h"

namespace cora
{

CResponseType::CResponseType(CSummaryResultFile& summaryResultFile,
  const QString& versionNumber, const std::vector <QString>& function)
: m_responseTypeBase(
  selectResponseType(summaryResultFile, versionNumber, function))
, m_summaryResultFile(summaryResultFile)
{
}

namespace
{

const QString INVALID_VALUE =
  "response parameter '%1' contains an invalid value";

} // anonymous namespace

void CResponseType::calculate(CResponseParameterFile& responseParameterFile,
  const TObject& object, const TFailureMode& failureMode)
{
  if (m_responseTypeBase != 0)
  {
  double value = CResponseTypeBase::UNDEFINED_OR_INVALID_RESPONSE_TYPE;

  try
  {
      value = m_responseTypeBase->calculate(object, failureMode);
  }

  catch (const CUndefinedOrInvalidValueVector&)
  {
  }

  QString responseParameter = QString("%1,%2,%3%4,%5").
      arg(m_responseTypeBase->getFailureType()).
      arg(failureMode->getResultComponent()->ExportLabel()).arg(object->prefix()).
      arg(object->name()).arg(m_responseTypeBase->getResponseType());

  if (value == CResponseTypeBase::UNDEFINED_OR_INVALID_RESPONSE_TYPE)
  {
      m_summaryResultFile.setResultValue(
    CSummaryResultFile::RESULT_VALUE_TOO_MANY);
      m_summaryResultFile.addAdditionalInformation(
    QString(INVALID_VALUE).arg(responseParameter));
  }

  responseParameterFile.addResponseParameter(QString("%1,%2").
      arg(responseParameter).arg(value));
  }
}

// private

namespace
{

const QString RESPONSE_TYPE_DOES_NOT_EXIST =
  "response type '%1' does not exist";

} // anonymous namespace

// static

TResponseTypeBase CResponseType::selectResponseType(
  CSummaryResultFile& summaryResultFile, const QString& versionNumber,
  const std::vector <QString>& function)
{
  try
  {
  if (function[3] == CResponseTypeBase::RESPONSE_TYPE_MAX)
  {
      return TResponseTypeBase(new CResponseTypeMax(
    summaryResultFile, function));
  }
  else if (function[3] == CResponseTypeBase::RESPONSE_TYPE_MIN)
  {
      return TResponseTypeBase(new CResponseTypeMin(
    summaryResultFile, function));
  }
  else if (function[3] == CResponseTypeBase::RESPONSE_TYPE_MEAN)
  {
      return TResponseTypeBase(new CResponseTypeMean(
    summaryResultFile, function));
  }
  else if ((versionNumber < MINIMUM_XML_VERSION_NUMBER) && (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MAX))
  {
      return TResponseTypeBase(new CPre20ResponseTypeFailureAreaMax(
    summaryResultFile, function));
  }
  else if ((versionNumber < MINIMUM_XML_VERSION_NUMBER) && (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MIN))
  {
      return TResponseTypeBase(new CPre20ResponseTypeFailureAreaMin(
    summaryResultFile, function));
  }
  else if ((versionNumber < MINIMUM_XML_VERSION_NUMBER) && (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MAX))
  {
      return TResponseTypeBase(new CPre20ResponseTypeFailureAreaPercMax(
    summaryResultFile, function));
  }
  else if ((versionNumber < MINIMUM_XML_VERSION_NUMBER) && (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MIN))
  {
      return TResponseTypeBase(new CPre20ResponseTypeFailureAreaPercMin(
    summaryResultFile, function));
  }
  else if (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MAX)
  {
      return TResponseTypeBase(new CResponseTypeFailureAreaMax(
    summaryResultFile, function));
  }
  else if (function[3] == CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_MIN)
  {
      return TResponseTypeBase(new CResponseTypeFailureAreaMin(
    summaryResultFile, function));
  }
  else if (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MAX)
  {
      return TResponseTypeBase(new CResponseTypeFailureAreaPercMax(
    summaryResultFile, function));
  }
  else if (function[3] ==
      CResponseTypeBase::RESPONSE_TYPE_FAILURE_AREA_PERCENTAGE_MIN)
  {
      return TResponseTypeBase(new CResponseTypeFailureAreaPercMin(
    summaryResultFile, function));
  }
  else if (QRegExp(CResponseTypeBase::RESPONSE_TYPE_FN).
      exactMatch(function[3]))
  {
      return TResponseTypeBase(new CResponseTypefn(
    summaryResultFile, function));
  }

  summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  summaryResultFile.addAdditionalInformation(
      QString(RESPONSE_TYPE_DOES_NOT_EXIST).arg(function[3]));
  }

  catch (const CIncompleteLimitStateFunction&)
  {
  }

  return TResponseTypeBase();
}

} // namespace cora
