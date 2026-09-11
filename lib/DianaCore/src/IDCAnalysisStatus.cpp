#include "IDCAnalysisStatus.h"

#include <QRegExp>
#include <QString>
#include <QStringList>
#include <cassert>

namespace {
const QRegExp SEPERATOR = QRegExp("[ \t,]");
}

namespace dia {

IAnalysisStatus::IAnalysisStatus()
    : m_nPhase(0), m_bInitializationConvergence(true) // we're optimistic
{
  m_vcTimeSteps.reserve(64);
}

IAnalysisStatus::~IAnalysisStatus() {}

void IAnalysisStatus::ParseMessage(const QString &message) {
  size_t steps = m_vcTimeSteps.size();

  if (message.contains("Convergence") && message.contains("not reached")) {
    if (steps == 0)
      m_bInitializationConvergence = false;
    else
      ProcessTimeStep(steps - 1, false);
  } else if (message.startsWith("PHASE")) {
    QStringList l = message.split(SEPERATOR, QString::SkipEmptyParts);

    if (l.size() < 3)
      return;

    bool isInt;
    int phase = l[1].toInt(&isInt);

    if (isInt && l[2] == "INITIALIZED")
      m_nPhase = phase;
  } else if (message.startsWith("STEP")) {
    QStringList l = message.split(SEPERATOR, QString::SkipEmptyParts);

    if (l.size() < 2)
      return;

    bool isInt;
    int step = l[1].toInt(&isInt);

    if (isInt) {
      --step;

      if (step < 0 || step > m_vcTimeSteps.size())
        return;

      if (l.size() > 3 && l[2] == "TERMINATED") {
        if (l.size() > 4 && l[3] == "NO" && l[4] == "CONVERGENCE")
          ProcessTimeStep(step, false);
        else if (l[3] == "CONVERGENCE")
          ProcessTimeStep(step, true);
      } else // make sure we have it
      {
        if (step == m_vcTimeSteps.size())
          ProcessTimeStep(step, true);
      }
    }
  }
}

int IAnalysisStatus::PhaseNumber() const { return m_nPhase; }

bool IAnalysisStatus::InitializationConvergence() const { return m_bInitializationConvergence; }

size_t IAnalysisStatus::TimeSteps() const { return m_vcTimeSteps.size(); }

size_t IAnalysisStatus::TimeStepsConverged() const {
  size_t nConverged = 0;
  for (size_t i = 0; i < m_vcTimeSteps.size(); ++i)
    if (m_vcTimeSteps[i])
      ++nConverged;
  return nConverged;
}

bool IAnalysisStatus::TimeStepConverged(size_t step) const {
  // comparison of unsigned expression >= 0 is always true

  assert(/*step >= 0 &&*/ step < m_vcTimeSteps.size());
  return m_vcTimeSteps[step];
}

bool IAnalysisStatus::Converged() const { return InitializationConvergence() && TimeSteps() == TimeStepsConverged(); }

void IAnalysisStatus::ProcessTimeStep(size_t step, bool value) {
  if (step >= m_vcTimeSteps.size())
    m_vcTimeSteps.push_back(value);
  else
    m_vcTimeSteps[step] = value;
}

IAnalysisStatusContainer::IAnalysisStatusContainer() {}

IAnalysisStatusContainer::~IAnalysisStatusContainer() {
  for (size_t i = 0; i < m_vcAnalysisStatus.size(); ++i)
    delete m_vcAnalysisStatus[i];
}

size_t IAnalysisStatusContainer::Phases() const { return m_vcAnalysisStatus.size(); }

IAnalysisStatus *IAnalysisStatusContainer::Phase(size_t phase) const {
  if (phase < m_vcAnalysisStatus.size())
    return m_vcAnalysisStatus[phase];
  return 0;
}

void IAnalysisStatusContainer::NewPhase() { m_vcAnalysisStatus.push_back(new IAnalysisStatus()); }

IAnalysisStatus *IAnalysisStatusContainer::LastPhase() const {
  size_t size = m_vcAnalysisStatus.size();
  if (size > 0)
    return m_vcAnalysisStatus[size - 1];
  return 0;
}

bool IAnalysisStatusContainer::Converged() {
  bool retval = true;
  for (size_t i = 0; i < m_vcAnalysisStatus.size(); ++i)
    retval = retval && m_vcAnalysisStatus[i]->Converged();
  return retval;
}

} // namespace dia