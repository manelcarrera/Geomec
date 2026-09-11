#include <numeric>

#include "ResponseTypeBaseValidate.h"
#include "SummaryResultFile.h"
#include "IncompleteLimitStateFunction.h"

namespace cora
{

// static

namespace
{

const QString LSF_DESCRIPTION_DOES_NOT_CONTAIN_ENOUGH_PARAMETERS =
  "the LSF description '%1' does not contain enough parameters "
  "('%2' instead of '%3')";

} // anonymous namespace

bool CResponseTypeBaseValidate::validateFunctionSize(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function,
  size_t validFunctionSize)
{
  if (function.size() < validFunctionSize)
  {
  QString message = concatenateFunction(function);

  summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  message = QString(LSF_DESCRIPTION_DOES_NOT_CONTAIN_ENOUGH_PARAMETERS).
      arg(message).arg(function.size()).arg(validFunctionSize);
  summaryResultFile.addAdditionalInformation(message);

  throw CIncompleteLimitStateFunction(message.toStdString());
  }

  return true;
}

// protected

CResponseTypeBaseValidate::CResponseTypeBaseValidate(CSummaryResultFile& summaryResultFile,
  const std::vector <QString>& function)
: CResponseTypeBase(summaryResultFile, function)
{
}

// static

namespace
{

const QString COMMA = ",";
const QString EMPTY = "";

struct TAccumulate
{
  QString operator () (const QString& lhs, const QString& rhs)
  {
  return lhs + (lhs.length() > 0 ? COMMA : EMPTY) + rhs;
  }
};

} // anonymous namespace

QString CResponseTypeBaseValidate::concatenateFunction(
  const std::vector <QString>& function)
{
  /*
  Too bad, the C++11 lambda expression is not yet supported on all platforms

  QString concatenation = std::accumulate(function.begin(), function.end(),
  QString(), [] (QString& lhs, QString rhs) -> QString {
  return lhs + (lhs.length() > 0 ? COMMA : EMPTY) + rhs;});
  */

  QString concatenation = std::accumulate(function.begin(), function.end(),
  QString(), TAccumulate());

  return concatenation;
}

namespace
{

const QString LSF_CANNOT_CONVERT_2_DOUBLE = "the LSF description '%1' contains "
  "a value '%2' that cannot be converted to a double value";

} // anonymous namespace

double CResponseTypeBaseValidate::extractValue(
  CSummaryResultFile& summaryResultFile, const std::vector <QString>& function,
  size_t validFunctionSize, size_t valuePosition)
{
  validateFunctionSize(summaryResultFile, function, validFunctionSize);

  bool ok = true;
  double value = function[valuePosition].toDouble(&ok);

  if (!ok)
  {
  QString message = concatenateFunction(function);

  summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  message = QString(LSF_CANNOT_CONVERT_2_DOUBLE).arg(message).
      arg(function[valuePosition]);
  summaryResultFile.addAdditionalInformation(message);

  throw CIncompleteLimitStateFunction(message.toStdString());
  }

  return value;
}

} // namespace cora
