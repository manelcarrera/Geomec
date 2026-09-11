#ifndef _MaterialErrorHandler_h_
#define _MaterialErrorHandler_h_

class CAnalysisLogger;

#include <map>

#include <QtCore/QString>

class CMaterialErrorHandler
{
  public:
    CMaterialErrorHandler(CAnalysisLogger& analysisLogger);
    ~CMaterialErrorHandler();

    void handleError(const QString& material, const QString& parameter,
      const QString& message);
    void resetErrorHandler();

  private:
    CMaterialErrorHandler(const CMaterialErrorHandler& rhs);
    CMaterialErrorHandler& operator = (const CMaterialErrorHandler& rhs);

    CAnalysisLogger& m_analysisLogger;
    std::map <QString, size_t> m_errorMessages;
    bool m_firstMessage;
};

#endif  // _MaterialErrorHandler_h_
