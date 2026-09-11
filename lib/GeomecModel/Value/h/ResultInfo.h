#ifndef CRESULTINFO_H
#define CRESULTINFO_H

#include <string>

// class IResult;
class IValueComposite;

class CResultInfo {
public:
  CResultInfo();
  void Clear();

  void TimeStep(int);
  int TimeStep() const;

  void Result(const IValueComposite *result);
  const IValueComposite *Result() const;

  void Component(const std::string &cc);
  void Component(unsigned int ii);
  unsigned int Component() const;

  // for debug
  void Name(const std::string &nm);
  const std::string Name() const;

private:
  const IValueComposite *m_result;
  int m_timeStep;
  unsigned int m_component;
  std::string m_name;
};

#endif // CRESULTINFO_H
