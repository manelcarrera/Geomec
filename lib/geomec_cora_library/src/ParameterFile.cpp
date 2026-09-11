#include <iostream>
#include <sstream>

#include "ParameterFile.h"
#include "SummaryResultFile.h"
#include "ParameterFormation4Pressure.h"
#include "ParameterFormation4PressureChange.h"

namespace cora
{

CParameterFile::CParameterFile(CSummaryResultFile& summaryResultFile,
  const CGetModelInfo& modelInfo, const QString& fileName)
: m_fileName(fileName)
, m_parameterFile(m_fileName.toStdString().c_str())
, m_summaryResultFile(summaryResultFile)
, m_parameters(readParameterFile(m_summaryResultFile, modelInfo,
    m_parameterFile, m_fileName))
{
  validateParameterFile();
}

std::vector <TFailureTypeParameter> CParameterFile::getParameters() const
{
  return m_parameters;
}

// private

namespace
{

const int INITIAL_DEPLETION_STAGE = 0;

const QString PRESSURE_ORDER =
  "Pressure_D0 not defined before Dlt_Pressure_D..., re-ordering";
const QString DEPLETION_STAGE_DESIGNATOR =
  "Pressure_Dx and Dlt_Pressure_Dx have the same depletion stage designator";

} // anonymous namespace

void CParameterFile::validateParameterFile()
{
  validatePressureOrdering();
  validateDepletionStageDesignator();
}

void CParameterFile::validatePressureOrdering()
{
  bool pressureSeen = false, pressureChangeSeen = false;
  size_t pressurePosition = 0, pressureChangePosition = 0, position = 0;

  for (std::vector <TFailureTypeParameter> ::const_iterator parameter =
    m_parameters.begin(); parameter != m_parameters.end(); ++parameter,
    ++position)
  {
    TParameter actualParameter = (*parameter)->getActualParameter();

    if (actualParameter->isParameterType <CParameterFormation4Pressure> ())
    {
      if (actualParameter->depletionStage() == INITIAL_DEPLETION_STAGE)
      {
        pressureSeen = true;
        pressurePosition = position;
      }
    }

    if (actualParameter->
      isParameterType <CParameterFormation4PressureChange> ())
    {
      pressureChangeSeen = true;
      pressureChangePosition = position;
    }
  }

  if (pressureSeen && pressureChangeSeen &&
    (pressurePosition > pressureChangePosition))
  {
    m_summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    m_summaryResultFile.addAdditionalInformation(PRESSURE_ORDER);

    m_parameters[pressurePosition].swap(m_parameters[pressureChangePosition]);
  }
}

void CParameterFile::validateDepletionStageDesignator()
{
  std::set <int> pressureDepletionStages;
  bool equalDepletionStageDesignator = false;

  for (std::vector <TFailureTypeParameter> ::const_iterator parameter =
    m_parameters.begin(); parameter != m_parameters.end(); ++parameter)
  {
    TParameter actualParameter = (*parameter)->getActualParameter();

    if (actualParameter->isParameterType <CParameterFormation4Pressure> ())
    {
      pressureDepletionStages.insert(actualParameter->depletionStage());
    }
  }

  for (std::vector <TFailureTypeParameter> ::const_iterator parameter =
    m_parameters.begin(); parameter != m_parameters.end(); ++parameter)
  {
    TParameter actualParameter = (*parameter)->getActualParameter();

    if (actualParameter->
      isParameterType <CParameterFormation4PressureChange> ())
    {
      if (!pressureDepletionStages.insert(actualParameter->depletionStage()).
        second)
      {
        equalDepletionStageDesignator = true;
      }
    }
  }

  if (equalDepletionStageDesignator)
  {
    m_summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    m_summaryResultFile.addAdditionalInformation(DEPLETION_STAGE_DESIGNATOR);
  }
}

// static

namespace
{

const QString PARAMETER_FILE_DOES_NOT_EXIST =
  "parameter file '%1' does not exist";

} // anonymous namespace

std::vector <TFailureTypeParameter> CParameterFile::readParameterFile(
  CSummaryResultFile& summaryResultFile, const CGetModelInfo& modelInfo,
  std::ifstream& parameterFile, const QString& fileName)
{
  if (!parameterFile.good())
  {
    summaryResultFile.setResultValue(
      CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
    summaryResultFile.addAdditionalInformation(
      QString(PARAMETER_FILE_DOES_NOT_EXIST).arg(fileName));
  }

  std::vector <TFailureTypeParameter> parameters;
  std::string line;

  while (!parameterFile.eof() && !std::getline(parameterFile, line).fail())
  {
    if (!line.empty())
    {
      std::istringstream stream(line, std::istringstream::in);
      TFailureTypeParameter failureType = TFailureTypeParameter();

      try
      {
        failureType = TFailureTypeParameter(
          new CFailureTypeParameter(summaryResultFile, stream, modelInfo));
      }

      catch (const std::exception& exception)
      {
        summaryResultFile.setResultValue(
          CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
        summaryResultFile.addAdditionalInformation(exception.what());
      }

      if (failureType)
      {
        parameters.push_back(failureType);
      }
    }
  }

  return parameters;
}

} // namespace cora
