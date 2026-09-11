#ifndef _IDC_ANALYSIS_STATUS_
#define _IDC_ANALYSIS_STATUS_

#include <cstddef>
#include <vector>

class QString;


namespace dia {

class IAnalysisStatus
{
public:
  IAnalysisStatus();
  virtual ~IAnalysisStatus();

  virtual void ParseMessage(const QString& message);

  virtual int PhaseNumber() const;

  virtual bool InitializationConvergence() const;

  virtual size_t TimeSteps() const;
  virtual size_t TimeStepsConverged() const;

  virtual bool TimeStepConverged(std::size_t step) const;

  // shortcut
  virtual bool Converged() const;

private:

  void ProcessTimeStep(std::size_t step, bool value);

  int m_nPhase;

  bool m_bInitializationConvergence;

  std::vector<bool> m_vcTimeSteps;
};


class IAnalysisStatusContainer
{
public:
  IAnalysisStatusContainer();
  virtual ~IAnalysisStatusContainer();

  virtual size_t Phases() const;
  virtual IAnalysisStatus *Phase(size_t phase) const;

  virtual void NewPhase();
  virtual IAnalysisStatus *LastPhase() const;

  virtual bool Converged();

private:
  std::vector<IAnalysisStatus *> m_vcAnalysisStatus;
};


}


#endif