#ifndef _TestRGInterface_9_h_
#define _TestRGInterface_9_h_

#include "TestRGInterface.h"

class CTestRGInterface9 : public ITestRGInterface
{
  public:
  CTestRGInterface9(const QString& path);

  virtual void createNodes(RGInterface& rgInterface);
  virtual void createElements(RGInterface& rgInterface);
  virtual void createProperties(RGInterface& rgInterface);
  virtual void createFormationNames(RGInterface& rgInterface);

  private:
  CTestRGInterface9(const CTestRGInterface9& rhs);
  CTestRGInterface9& operator = (const CTestRGInterface9& rhs);
};

#endif  // _TestRGInterface_9_h_
