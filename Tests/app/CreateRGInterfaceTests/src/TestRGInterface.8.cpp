#include "TestRGInterface.8.h"
#include "RGInterface.h"

CTestRGInterface8::CTestRGInterface8(const QString& modelName)
: ITestRGInterface()
{
  createTest(modelName);
}

void CTestRGInterface8::createNodes(RGInterface& rgInterface)
{
  rgInterface.addNode(RGNode(75, 50, 20));
  rgInterface.addNode(RGNode(75, 25, 0));
  rgInterface.addNode(RGNode(50, 25, 0));
  rgInterface.addNode(RGNode(75, 25, 20));
  rgInterface.addNode(RGNode(75, 50, 0));
  rgInterface.addNode(RGNode(75, 75, 0));
  rgInterface.addNode(RGNode(75, 75, 20));
  rgInterface.addNode(RGNode(50, 50, 0));
  rgInterface.addNode(RGNode(25, 50, 0));
  rgInterface.addNode(RGNode(50, 75, 20));
  rgInterface.addNode(RGNode(37.4987861522, 37.4987861522, 19.9987861522));
  rgInterface.addNode(RGNode(25, 50, 40));
  rgInterface.addNode(RGNode(50, 50, 40));
  rgInterface.addNode(RGNode(0, 75, 0));
  rgInterface.addNode(RGNode(0, 75, 20));
  rgInterface.addNode(RGNode(25, 75, 20));
  rgInterface.addNode(RGNode(0, 50, 0));
  rgInterface.addNode(RGNode(25, 75, 0));
  rgInterface.addNode(RGNode(25, 25, 40));
  rgInterface.addNode(RGNode(25, 25, 0));
  rgInterface.addNode(RGNode(50, 0, 20));
  rgInterface.addNode(RGNode(0, 25, 20));
  rgInterface.addNode(RGNode(25, 0, 20));
  rgInterface.addNode(RGNode(25, 0, 0));
  rgInterface.addNode(RGNode(0, 50, 20));
  rgInterface.addNode(RGNode(50, 25, 40));
  rgInterface.addNode(RGNode(75, 50, 40));
  rgInterface.addNode(RGNode(0, 25, 40));
  rgInterface.addNode(RGNode(0, 50, 40));
  rgInterface.addNode(RGNode(75, 25, 40));
  rgInterface.addNode(RGNode(0, 0, 20));
  rgInterface.addNode(RGNode(50, 0, 0));
  rgInterface.addNode(RGNode(75, 0, 20));
  rgInterface.addNode(RGNode(50, 75, 0));
  rgInterface.addNode(RGNode(0, 25, 0));
  rgInterface.addNode(RGNode(75, 0, 0));
  rgInterface.addNode(RGNode(0, 0, 0));
  rgInterface.addNode(RGNode(25, 0, 60));
  rgInterface.addNode(RGNode(37.8389891316, 11.8529811615, 40.0826708316));
  rgInterface.addNode(RGNode(50, 0, 40));
  rgInterface.addNode(RGNode(50, 0, 60));
  rgInterface.addNode(RGNode(75, 0, 60));
  rgInterface.addNode(RGNode(25, 0, 40));
  rgInterface.addNode(RGNode(0, 0, 60));
  rgInterface.addNode(RGNode(0, 0, 40));
  rgInterface.addNode(RGNode(75, 0, 40));
  rgInterface.addNode(RGNode(50, 75, 40));
  rgInterface.addNode(RGNode(37.1299907708, 63.3980319784, 39.8886862088));
  rgInterface.addNode(RGNode(75, 75, 60));
  rgInterface.addNode(RGNode(50, 75, 60));
  rgInterface.addNode(RGNode(25, 75, 60));
  rgInterface.addNode(RGNode(25, 75, 40));
  rgInterface.addNode(RGNode(75, 75, 40));
  rgInterface.addNode(RGNode(0, 75, 40));
  rgInterface.addNode(RGNode(0, 75, 60));
  rgInterface.addNode(RGNode(37.4987861522, 37.4987861522, 59.9987861522));
  rgInterface.addNode(RGNode(50, 25, 80));
  rgInterface.addNode(RGNode(18.8909185256, 19.3044182105, 63.9636894856));
  rgInterface.addNode(RGNode(0, 25, 60));
  rgInterface.addNode(RGNode(0, 25, 80));
  rgInterface.addNode(RGNode(25, 50, 80));
  rgInterface.addNode(RGNode(25, 25, 80));
  rgInterface.addNode(RGNode(25, 0, 80));
  rgInterface.addNode(RGNode(50, 0, 80));
  rgInterface.addNode(RGNode(75, 25, 80));
  rgInterface.addNode(RGNode(0, 50, 80));
  rgInterface.addNode(RGNode(50, 50, 80));
  rgInterface.addNode(RGNode(75, 50, 60));
  rgInterface.addNode(RGNode(75, 25, 60));
  rgInterface.addNode(RGNode(75, 50, 80));
  rgInterface.addNode(RGNode(0, 50, 60));
  rgInterface.addNode(RGNode(50, 75, 80));
  rgInterface.addNode(RGNode(0, 0, 80));
  rgInterface.addNode(RGNode(75, 0, 80));
  rgInterface.addNode(RGNode(75, 75, 80));
  rgInterface.addNode(RGNode(0, 75, 80));
  rgInterface.addNode(RGNode(25, 75, 80));
}

