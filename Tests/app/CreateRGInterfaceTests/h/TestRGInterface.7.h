#ifndef _TestRGInterface_7_h_
#define _TestRGInterface_7_h_

#include "TestRGInterface.h"

class CTestRGInterface7 : public ITestRGInterface {
public:
  CTestRGInterface7(const QString &path);

  virtual void createNodes(RGInterface &rgInterface);
  virtual void createElements(RGInterface &rgInterface);
  virtual void createProperties(RGInterface &rgInterface);
  virtual void createFormationNames(RGInterface &rgInterface);

private:
  CTestRGInterface7(const CTestRGInterface7 &rhs);
  CTestRGInterface7 &operator=(const CTestRGInterface7 &rhs);
};

#endif // _TestRGInterface_7_h_
