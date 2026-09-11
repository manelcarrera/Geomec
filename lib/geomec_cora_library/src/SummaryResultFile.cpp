#include "SummaryResultFile.h"

namespace cora
{

namespace
{

const QString RESULT_OK = "OK";
const QString RESULT_INCONSISTENT = "Inconsistent input";
const QString RESULT_TOO_MANY = "Too many parameters out of range";
const QString RESULT_DIVERGENCE = "Divergence in analysis";

} // anonymous namespace

CSummaryResultFile::CSummaryResultFile(const QString& fileName)
: m_summaryResultFile(fileName.toStdString().c_str())
, m_resultValue(RESULT_VALUE_OK)
, m_additionalInformation()
{
}

CSummaryResultFile::~CSummaryResultFile()
{
  switch (m_resultValue)
  {
    case RESULT_VALUE_OK:
      m_summaryResultFile << RESULT_OK.toStdString();
      break;
    case RESULT_VALUE_INCONSISTENT:
      m_summaryResultFile << RESULT_INCONSISTENT.toStdString();
      break;
    case RESULT_VALUE_TOO_MANY:
      m_summaryResultFile << RESULT_TOO_MANY.toStdString();
      break;
    case RESULT_VALUE_DIVERGENCE:
      m_summaryResultFile << RESULT_DIVERGENCE.toStdString();
      break;
  }

  m_summaryResultFile << std::endl << m_additionalInformation.str();
}

CSummaryResultFile::TResultValue CSummaryResultFile::getResultValue() const
{
  return m_resultValue;
}

void CSummaryResultFile::setResultValue(TResultValue resultValue)
{
  m_resultValue = resultValue;
}

void CSummaryResultFile::addAdditionalInformation(
  const QString& additionalInformation)
{
  m_additionalInformation << additionalInformation.toStdString() << std::endl;
}

} // namespace cora
