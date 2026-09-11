#ifndef _TestRGInterface_8_h_
#define _TestRGInterface_8_h_

#include "TestRGInterface.h"

class CTestRGInterface8 : public ITestRGInterface
{
  public:
    CTestRGInterface8(const QString& path);

    virtual void createNodes(RGInterface& rgInterface);
    virtual void createElements(RGInterface& rgInterface);
    virtual void createProperties(RGInterface& rgInterface);
    virtual void createFormationNames(RGInterface& rgInterface);

  private:
    CTestRGInterface8(const CTestRGInterface8& rhs);
    CTestRGInterface8& operator = (const CTestRGInterface8& rhs);
};

#endif  // _TestRGInterface_8_h_
