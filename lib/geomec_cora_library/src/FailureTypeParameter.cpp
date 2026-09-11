#include <sstream>
#include <cassert>

#include "FailureTypeParameter.h"
#include "FailureTypeParameterFault.h"
#include "FailureTypeParameterFormation.h"
#include "FailureTypeParameterSurface.h"
#include "FailureTypeParameterWell.h"
#include "SummaryResultFile.h"
#include "GetModelInfo.h"
#include "ParameterFormation4Material.h"
#include "ParameterFormation4Pressure.h"
#include "FailureTypeParameterFormation4Material.h"
#include "FailureTypeParameterFormation4Pressure.h"
#include "ParameterFormation4PressureChange.h"
#include "FailureTypeParameterFormation4PressureChange.h"

namespace cora
{

CFailureTypeParameter::CFailureTypeParameter(
  CSummaryResultFile& summaryResultFile, std::istream& stream,
  const CGetModelInfo& modelInfo)
: m_failureTypeParameterBase(
  selectFailureType(summaryResultFile, stream, modelInfo))
{
}

void CFailureTypeParameter::modify(CModelBase* modelBase)
{
  if (m_failureTypeParameterBase != 0)
  {
  m_failureTypeParameterBase->modify(modelBase);
  }
}

const TParameter CFailureTypeParameter::getActualParameter() const
{
  return m_failureTypeParameterBase->getActualParameter();
}

// private

namespace
{

const std::string stripQuotes(const std::string& string)
{
  std::string stripped(string);

  stripped.erase(remove(stripped.begin(), stripped.end(), '\"'),
  stripped.end());

  return stripped;
}

const QString FAILURE_TYPE_FAULT = "fault";
const QString FAILURE_TYPE_FORMATION = "formation";
const QString FAILURE_TYPE_SURFACE = "surface";
const QString FAILURE_TYPE_WELL = "well";

const QString FAILURE_TYPE_SEPARATOR = ":";

const QString FAULT_SEPARATOR =
  FAILURE_TYPE_FAULT + FAILURE_TYPE_SEPARATOR;
const QString FORMATION_SEPARATOR =
  FAILURE_TYPE_FORMATION + FAILURE_TYPE_SEPARATOR;
const QString SURFACE_SEPARATOR =
  FAILURE_TYPE_SURFACE + FAILURE_TYPE_SEPARATOR;
const QString WELL_SEPARATOR =
  FAILURE_TYPE_WELL + FAILURE_TYPE_SEPARATOR;

const QString FAILURE_TYPE_DOES_NOT_EXIST =
  "failure type '%1' for object '%2' does not exist";

} // anonymous namespace

// static

namespace
{

struct TQuotedStringReader
{
  TQuotedStringReader(std::string& s)
  : string(s)
  {
  }

  std::string& string;
};

std::istream& operator >> (std::istream& stream,
  const TQuotedStringReader& quotedString)
{
  char c;

  stream >> c;

  if (stream && (c == '"'))
  {
  std::string extra;

  std::getline(stream, extra, '"');
  quotedString.string = std::string("\"").append(extra).append("\"");
  }
  else
  {
  stream.putback(c);
  stream >> quotedString.string;
  }
 
  return stream;
}

} // anonymous namespace

TFailureTypeParameterBase CFailureTypeParameter::selectFailureType(
  CSummaryResultFile& summaryResultFile, std::istream& stream,
  const CGetModelInfo& modelInfo)
{
  std::string object, option, parameter;
  std::vector <double> value;

  stream >> TQuotedStringReader(parameter) >> TQuotedStringReader(option) >>
  TQuotedStringReader(object);

  std::copy(std::istream_iterator <double> (stream),
  std::istream_iterator <double> (), std::back_inserter(value));

  object = stripQuotes(object).c_str();
  option = stripQuotes(option).c_str();
  parameter = stripQuotes(parameter).c_str();

  return selectFailureType(summaryResultFile, object, option, parameter, value,
  modelInfo);
}

TFailureTypeParameterBase CFailureTypeParameter::selectFailureType(
  CSummaryResultFile& summaryResultFile, const std::string& object,
  const std::string& option, const std::string& parameter,
  std::vector <double>& value, const CGetModelInfo& modelInfo)
{
  if (object.find(FAULT_SEPARATOR.toStdString().c_str()) == 0)
  {
  return TFailureTypeParameterBase(new CFailureTypeParameterFault(
      summaryResultFile, object, option, parameter, value, modelInfo));
  }
  else if (object.find(FORMATION_SEPARATOR.toStdString().c_str()) == 0)
  {
  TObject actualObject = CFailureTypeParameterBase::findActualObject(
      summaryResultFile, object, modelInfo.getFormationInfo().getObjects(),
      CFailureTypeParameterFormation::FORMATION);
  TParameter actualParameter = CFailureTypeParameterBase::findActualParameter(
      summaryResultFile, object, parameter, actualObject->getParameters());

  if (actualParameter->isParameterType <CParameterFormation4Material> ())
  {
      return TFailureTypeParameterBase(
    new CFailureTypeParameterFormation4Material(summaryResultFile, object,
          option, parameter, value, modelInfo));
  }
  else if (actualParameter->isParameterType <CParameterFormation4Pressure> ())
  {
      return TFailureTypeParameterBase(
    new CFailureTypeParameterFormation4Pressure(summaryResultFile, object,
          option, parameter, value, modelInfo));
  }
  else if (actualParameter->
      isParameterType <CParameterFormation4PressureChange> ())
  {
      return TFailureTypeParameterBase(
    new CFailureTypeParameterFormation4PressureChange(summaryResultFile,
          object, option, parameter, value, modelInfo));
  }
  }
  else if (object.find(SURFACE_SEPARATOR.toStdString().c_str()) == 0)
  {
  return TFailureTypeParameterBase(new CFailureTypeParameterSurface(
      summaryResultFile, object, option, parameter, value, modelInfo));
  }
  else if (object.find(WELL_SEPARATOR.toStdString().c_str()) == 0)
  {
  return TFailureTypeParameterBase(new CFailureTypeParameterWell(
      summaryResultFile, object, option, parameter, value, modelInfo));
  }

  std::string::size_type offset =
  object.find(FAILURE_TYPE_SEPARATOR.toStdString());

  summaryResultFile.setResultValue(
  CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  summaryResultFile.addAdditionalInformation(
  QString(FAILURE_TYPE_DOES_NOT_EXIST).arg(parameter.c_str()).
      arg(object.c_str()));

  return TFailureTypeParameterBase();
}

} // namespace cora
