#include "TestRGInterface.h"
#include "RGInterface.h"

ITestRGInterface::ITestRGInterface() {}

void ITestRGInterface::createDepletionStage(RGInterface &rgInterface) {
  rgInterface.setCurrentDepletionStage(RGDepletionStage(0, 0));
}

void ITestRGInterface::createCommands(RGInterface &rgInterface) {
  rgInterface.clearCommandList();
  rgInterface.addCommand(GMCommand(typeCommandLoadGrid));
  rgInterface.addCommand(GMCommand(typeCommandQuit));
}

// protected

void ITestRGInterface::createTest(const QString &modelName) {
  RGInterface rgInterface(modelName.toStdString());

  createNodes(rgInterface);
  createElements(rgInterface);
  createDepletionStage(rgInterface);
  createProperties(rgInterface);
  createFormationNames(rgInterface);
  createCommands(rgInterface);

  rgInterface.dumpModel();
}
