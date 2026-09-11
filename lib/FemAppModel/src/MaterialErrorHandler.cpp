#include "MaterialErrorHandler.h"
//#include "GlobalMessage.h"
#include "AnalysisLogger.h"

namespace
{

const std::string REPEAT_HEADER = "All messages concerning distributed "
  "material property values are repeated paired with their number of "
  "occurrences.";
const QString REPEAT_MESSAGE = "The message \"%1\" has been encountered %2 "
  "times.";
const QString LOG_MESSAGE = "Assigning distributed material property values "
  "for property '%1' of material '%2' resulted in the error message '%3'.";

} // anonymous namespace

CMaterialErrorHandler::CMaterialErrorHandler(CAnalysisLogger& analysisLogger)
: m_analysisLogger(analysisLogger)
, m_errorMessages()
, m_firstMessage(true)
{
}

CMaterialErrorHandler::~CMaterialErrorHandler()
{
  if (!m_errorMessages.empty())
  {
  m_analysisLogger.AddLine(REPEAT_HEADER);

  for (std::map <QString, size_t> ::const_iterator errorMessage =
      m_errorMessages.begin(); errorMessage != m_errorMessages.end();
      ++errorMessage)
  {
      QString logMessage = QString(REPEAT_MESSAGE).arg((*errorMessage).first).
    arg((*errorMessage).second);

      m_analysisLogger.AddLine(logMessage.toStdString());
  }
  }
}

void CMaterialErrorHandler::handleError(const QString& material,
  const QString& parameter, const QString& message)
{
  if (!message.isEmpty())
  {
  //if (m_firstMessage)
  //{
  //  _m()->msg(GLOBAL_MESSAGE);
  //  m_firstMessage = false;
  //}

  QString errorMessage =
      QString(LOG_MESSAGE).arg(parameter).arg(material).arg(message);
  std::pair <std::map <QString, size_t> ::iterator, bool> result =
      m_errorMessages.
      insert(std::map<QString, size_t>::value_type(errorMessage, 1));

  if (!result.second)
  {
      ++(*result.first).second;
  }
  else
  {
      m_analysisLogger.AddLine(errorMessage.toStdString());
  }
  }
}

void CMaterialErrorHandler::resetErrorHandler()
{
  m_errorMessages.clear();
  m_firstMessage = true;
}
