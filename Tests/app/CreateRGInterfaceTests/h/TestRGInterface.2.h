#ifndef _TestRGInterface_2_h_
#define _TestRGInterface_2_h_

#include "TestRGInterface.h"

class CTestRGInterface2 : public ITestRGInterface
{
  public:
    CTestRGInterface2(const QString& path);

    virtual void createNodes(RGInterface& rgInterface);
    virtual void createElements(RGInterface& rgInterface);
    virtual void createProperties(RGInterface& rgInterface);
    virtual void createFormationNames(RGInterface& rgInterface);

  private:
    CTestRGInterface2(const CTestRGInterface2& rhs);
    CTestRGInterface2& operator = (const CTestRGInterface2& rhs);
};

#endif  // _TestRGInterface_2_h_
