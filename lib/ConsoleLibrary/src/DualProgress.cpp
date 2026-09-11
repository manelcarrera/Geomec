
#include "GlobalMessage.h"
#include "clDualProgress.h"

CDualProgress::CDualProgress(int /*jobs*/, const QString & /*primaryTitle*/, bool /*cancel*/)
    : CProgressBase(), m_secondaryTitle(), m_totalSteps(0), m_currentSteps(0) {}

CDualProgress::~CDualProgress() {}

void CDualProgress::AddSteps(int steps) { m_totalSteps += steps; }

void CDualProgress::Step(int steps) {
  int before = (m_currentSteps * 80) / m_totalSteps;
  int after = ((m_currentSteps + steps) * 80) / m_totalSteps;

  if (before != after) {
    _m()->outstream() << '*';
  }

  m_currentSteps += steps;
}

void CDualProgress::StatusMessage(const QString &message) {
  m_secondaryTitle = message;
  if (message != m_secondaryTitle) {
    _m()->outstream() << std::endl
                      << CProgressBase::getDateTimeStamp().toStdString() << message.toStdString() << std::endl;
    m_secondaryTitle = message;
    if (m_totalSteps > 0) {
      int n = (m_currentSteps * 80) / m_totalSteps;
      for (int i = 0; i < n; ++i)
        _m()->outstream() << '*';
    }
  }
}

void CDualProgress::NextJob(const QString &secondaryTitle) {
  _m()->outstream() << std::endl
                    << CProgressBase::getDateTimeStamp().toStdString() << secondaryTitle.toStdString() << std::endl;
  m_secondaryTitle = secondaryTitle;
  m_totalSteps = 0;
  m_currentSteps = 0;
}
