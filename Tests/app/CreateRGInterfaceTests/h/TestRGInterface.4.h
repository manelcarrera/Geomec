#ifndef _TestRGInterface_4_h_
#define _TestRGInterface_4_h_

#include "TestRGInterface.h"

class CTestRGInterface4 : public ITestRGInterface
{
  public:
  CTestRGInterface4(const QString& path);

  virtual void createNodes(RGInterface& rgInterface);
  virtual void createElements(RGInterface& rgInterface);
  virtual void createProperties(RGInterface& rgInterface);
  virtual void createFormationNames(RGInterface& rgInterface);

  private:
  CTestRGInterface4(const CTestRGInterface4& rhs);
  CTestRGInterface4& operator = (const CTestRGInterface4& rhs);
};

#endif  // _TestRGInterface_4_h_
