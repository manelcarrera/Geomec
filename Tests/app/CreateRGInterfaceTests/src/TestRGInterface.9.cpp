#include "TestRGInterface.9.h"
#include "RGInterface.h"

CTestRGInterface9::CTestRGInterface9(const QString& modelName)
: ITestRGInterface()
{
  createTest(modelName);
}

void CTestRGInterface9::createNodes(RGInterface& rgInterface)
{
  rgInterface.addNode(RGNode(60, 40, 0));
  rgInterface.addNode(RGNode(39.7148322911, 46.8832038274, 19.9687829511));
  rgInterface.addNode(RGNode(80, 40, 20));
  rgInterface.addNode(RGNode(39.713185291, 29.9380846907, 20.1332560497));
  rgInterface.addNode(RGNode(19.9996732452, 30.0004840081, 30.0004840081));
  rgInterface.addNode(RGNode(20, 40, 0));
  rgInterface.addNode(RGNode(39.6433876033, 19.8039507046, 49.5685564843));
  rgInterface.addNode(RGNode(39.9987052291, 39.9987052291, 39.9987052291));
  rgInterface.addNode(RGNode(20, 0, 20));
  rgInterface.addNode(RGNode(40, 0, 40));
  rgInterface.addNode(RGNode(40, 20, 0));
  rgInterface.addNode(RGNode(58.4345812526, 28.0297676851, 30.3207301809));
  rgInterface.addNode(RGNode(40, 0, 20));
  rgInterface.addNode(RGNode(40.2215991206, 61.7672995302, 51.5552286983));
  rgInterface.addNode(RGNode(40, 80, 60));
  rgInterface.addNode(RGNode(30.1502923931, 50.2073983612, 60.4868116305));
  rgInterface.addNode(RGNode(20, 80, 60));
  rgInterface.addNode(RGNode(60, 80, 60));
  rgInterface.addNode(RGNode(59.4105728655, 49.7028902139, 50.5987459088));
  rgInterface.addNode(RGNode(60, 60, 80));
  rgInterface.addNode(RGNode(19.8367102677, 50.1638658981, 39.4430578334));
  rgInterface.addNode(RGNode(40, 60, 80));
  rgInterface.addNode(RGNode(60, 80, 40));
  rgInterface.addNode(RGNode(51.4712421665, 60.2532056468, 28.7779579585));
  rgInterface.addNode(RGNode(0, 60, 20));
  rgInterface.addNode(RGNode(0, 60, 40));
  rgInterface.addNode(RGNode(0, 40, 20));
  rgInterface.addNode(RGNode(40, 40, 80));
  rgInterface.addNode(RGNode(50.1919963496, 29.8841134235, 59.8230530917));
  rgInterface.addNode(RGNode(30.0004840081, 30.0004840081, 60.0003267548));
  rgInterface.addNode(RGNode(20, 80, 40));
  rgInterface.addNode(RGNode(20, 80, 20));
  rgInterface.addNode(RGNode(29.9251485075, 60.8884205496, 29.4550624161));
  rgInterface.addNode(RGNode(40, 60, 0));
  rgInterface.addNode(RGNode(20, 60, 0));
  rgInterface.addNode(RGNode(40, 80, 20));
  rgInterface.addNode(RGNode(40, 80, 40));
  rgInterface.addNode(RGNode(0, 40, 40));
  rgInterface.addNode(RGNode(20, 20, 0));
  rgInterface.addNode(RGNode(0, 40, 60));
  rgInterface.addNode(RGNode(0, 60, 60));
  rgInterface.addNode(RGNode(20, 0, 40));
  rgInterface.addNode(RGNode(40, 0, 60));
  rgInterface.addNode(RGNode(20, 60, 80));
  rgInterface.addNode(RGNode(0, 20, 40));
  rgInterface.addNode(RGNode(0, 20, 20));
  rgInterface.addNode(RGNode(60, 60, 0));
  rgInterface.addNode(RGNode(40, 40, 0));
  rgInterface.addNode(RGNode(80, 20, 20));
  rgInterface.addNode(RGNode(80, 40, 40));
  rgInterface.addNode(RGNode(80, 60, 40));
  rgInterface.addNode(RGNode(60, 0, 40));
  rgInterface.addNode(RGNode(80, 20, 40));
  rgInterface.addNode(RGNode(60, 40, 80));
  rgInterface.addNode(RGNode(80, 40, 60));
  rgInterface.addNode(RGNode(20, 40, 80));
  rgInterface.addNode(RGNode(20, 80, 80));
  rgInterface.addNode(RGNode(80, 60, 60));
  rgInterface.addNode(RGNode(60, 0, 60));
  rgInterface.addNode(RGNode(60, 0, 20));
  rgInterface.addNode(RGNode(80, 20, 60));
  rgInterface.addNode(RGNode(20, 0, 60));
  rgInterface.addNode(RGNode(0, 20, 60));
  rgInterface.addNode(RGNode(20, 20, 80));
  rgInterface.addNode(RGNode(40, 20, 80));
  rgInterface.addNode(RGNode(60, 20, 0));
  rgInterface.addNode(RGNode(80, 20, 80));
  rgInterface.addNode(RGNode(80, 0, 60));
  rgInterface.addNode(RGNode(80, 80, 80));
  rgInterface.addNode(RGNode(80, 60, 80));
  rgInterface.addNode(RGNode(80, 80, 60));
  rgInterface.addNode(RGNode(80, 80, 40));
  rgInterface.addNode(RGNode(80, 80, 20));
  rgInterface.addNode(RGNode(60, 80, 20));
  rgInterface.addNode(RGNode(80, 60, 20));
  rgInterface.addNode(RGNode(80, 60, 0));
  rgInterface.addNode(RGNode(80, 80, 0));
  rgInterface.addNode(RGNode(60, 80, 0));
  rgInterface.addNode(RGNode(40, 80, 0));
  rgInterface.addNode(RGNode(20, 80, 0));
  rgInterface.addNode(RGNode(0, 60, 0));
  rgInterface.addNode(RGNode(80, 40, 0));
  rgInterface.addNode(RGNode(40, 0, 0));
  rgInterface.addNode(RGNode(0, 20, 0));
  rgInterface.addNode(RGNode(60, 0, 0));
  rgInterface.addNode(RGNode(0, 0, 0));
  rgInterface.addNode(RGNode(0, 0, 20));
  rgInterface.addNode(RGNode(20, 0, 0));
  rgInterface.addNode(RGNode(0, 40, 0));
  rgInterface.addNode(RGNode(60, 80, 80));
  rgInterface.addNode(RGNode(80, 40, 80));
  rgInterface.addNode(RGNode(40, 80, 80));
  rgInterface.addNode(RGNode(60, 20, 80));
  rgInterface.addNode(RGNode(20, 0, 80));
  rgInterface.addNode(RGNode(40, 0, 80));
  rgInterface.addNode(RGNode(0, 0, 60));
  rgInterface.addNode(RGNode(60, 0, 80));
  rgInterface.addNode(RGNode(80, 0, 40));
  rgInterface.addNode(RGNode(0, 20, 80));
  rgInterface.addNode(RGNode(0, 0, 40));
  rgInterface.addNode(RGNode(80, 0, 20));
  rgInterface.addNode(RGNode(0, 60, 80));
  rgInterface.addNode(RGNode(0, 40, 80));
  rgInterface.addNode(RGNode(0, 80, 60));
  rgInterface.addNode(RGNode(0, 80, 40));
  rgInterface.addNode(RGNode(0, 80, 20));
  rgInterface.addNode(RGNode(0, 80, 80));
  rgInterface.addNode(RGNode(0, 80, 0));
  rgInterface.addNode(RGNode(0, 0, 80));
  rgInterface.addNode(RGNode(80, 0, 80));
  rgInterface.addNode(RGNode(80, 0, 0));
  rgInterface.addNode(RGNode(80, 20, 0));
}

