#ifndef _TestRGInterface_6_h_
#define _TestRGInterface_6_h_

#include "TestRGInterface.h"

class CTestRGInterface6 : public ITestRGInterface {
public:
  CTestRGInterface6(const QString &path);

  virtual void createNodes(RGInterface &rgInterface);
  virtual void createElements(RGInterface &rgInterface);
  virtual void createProperties(RGInterface &rgInterface);
  virtual void createFormationNames(RGInterface &rgInterface);

private:
  CTestRGInterface6(const CTestRGInterface6 &rhs);
  CTestRGInterface6 &operator=(const CTestRGInterface6 &rhs);
};

#endif // _TestRGInterface_6_h_
