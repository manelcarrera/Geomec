#include "ResponseTypeBaseWithValue.h"
#include "SummaryResultFile.h"
#include "IncompleteLimitStateFunction.h"

namespace cora
{

namespace
{

const QString TOO_HIGH = "TooHigh";
const QString TOO_LOW = "TooLow";

const size_t LSF_LENGTH_WITH_VALUE = 6;
const size_t VALUE_POSITION = 4;

} // anonymous namepsace

CResponseTypeBaseWithValue::CResponseTypeBaseWithValue(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function)
: CResponseTypeBaseValidate(summaryResultFile, function)
, m_value(extractValue(summaryResultFile, function, LSF_LENGTH_WITH_VALUE,
  VALUE_POSITION))
, m_tooHigh(extractInternalFailsWhen(summaryResultFile, function, TOO_HIGH))
, m_tooLow(extractInternalFailsWhen(summaryResultFile, function, TOO_LOW))
{
}

const QString CResponseTypeBaseWithValue::getResponseType() const
{
  QString responseType = QString("%1,%2,%3").
  arg(CResponseTypeBase::getResponseType()).arg(m_value).
  arg(m_tooHigh ? TOO_HIGH : TOO_LOW);

  return responseType;
}

// protected

// static

namespace
{

const QString LSF_DESCRIPTION_MISSES_INTERNAL_FAILS_WHEN =
  "the LSF description '%1' misses a 'InternalFailsWhen' parameter "
  "('TooHigh' or 'TooLow')";

const size_t INTERNAL_FAILS_WHEN_POSITION = 5;

} // anonymous namespace

bool CResponseTypeBaseWithValue::extractInternalFailsWhen(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function,
  const QString& internalFailsWhen)
{
  validateFunctionSize(summaryResultFile, function, LSF_LENGTH_WITH_VALUE);

  if ((function[INTERNAL_FAILS_WHEN_POSITION] != TOO_LOW) &&
      (function[INTERNAL_FAILS_WHEN_POSITION] != TOO_HIGH))
  {
  QString message = concatenateFunction(function);

  summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  message = QString(LSF_DESCRIPTION_MISSES_INTERNAL_FAILS_WHEN).arg(message);
  summaryResultFile.addAdditionalInformation(message);

  throw CIncompleteLimitStateFunction(message.toStdString());
  }

  return (function[INTERNAL_FAILS_WHEN_POSITION] == internalFailsWhen);
}

} // namespace cora
