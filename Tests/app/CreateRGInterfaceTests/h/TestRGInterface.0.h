#ifndef _TestRGInterface_0_h_
#define _TestRGInterface_0_h_

#include "TestRGInterface.h"

class CTestRGInterface0 : public ITestRGInterface
{
  public:
  CTestRGInterface0(const QString& path);

  virtual void createNodes(RGInterface& rgInterface);
  virtual void createElements(RGInterface& rgInterface);
  virtual void createProperties(RGInterface& rgInterface);
  virtual void createFormationNames(RGInterface& rgInterface);

  private:
  CTestRGInterface0(const CTestRGInterface0& rhs);
  CTestRGInterface0& operator = (const CTestRGInterface0& rhs);
};

#endif  // _TestRGInterface_0_h_
