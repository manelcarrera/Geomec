#ifndef _TSPROGRESSBASE_H_
#define _TSPROGRESSBASE_H_

class ITSProgressBase {
public:
  ITSProgressBase() {}
  virtual ~ITSProgressBase() {}

  virtual void Begin(int nSteps) = 0;
  virtual void End() = 0;
  virtual void Step() = 0;
};

#endif // _TSPROGRESSBASE_H_
