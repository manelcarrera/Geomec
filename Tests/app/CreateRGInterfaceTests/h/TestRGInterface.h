#ifndef _TestRGInterface_h_
#define _TestRGInterface_h_

class RGInterface;

#include <vector>

#include <QString>

class ITestRGInterface
{
  public:
    ITestRGInterface();

    virtual void createNodes(RGInterface& rgInterface) = 0;
    virtual void createElements(RGInterface& rgInterface) = 0;
    virtual void createProperties(RGInterface& rgInterface) = 0;
    virtual void createFormationNames(RGInterface& rgInterface) = 0;

    void createDepletionStage(RGInterface& rgInterface);
    void createCommands(RGInterface& rgInterface);

  protected:
    template <typename T>
      std::vector <T> createNodes(T n0, T n1, T n2, T n3);

    void createTest(const QString& modelName);

  private:
    ITestRGInterface(const ITestRGInterface& rhs);
    ITestRGInterface& operator = (const ITestRGInterface& rhs);
};

template <typename T>
  std::vector <T> ITestRGInterface::createNodes(T n0, T n1, T n2, T n3)
{
  std::vector <T> nodes;

  nodes.push_back(n0 - 1);
  nodes.push_back(n1 - 1);
  nodes.push_back(n2 - 1);
  nodes.push_back(n3 - 1);

  return nodes;
}

#endif  // _TestRGInterface_h_
