#ifndef _TestRGInterface_5_h_
#define _TestRGInterface_5_h_

#include "TestRGInterface.h"

class CTestRGInterface5 : public ITestRGInterface {
public:
  CTestRGInterface5(const QString &path);

  virtual void createNodes(RGInterface &rgInterface);
  virtual void createElements(RGInterface &rgInterface);
  virtual void createProperties(RGInterface &rgInterface);
  virtual void createFormationNames(RGInterface &rgInterface);

private:
  CTestRGInterface5(const CTestRGInterface5 &rhs);
  CTestRGInterface5 &operator=(const CTestRGInterface5 &rhs);
};

#endif // _TestRGInterface_5_h_