void CTestRGInterface8::createElements(RGInterface& rgInterface)
{
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (5, 6, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 11, 8, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 11, 12, 13)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 16, 17)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 16, 18, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 13, 19)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 3, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 3, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 21, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 22, 23)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 21, 24, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 16, 17, 25)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 21, 23, 24)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 26, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 11, 13)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 22, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 16, 25, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 19, 23, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 26, 30, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 13, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 22, 23, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 22, 28, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 32, 33, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 21, 23, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 33, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 18, 34, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 32, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 21, 23)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 3, 32, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 22, 28, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 25, 35, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 11, 12, 16)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 11, 16, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 16, 25)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 16, 25, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 16, 17, 18)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 13, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 34, 8, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 21, 26, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 27, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 3, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 11, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 33, 36, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 32, 36, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 22, 25)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 22, 25, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 18, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 19, 22, 23)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 22, 25, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 25, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 22, 35, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 23, 24, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 6, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 22, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 34, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 5, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 21, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 22, 35, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 24, 3, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 25, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 26, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 27, 30, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 22, 23, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 23, 31, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 19, 22)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 25, 35, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 20, 8, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 26, 27, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 19, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 26, 38, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 30, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 26, 30, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 40, 41, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 39, 41, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 38, 39, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 38, 44, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 40, 41, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 23, 26, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 40, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 23, 31, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 28, 31, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 40, 42, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 30, 33, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 26, 39, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 23, 39, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 39, 40, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 39, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 38, 43, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 31, 43, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 23, 26, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 38, 39, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 28, 44, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 13, 47, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 47, 49, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 50, 51, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 16, 48, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 47, 49, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 47, 48, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 16, 47, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 47, 48, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 16, 29, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 16, 29, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 27, 49, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 48, 50, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 51, 52, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 29, 52, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 13, 47, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 13, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 48, 50, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 12, 16, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 13, 53, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 51, 54, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 27, 53, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 13, 48, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 51, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 41, 56, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 58, 59, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (58, 59, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 58, 61, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 38, 41, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (57, 58, 62, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 57, 58, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 44, 58, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 57, 64, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 38, 56, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 60, 61, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 58, 59, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 57, 62, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 57, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (57, 68, 69, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 57, 63, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 61, 66, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 51, 56, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 51, 61, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (58, 60, 62, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 26, 56, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 49, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 13, 50, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 56, 68, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 26, 27, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 42, 65, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 30, 41, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 28, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 29, 51, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 19, 56, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 38, 44, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 28, 59, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 56, 58, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 64, 65, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 57, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 26, 38, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 41, 42, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (58, 60, 61, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 56, 61, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (57, 67, 68, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 58, 60, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 28, 44, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 44, 58, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 51, 56, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 61, 67, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (29, 51, 55, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 67, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (57, 65, 69, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 42, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 56, 57, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 57, 68, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 56, 57, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 41, 57, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (55, 71, 76, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 30, 68, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 61, 62, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 27, 30, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 41, 56, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (61, 66, 71, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (56, 59, 61, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (66, 71, 76, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (51, 56, 61, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 58, 63, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (67, 68, 70, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (51, 55, 71, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 56, 59, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (51, 61, 71, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (58, 62, 63, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 67, 72, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 50, 51, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 19, 26, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 56, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 50, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 27, 49, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 57, 58, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 13, 19, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 50, 56, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (51, 61, 72, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 28, 29, 71)));
}

void CTestRGInterface8::createProperties(RGInterface& rgInterface)
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
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(3);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);
  elementFormation.push_back(4);

  rgInterface.saveProperty(RGPropertyType::propElementFormation,
  elementFormation);
}

namespace
{

const std::string FORMATION_0 = "Formation0";
const std::string FORMATION_1 = "Formation1";
const std::string FORMATION_2 = "Formation2";
const std::string FORMATION_3 = "Formation3";

} // anonymous namespace

void CTestRGInterface8::createFormationNames(RGInterface& rgInterface)
{
  rgInterface.setFormationName(1, FORMATION_0);
  rgInterface.setFormationName(2, FORMATION_1);
  rgInterface.setFormationName(3, FORMATION_2);
  rgInterface.setFormationName(4, FORMATION_3);
}
