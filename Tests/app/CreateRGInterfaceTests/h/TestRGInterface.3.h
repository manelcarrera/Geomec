#ifndef _TestRGInterface_3_h_
#define _TestRGInterface_3_h_

#include "TestRGInterface.h"

class CTestRGInterface3 : public ITestRGInterface {
public:
  CTestRGInterface3(const QString &path);

  virtual void createNodes(RGInterface &rgInterface);
  virtual void createElements(RGInterface &rgInterface);
  virtual void createProperties(RGInterface &rgInterface);
  virtual void createFormationNames(RGInterface &rgInterface);

private:
  CTestRGInterface3(const CTestRGInterface3 &rhs);
  CTestRGInterface3 &operator=(const CTestRGInterface3 &rhs);
};

#endif // _TestRGInterface_3_h_
