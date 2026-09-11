#ifndef _TestRGInterface_1_h_
#define _TestRGInterface_1_h_

#include "TestRGInterface.h"

class CTestRGInterface1 : public ITestRGInterface {
public:
  CTestRGInterface1(const QString &path);

  virtual void createNodes(RGInterface &rgInterface);
  virtual void createElements(RGInterface &rgInterface);
  virtual void createProperties(RGInterface &rgInterface);
  virtual void createFormationNames(RGInterface &rgInterface);

private:
  CTestRGInterface1(const CTestRGInterface1 &rhs);
  CTestRGInterface1 &operator=(const CTestRGInterface1 &rhs);
};

#endif // _TestRGInterface_1_h_