void CTestRGInterface9::createElements(RGInterface& rgInterface)
{
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 4, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (4, 5, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 4, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 13, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 2, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 4, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 16, 17)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 18, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 21, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 16, 22)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 18, 19, 23)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 19, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 19, 23, 24)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 26, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 19, 28, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 29, 30, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 19, 24, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 27, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 31, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 26, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 33, 34, 35)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 34, 35, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 33, 36, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 21, 33, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 21, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 27, 38, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 38, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 31, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 4, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 38, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 17, 21, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 27, 35)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 30, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 21, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 43, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 26, 38, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 16, 22, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 45, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 41, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 45, 46, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 5, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 24, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 24, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 4, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 4, 48, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 24, 34, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 21, 33, 35)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 24, 33, 34)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 21, 35, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 33, 34, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 3, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 29, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 24, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 23, 24, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 52, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 29, 54, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 24, 3, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 24, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 50, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 29, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 24, 50, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 29, 50, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 20, 22)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 3, 50, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 2, 24, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 47, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 18, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 24, 33, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 28, 30, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (31, 32, 33, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 33, 35, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 41, 44, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 26, 27, 38)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 19, 29, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 24, 33, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 30, 43, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 20, 22)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 50, 51, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 31, 33, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 20, 22, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 28, 29, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 19, 20, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 30, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 50, 53, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 52, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 13, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 13, 52, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 52, 53, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 38, 40, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 43, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 40, 56, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 13, 4, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (28, 30, 56, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (28, 29, 54, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 43, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 45, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 3, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 61, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 58, 69, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 51, 71, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 20, 58, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 58, 69, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (72, 73, 74, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 21, 33, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 73, 76, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 34, 47, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 34, 47, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 24, 47, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 47, 75, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 47, 74, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 34, 48, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 36, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 2, 24, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 27, 35, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 2, 4, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 39, 4, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 38, 45, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 79, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 32, 35, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 35, 36, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 3, 66, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 49, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 20, 54, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 3, 49, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 39, 4, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 60, 66, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 6, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 66, 83, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 86, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 88, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 83, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 6, 84, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 20, 22, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 28, 29, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 22, 28, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (54, 55, 70, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 54, 55, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 22, 44, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 20, 28, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 22, 44, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (28, 29, 30, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 54, 65, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 17, 21)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (28, 30, 64, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 64, 65, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 51, 72, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 18, 20, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 62, 64, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (43, 65, 93, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (55, 61, 91, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 40, 44, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 64, 94, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 65, 94, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 43, 65, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 59, 61, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (43, 59, 93, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 43, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 61, 67, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 52, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (52, 59, 61, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 63, 64, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 42, 43, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 52, 59, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 42, 62, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 13, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 55, 58, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (52, 53, 60, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 60, 66, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 96, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 42, 62, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 4, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 42, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 49, 60, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 49, 60, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 55, 61, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 52, 53, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 4, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 60, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 46, 87, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 39, 88, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 73, 75, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 13, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 87, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 45, 46, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 4, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 47, 74, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 42, 45, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 46, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 52, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 56, 63, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 103, 40, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 30, 43, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 40, 41, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 41, 44, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 56, 63, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (54, 55, 91, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 4, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 40, 45, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 17, 26, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 40, 44, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 30, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 21, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 30, 40, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 38, 46, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 17, 26, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 4, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 32, 35)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 35, 81, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 25, 32, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 106, 25, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 27, 35, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 46, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 25, 31, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 62, 63, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 17, 44, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 107, 41, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 35, 6, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 32, 80, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 22, 90, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 44, 57, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 16, 17, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 62, 63, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 107, 41, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (43, 62, 65, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (109, 64, 94, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 17, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 105, 17, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 20, 69, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 23, 51, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 24, 33, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 25, 26, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 40, 56, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 54, 55, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 35, 80, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 74, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 72, 74, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 33, 36, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 31, 32, 33)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 51, 58, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 74, 78, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 24, 3, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 36, 74, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 73, 74, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 108, 32, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 21, 26, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 23, 24, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 19, 51, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 34, 36, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 59, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 20, 55, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (61, 67, 91, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 67, 93, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 43, 59, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 53, 55, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (52, 61, 68, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 50, 55, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (52, 53, 61, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 49, 50, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 59, 67, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 51, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 3, 51, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 49, 53, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 53, 60, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 3, 49, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 3, 75, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 111, 49, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 86, 88, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 73, 77, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 75, 76, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 3, 66, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 17, 41, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 3, 49, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 18, 23, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 46, 6, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 32, 33, 35)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 46, 84, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 21, 26, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 63, 64, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 34, 35, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 38, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 49, 66, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 42, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 36, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (109, 64, 96, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 19, 20, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 4, 48, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 40, 41, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 52, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 21, 31, 33)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 73, 74, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 49, 53, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 86, 87, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 43, 62, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 18, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (43, 93, 95, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 112, 49, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 4, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 45, 46, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 42, 45, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 36, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 19, 23, 51)));
}

void CTestRGInterface9::createProperties(RGInterface& rgInterface)
{
  std::vector <int> elementFormation;

  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);
  elementFormation.push_back(1);

  rgInterface.saveProperty(RGPropertyType::propElementFormation,
  elementFormation);
}

namespace
{

const std::string FORMATION_0 = "Formation0";

} // anonymous namespace

void CTestRGInterface9::createFormationNames(RGInterface& rgInterface)
{
  rgInterface.setFormationName(1, FORMATION_0);
}
