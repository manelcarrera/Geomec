#include "ParameterModifier.h"
#include "Value.h"
#include "ParameterModifierFactor.h"
#include "ParameterModifierShift.h"
#include "ParameterModifierAbsolute.h"
#include "SummaryResultFile.h"
#include "ParameterModifierFactorShift.h"
#include "ParameterModifierShiftFactor.h"

namespace cora
{

CParameterModifier::CParameterModifier(CSummaryResultFile& summaryResultFile,
  const QString& option, std::vector <double>& value)
: m_parameterModifierBase(
    selectParameterModifier(summaryResultFile, option, value))
{
}

geo::CValue CParameterModifier::modify(const geo::CValue& value) const
{
  if (m_parameterModifierBase != 0)
  {
    return m_parameterModifierBase->modify(value);
  }

  return value;
}

CParameterModifier::TValueVec CParameterModifier::modify(TValueVec& valueVec)
{
  if (m_parameterModifierBase != 0)
  {
    return m_parameterModifierBase->modify(valueVec);
  }

  return valueVec;
}

// private

// static

namespace
{

const QString OPTION_FACTOR = "factor";
const QString OPTION_SHIFT = "shift";
const QString OPTION_ABSOLUTE = "absolute";
const QString OPTION_FACTOR_SHIFT = "factor_shift";
const QString OPTION_SHIFT_FACTOR = "shift_factor";

const QString ERROR_MESSAGE =
  "Unknown option '%1', known options are: '%2', '%3', '%4'";

const QString ERROR_COUNT = "The option '%1' requires %2 argument(s)";

const size_t ZERO_ARGUMENTS = 0;
const size_t ONE_ARGUMENT = 1;
const size_t TWO_ARGUMENTS = 2;

bool verifyArgumentCount(CSummaryResultFile& summaryResultFile,
  const QString& option, std::vector <double>& value)
{
  bool invalidCount = false;
  size_t requiredArguments = ZERO_ARGUMENTS;

  if ((option == OPTION_FACTOR) || (option == OPTION_SHIFT) ||
    (option == OPTION_ABSOLUTE))
  {
    requiredArguments = ONE_ARGUMENT;
    invalidCount = (value.size() != ONE_ARGUMENT);
  }
  else if ((option == OPTION_FACTOR_SHIFT) ||
    (option == OPTION_SHIFT_FACTOR))
  {
    requiredArguments = TWO_ARGUMENTS;
    invalidCount = (value.size() != TWO_ARGUMENTS);
  }

  if (invalidCount)
  {
    QString additionalInformation = QString(ERROR_COUNT).arg(option).
      arg(requiredArguments);

    summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    summaryResultFile.addAdditionalInformation(additionalInformation);
  }

  return !invalidCount;
}

} // anonymous namespace

TParameterModifierBase CParameterModifier::selectParameterModifier(
  CSummaryResultFile& summaryResultFile, const QString& option,
  std::vector <double>& value)
{
  TParameterModifierBase parameterModifierBase;

  if (verifyArgumentCount(summaryResultFile, option, value))
  {
    if (option == OPTION_FACTOR)
    {
      parameterModifierBase =
        TParameterModifierBase(new CParameterModifierFactor(value));
    }
    else if (option == OPTION_SHIFT)
    {
      parameterModifierBase =
        TParameterModifierBase(new CParameterModifierShift(value));
    }
    else if (option == OPTION_ABSOLUTE)
    {
      parameterModifierBase =
        TParameterModifierBase(new CParameterModifierAbsolute(value));
    }
    else if (option == OPTION_FACTOR_SHIFT)
    {
      parameterModifierBase =
        TParameterModifierBase(new CParameterModifierFactorShift(value));
    }
    else if (option == OPTION_SHIFT_FACTOR)
    {
      parameterModifierBase =
        TParameterModifierBase(new CParameterModifierShiftFactor(value));
    }
    else
    {
      QString additionalInformation = QString(ERROR_MESSAGE).arg(option).
        arg(OPTION_FACTOR).arg(OPTION_SHIFT).arg(OPTION_ABSOLUTE);

      summaryResultFile.setResultValue(
        CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
      summaryResultFile.addAdditionalInformation(additionalInformation);
    }
  }

  return parameterModifierBase;
}

} // namespace cora
