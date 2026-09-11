#include "TestRGInterface.4.h"
#include "RGInterface.h"

CTestRGInterface4::CTestRGInterface4(const QString& modelName)
: ITestRGInterface()
{
  createTest(modelName);
}

namespace
{

const size_t ANGLE = 45;
const double MINUS_ONE = -1.0;

RGNode rotateY(double x, double y, double z)
{
  double pi = std::acos(MINUS_ONE);
  double sinus = std::sin((ANGLE * pi) / 180);
  double cosinus = std::cos((ANGLE * pi) / 180);
  double rotatedX = (cosinus * x) + (sinus * z);
  double rotatedY = y;
  double rotatedZ = (-sinus * x) + (cosinus * z);

  return RGNode(rotatedX, rotatedY, rotatedZ);
}

} // anonymous namespace

void CTestRGInterface4::createNodes(RGInterface& rgInterface)
{
  rgInterface.addNode(rotateY(37.5006050101, 49.9983815363, 24.9995915564));
  rgInterface.addNode(rotateY(24.9995915564, 37.5006050101, 49.9983815363));
  rgInterface.addNode(rotateY(21.5879771419, 63.1687700752, 51.7218004169));
  rgInterface.addNode(rotateY(49.9983815363, 49.9983815363, 49.9983815363));
  rgInterface.addNode(rotateY(62.5429154694, 49.7436424314, 24.1418299848));
  rgInterface.addNode(rotateY(63.4761952318, 74.1045055969, 37.7610631422));
  rgInterface.addNode(rotateY(50, 25, 0));
  rgInterface.addNode(rotateY(50, 50, 0));
  rgInterface.addNode(rotateY(75, 75, 0));
  rgInterface.addNode(rotateY(50, 75, 0));
  rgInterface.addNode(rotateY(74.7965787662, 36.9875711661, 37.0400598134));
  rgInterface.addNode(rotateY(75, 25, 0));
  rgInterface.addNode(rotateY(37.5006050101, 24.9995915564, 37.5006050101));
  rgInterface.addNode(rotateY(25, 25, 0));
  rgInterface.addNode(rotateY(0, 25, 25));
  rgInterface.addNode(rotateY(100, 50, 25));
  rgInterface.addNode(rotateY(75, 50, 0));
  rgInterface.addNode(rotateY(37.5006050101, 75.0004084436, 37.5006050101));
  rgInterface.addNode(rotateY(100, 100, 25));
  rgInterface.addNode(rotateY(100, 75, 0));
  rgInterface.addNode(rotateY(100, 75, 25));
  rgInterface.addNode(rotateY(63.0944133701, 23.4624019485, 51.9914269042));
  rgInterface.addNode(rotateY(76.8604615556, 50.1003833966, 63.9412536694));
  rgInterface.addNode(rotateY(50, 100, 50));
  rgInterface.addNode(rotateY(61.7987406434, 73.0138377169, 65.1440759622));
  rgInterface.addNode(rotateY(50, 100, 25));
  rgInterface.addNode(rotateY(37.5006050101, 24.9995915564, 62.4993949899));
  rgInterface.addNode(rotateY(50, 0, 50));
  rgInterface.addNode(rotateY(25, 0, 50));
  rgInterface.addNode(rotateY(62.4993949899, 37.5006050101, 75.0004084436));
  rgInterface.addNode(rotateY(50, 0, 75));
  rgInterface.addNode(rotateY(50, 25, 100));
  rgInterface.addNode(rotateY(100, 25, 75));
  rgInterface.addNode(rotateY(75, 50, 100));
  rgInterface.addNode(rotateY(100, 75, 75));
  rgInterface.addNode(rotateY(75, 75, 100));
  rgInterface.addNode(rotateY(100, 75, 50));
  rgInterface.addNode(rotateY(100, 50, 50));
  rgInterface.addNode(rotateY(100, 25, 50));
  rgInterface.addNode(rotateY(75, 100, 50));
  rgInterface.addNode(rotateY(50, 50, 100));
  rgInterface.addNode(rotateY(38.0499367586, 64.397347309, 72.6947602767));
  rgInterface.addNode(rotateY(0, 50, 25));
  rgInterface.addNode(rotateY(0, 75, 25));
  rgInterface.addNode(rotateY(75, 100, 100));
  rgInterface.addNode(rotateY(100, 75, 100));
  rgInterface.addNode(rotateY(100, 100, 100));
  rgInterface.addNode(rotateY(0, 25, 75));
  rgInterface.addNode(rotateY(25, 50, 100));
  rgInterface.addNode(rotateY(25, 75, 0));
  rgInterface.addNode(rotateY(0, 50, 75));
  rgInterface.addNode(rotateY(0, 50, 50));
  rgInterface.addNode(rotateY(25, 100, 50));
  rgInterface.addNode(rotateY(100, 50, 75));
  rgInterface.addNode(rotateY(75, 100, 25));
  rgInterface.addNode(rotateY(50, 100, 75));
  rgInterface.addNode(rotateY(0, 25, 50));
  rgInterface.addNode(rotateY(25, 100, 25));
  rgInterface.addNode(rotateY(0, 75, 75));
  rgInterface.addNode(rotateY(25, 100, 75));
  rgInterface.addNode(rotateY(0, 75, 50));
  rgInterface.addNode(rotateY(25, 50, 0));
  rgInterface.addNode(rotateY(75, 100, 75));
  rgInterface.addNode(rotateY(50, 75, 100));
  rgInterface.addNode(rotateY(25, 25, 100));
  rgInterface.addNode(rotateY(25, 0, 75));
  rgInterface.addNode(rotateY(75, 0, 75));
  rgInterface.addNode(rotateY(0, 75, 100));
  rgInterface.addNode(rotateY(0, 50, 100));
  rgInterface.addNode(rotateY(25, 0, 25));
  rgInterface.addNode(rotateY(50, 0, 25));
  rgInterface.addNode(rotateY(75, 0, 25));
  rgInterface.addNode(rotateY(75, 0, 50));
  rgInterface.addNode(rotateY(100, 25, 25));
  rgInterface.addNode(rotateY(0, 100, 0));
  rgInterface.addNode(rotateY(25, 100, 0));
  rgInterface.addNode(rotateY(0, 75, 0));
  rgInterface.addNode(rotateY(100, 0, 25));
  rgInterface.addNode(rotateY(75, 0, 0));
  rgInterface.addNode(rotateY(50, 100, 0));
  rgInterface.addNode(rotateY(75, 100, 0));
  rgInterface.addNode(rotateY(100, 25, 0));
  rgInterface.addNode(rotateY(100, 50, 0));
  rgInterface.addNode(rotateY(100, 0, 0));
  rgInterface.addNode(rotateY(100, 50, 100));
  rgInterface.addNode(rotateY(0, 25, 0));
  rgInterface.addNode(rotateY(25, 0, 0));
  rgInterface.addNode(rotateY(0, 50, 0));
  rgInterface.addNode(rotateY(50, 0, 0));
  rgInterface.addNode(rotateY(75, 25, 100));
  rgInterface.addNode(rotateY(50, 0, 100));
  rgInterface.addNode(rotateY(25, 75, 100));
  rgInterface.addNode(rotateY(25, 100, 100));
  rgInterface.addNode(rotateY(0, 100, 100));
  rgInterface.addNode(rotateY(100, 0, 75));
  rgInterface.addNode(rotateY(100, 0, 100));
  rgInterface.addNode(rotateY(75, 0, 100));
  rgInterface.addNode(rotateY(0, 0, 100));
  rgInterface.addNode(rotateY(25, 0, 100));
  rgInterface.addNode(rotateY(0, 25, 100));
  rgInterface.addNode(rotateY(0, 0, 75));
  rgInterface.addNode(rotateY(100, 0, 50));
  rgInterface.addNode(rotateY(100, 25, 100));
  rgInterface.addNode(rotateY(0, 0, 50));
  rgInterface.addNode(rotateY(0, 0, 25));
  rgInterface.addNode(rotateY(100, 100, 75));
  rgInterface.addNode(rotateY(50, 100, 100));
  rgInterface.addNode(rotateY(0, 100, 50));
  rgInterface.addNode(rotateY(0, 100, 75));
  rgInterface.addNode(rotateY(100, 100, 50));
  rgInterface.addNode(rotateY(0, 100, 25));
  rgInterface.addNode(rotateY(0, 0, 0));
  rgInterface.addNode(rotateY(100, 100, 0));
  rgInterface.addNode(rotateY(24.6413288273, 35.3306051167, 162.438830247));
  rgInterface.addNode(rotateY(0, 25, 125));
  rgInterface.addNode(rotateY(25, 0, 150));
  rgInterface.addNode(rotateY(37.2042374946, 23.104690867, 150.830130978));
  rgInterface.addNode(rotateY(49.9983815363, 37.5006050101, 175.000408444));
  rgInterface.addNode(rotateY(49.9983815363, 49.9983815363, 149.998381536));
  rgInterface.addNode(rotateY(37.5006050101, 37.5006050101, 124.999591556));
  rgInterface.addNode(rotateY(50, 0, 125));
  rgInterface.addNode(rotateY(61.7243920292, 24.2994007024, 150.396041886));
  rgInterface.addNode(rotateY(75.6821115345, 37.5684019181, 162.742727191));
  rgInterface.addNode(rotateY(75, 0, 175));
  rgInterface.addNode(rotateY(75, 25, 200));
  rgInterface.addNode(rotateY(75, 50, 200));
  rgInterface.addNode(rotateY(37.1155848624, 63.3710752401, 175.460464569));
  rgInterface.addNode(rotateY(63.424175939, 63.8177795688, 174.528949346));
  rgInterface.addNode(rotateY(51.6514625576, 76.4377686206, 161.887843177));
  rgInterface.addNode(rotateY(25, 100, 175));
  rgInterface.addNode(rotateY(24.9995915564, 62.4993949899, 149.998381536));
  rgInterface.addNode(rotateY(0, 75, 175));
  rgInterface.addNode(rotateY(0, 50, 175));
  rgInterface.addNode(rotateY(50, 25, 200));
  rgInterface.addNode(rotateY(25, 0, 175));
  rgInterface.addNode(rotateY(25, 25, 200));
  rgInterface.addNode(rotateY(50, 50, 200));
  rgInterface.addNode(rotateY(50, 75, 200));
  rgInterface.addNode(rotateY(0, 25, 175));
  rgInterface.addNode(rotateY(25, 50, 200));
  rgInterface.addNode(rotateY(50, 100, 175));
  rgInterface.addNode(rotateY(25, 100, 150));
  rgInterface.addNode(rotateY(38.8719484567, 77.4181060473, 135.242543365));
  rgInterface.addNode(rotateY(50, 100, 150));
  rgInterface.addNode(rotateY(75, 100, 175));
  rgInterface.addNode(rotateY(75.0004084436, 62.4993949899, 137.50060501));
  rgInterface.addNode(rotateY(75, 0, 150));
  rgInterface.addNode(rotateY(75.0004084436, 37.5006050101, 137.50060501));
  rgInterface.addNode(rotateY(75, 0, 125));
  rgInterface.addNode(rotateY(50, 0, 150));
  rgInterface.addNode(rotateY(0, 50, 125));
  rgInterface.addNode(rotateY(0, 75, 125));
  rgInterface.addNode(rotateY(49.1116397933, 62.3656258907, 124.592314871));
  rgInterface.addNode(rotateY(100, 50, 125));
  rgInterface.addNode(rotateY(100, 50, 150));
  rgInterface.addNode(rotateY(100, 25, 150));
  rgInterface.addNode(rotateY(75, 100, 200));
  rgInterface.addNode(rotateY(100, 75, 175));
  rgInterface.addNode(rotateY(100, 75, 200));
  rgInterface.addNode(rotateY(100, 100, 200));
  rgInterface.addNode(rotateY(0, 50, 150));
  rgInterface.addNode(rotateY(50, 100, 125));
  rgInterface.addNode(rotateY(100, 50, 175));
  rgInterface.addNode(rotateY(0, 75, 150));
  rgInterface.addNode(rotateY(100, 75, 150));
  rgInterface.addNode(rotateY(100, 0, 125));
  rgInterface.addNode(rotateY(25, 0, 125));
  rgInterface.addNode(rotateY(50, 0, 175));
  rgInterface.addNode(rotateY(0, 25, 150));
  rgInterface.addNode(rotateY(25, 100, 125));
  rgInterface.addNode(rotateY(0, 50, 200));
  rgInterface.addNode(rotateY(0, 75, 200));
  rgInterface.addNode(rotateY(75, 75, 200));
  rgInterface.addNode(rotateY(75, 100, 150));
  rgInterface.addNode(rotateY(100, 100, 125));
  rgInterface.addNode(rotateY(100, 25, 125));
  rgInterface.addNode(rotateY(25, 75, 200));
  rgInterface.addNode(rotateY(25, 100, 200));
  rgInterface.addNode(rotateY(0, 100, 200));
  rgInterface.addNode(rotateY(50, 0, 200));
  rgInterface.addNode(rotateY(100, 25, 175));
  rgInterface.addNode(rotateY(100, 0, 200));
  rgInterface.addNode(rotateY(100, 0, 175));
  rgInterface.addNode(rotateY(25, 0, 200));
  rgInterface.addNode(rotateY(0, 25, 200));
  rgInterface.addNode(rotateY(100, 100, 150));
  rgInterface.addNode(rotateY(100, 75, 125));
  rgInterface.addNode(rotateY(75, 100, 125));
  rgInterface.addNode(rotateY(75, 0, 200));
  rgInterface.addNode(rotateY(100, 0, 150));
  rgInterface.addNode(rotateY(0, 0, 150));
  rgInterface.addNode(rotateY(0, 0, 200));
  rgInterface.addNode(rotateY(0, 0, 175));
  rgInterface.addNode(rotateY(100, 25, 200));
  rgInterface.addNode(rotateY(100, 50, 200));
  rgInterface.addNode(rotateY(0, 100, 175));
  rgInterface.addNode(rotateY(0, 0, 125));
  rgInterface.addNode(rotateY(100, 100, 175));
  rgInterface.addNode(rotateY(50, 100, 200));
  rgInterface.addNode(rotateY(0, 100, 150));
  rgInterface.addNode(rotateY(0, 100, 125));
}

void CTestRGInterface4::createElements(RGInterface& rgInterface)
{
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 4, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 5, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 5, 6, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 14, 15)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 17, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 18, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 20, 21, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 22, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 4, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 23, 4, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 25, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 18, 26, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 22, 27, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 2, 27, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 27, 28, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 30, 31, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 30, 33, 34)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 35, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 22, 23, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 35, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 30, 34)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 4, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 37, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 23, 38, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 37, 38, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 23, 30, 33)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 37, 40, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 4, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 15, 2, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 30, 41, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 2, 27, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 15, 2, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 2, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 18, 3, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 45, 46, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 14, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 27, 29, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 5, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 27, 28, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 27, 42, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 18, 44, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 4, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 51, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 43, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 22, 23, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 30, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 41, 42, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 30, 41, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 3, 42, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 25, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 17, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 25, 4, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 18, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 17, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 35, 38, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 37, 38, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 55, 6, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 17, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 37, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 40, 55, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 25, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 3, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 26, 40, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 25, 42, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 2, 52, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 26, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 18, 26, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 44, 50, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 42, 59, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 2, 43, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 44, 53, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 42, 53, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 43, 44, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 48, 51, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 35, 36, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 42, 56, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 48, 49, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 23, 33, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 49, 51, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 52, 59, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 43, 52, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 42, 53, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 53, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 32, 41, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 34, 36, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 34, 41, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 29, 31, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 42, 49, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 30, 33, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 27, 30, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 51, 68, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 28, 29, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 17, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 5, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 71, 72, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 28, 70, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 30, 32, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 38, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 38, 39, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 7, 71, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 58, 75, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 50, 58, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (50, 58, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 17, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 72, 78, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 6, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 18, 4, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 50, 62, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 18, 50, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 50, 58, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 55, 80, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 7, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 7, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 55, 81, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 26, 55, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 50, 62, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 55, 6, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 82, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 14, 7, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 78, 79, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 74, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 62, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 46, 54, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 43, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 26, 76, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 43, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 70, 71, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 43, 86, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 71, 87, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 22, 28, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 32, 41, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 65, 66, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (48, 49, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 33, 67, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 32, 49, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 42, 49, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 51, 59, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 51, 52, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 64, 92, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 92, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 30, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 30, 31, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 33, 34, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 42, 64, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (67, 90, 95, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 36, 56, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 31, 67, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (31, 67, 90, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 28, 31, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 28, 67, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 67, 73, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 22, 71, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 28, 71, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (7, 72, 79, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 12, 72, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 72, 74, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 13, 22, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 34, 36, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (31, 32, 66, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 31, 32, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (31, 32, 91, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 28, 29, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 101, 98, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (31, 32, 90, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 48, 49, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 39, 72, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 33, 90, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 72, 74, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 22, 39, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 21, 83, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 29, 48, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 29, 48, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 33, 39, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 31, 32, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 22, 27, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 7, 71, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 15, 29, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (15, 2, 29, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 105, 29, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (7, 71, 72, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 86, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 14, 15, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 45, 47, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 45, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 37, 40, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 25, 56, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 37, 40, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (55, 80, 81, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 20, 81, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 26, 55, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 36, 45, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 60, 64, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 25, 40, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 56, 60, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 46, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 30, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 59, 60, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 53, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (19, 21, 55, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 44, 50, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 60, 64, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 53, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 60, 92, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 109, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 15, 2)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 37, 40, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (109, 59, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 59, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 21, 37, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 111, 44, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 58, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 40, 55, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 44, 58, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 45, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (109, 59, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (59, 68, 92, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 27, 48, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 3, 43, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (109, 59, 60, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 43, 44, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (44, 62, 77, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 62, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 15, 29, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 36, 56, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 41, 42, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (48, 49, 51, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 27, 4, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 43, 62, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 51, 52, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 48, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 42, 51, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 101, 48, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 104, 48, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 101, 66, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 29, 48, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 15, 70, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 65, 66, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 27, 30, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 15, 43)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 3, 44, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 35, 37, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 53, 56, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 37, 55, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 19, 21, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 59, 68, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 19, 20, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 26, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 45, 56, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 34, 54, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 36, 54, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 48, 65, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 33, 34, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 39, 72, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 34, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 23, 30, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 33, 73, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 90, 95, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 33, 39, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 67, 90, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 33, 39, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (67, 90, 96, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 39, 72, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 33, 67, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 5, 6, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 74, 78, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 17, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 33, 34, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 32, 65, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 35, 36, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 7, 72, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 15, 29, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 112, 15, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 38, 39, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (27, 48, 65, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 15, 86, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 25, 40, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 36, 56, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 44, 53, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 56, 60, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (65, 66, 91, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 44, 53, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (43, 44, 62, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 24, 42, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 50, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 5, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 35, 47, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (42, 49, 59, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 17, 74, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 44, 53, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 26, 58, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 34, 41, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 23, 38, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 21, 83, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 15, 70, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 29, 48, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 42, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (25, 30, 34, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 17, 83, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 16, 38, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 35, 37, 63)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 18, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (30, 33, 34, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 39, 72, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 35, 37, 38)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 14, 70, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 110, 37, 40)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (12, 74, 82, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 116, 117)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 118, 119)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 120, 121, 122)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 117, 120)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 119, 120)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 123, 124, 125)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 123, 125, 126)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 118, 119, 122)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 118, 119, 127)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 123, 126, 128)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 129, 130, 131)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 119, 127, 128)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 131, 132, 133)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 119, 127, 131)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 127, 131, 133)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 134, 135, 136)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 128, 137, 138)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 127, 129, 131)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 136, 139, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 127, 133, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 118, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 128, 138, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 142, 143, 144)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 129, 141, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 131, 142, 143)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 129, 143, 146)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 128, 129, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 129, 131, 143)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 123, 147, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 147, 148, 149)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 121, 122, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 122, 123, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 148, 149)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 121, 122, 150)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (151, 152, 153, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 148, 154, 155)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 148, 155, 156)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 153, 41, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 148, 32, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (157, 158, 159, 160)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 148, 153, 34)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 153, 34, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 131, 151, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 120, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 148, 154, 34)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 153, 64, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 146, 153, 162)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (152, 153, 49, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 131, 143, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 151, 152, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 123, 128, 146)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 118, 135, 136)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 118, 136, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 126, 128, 137)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 123, 124, 147)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 147, 148, 156)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 126, 128, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 130, 131, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 142, 143, 152)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 142, 152, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 129, 146, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 143, 144, 146)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 120, 131, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (166, 90, 96, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 120, 121, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 119, 122, 123)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 127, 128, 129)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 129, 145, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 118, 135, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 146, 155, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 143, 152, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 151, 153, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 161, 169)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 153, 162, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 144, 162, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 141, 142, 144)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 130, 138, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 120, 151, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 118, 122, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 154, 34, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 146, 148, 155)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 140, 171, 172)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 122, 124, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 145, 158, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 162, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 151, 161, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 141, 144, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 120, 148, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 155, 158, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 45, 46, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (153, 162, 36, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (153, 34, 36, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 125, 134, 137)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 124, 134, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 125, 126, 137)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 117, 150, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 120, 122, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 146, 148, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 121, 150, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 123, 146, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 153, 34, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 154, 155, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 155, 156, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 121, 148, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (152, 49, 68, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 144, 146, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 137, 138, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 118, 127, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 133, 140, 172)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 178, 179)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 139, 140, 171)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 135, 136, 180)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 122, 150, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 127, 128, 137)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 125, 126, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 182, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 128, 137, 138)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 136, 180, 184)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 136, 139, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 32, 49, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 115, 151, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 120, 151, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 121, 32, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (153, 41, 49, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 174, 186, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 32, 65, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (170, 64, 92, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (152, 49, 68, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (170, 92, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 167, 65, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 148, 153, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 153, 162, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 167, 65, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (153, 34, 41, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 154, 34, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (154, 34, 36, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (153, 41, 64, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 162, 188, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (187, 188, 36, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (154, 187, 36, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 154, 187, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 162, 188, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 134, 168, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 124, 150, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 147, 149)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 148, 149, 156)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 149, 156, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 131, 133, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (142, 143, 144, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 166, 176, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 32, 41, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 141, 145, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 134, 137, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 134, 136, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 134, 135, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 124, 147, 150)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 147, 181, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 181, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 136, 184, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 149, 32, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 156, 166, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 121, 167, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 147, 150)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 148, 149, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 128, 138, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 124, 125, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 117, 150, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 135, 139, 191)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (184, 185, 192, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 117, 135, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 156, 181, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 181, 194, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 178, 179, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 167, 98, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 169, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 167, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 169, 191, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 176, 90, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 167, 197, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (145, 157, 158, 160)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (157, 158, 159, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 145, 165, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 128, 129, 146)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (145, 165, 174, 198)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (174, 186, 187, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 174, 187, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 188, 36, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 32, 90, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 162, 36, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 187, 188, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (141, 157, 173, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 138, 178, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 144, 145, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (158, 159, 163, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 138, 141, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 138, 141, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 162, 64, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 162, 170, 64)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 144, 146, 162)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 132, 133, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 142, 164, 200)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 154, 34, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 132, 177, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 164, 196, 200)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 162, 174, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (170, 201, 92, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 152, 170, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 129, 130, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 132, 164, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 132, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (152, 170, 201, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (142, 152, 200, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (152, 201, 68, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 149, 176, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (142, 143, 152, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (142, 152, 170, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (145, 157, 158, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 130, 131, 132)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 172, 177, 179)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 151, 152, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 152, 153, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 132, 178, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (162, 170, 64, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (138, 141, 173, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (151, 152, 49, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 120, 131, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 130, 132, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (201, 68, 92, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 133, 139, 169)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 139, 140, 171)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 117, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 151, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 133, 161, 169)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 137, 140, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 139, 171, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 139, 185, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 139, 191, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 184, 185, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 116, 135, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 116, 169)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 65, 91, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 130, 138, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 168, 180, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 119, 120, 131)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 149, 32, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 119, 120, 122)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 154, 176, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 145, 158, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 130, 141, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 165, 174, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 186, 187, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 187, 188, 36)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 140, 172, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 117, 120, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 154, 165, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (187, 36, 46, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (141, 145, 157, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 126, 181, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 163, 173, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 124, 125, 134)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 163, 181, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (187, 36, 45, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 128, 163, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 128, 155, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 34, 41, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 181, 183, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 32, 91, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 183, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 156, 163, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 120, 167, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 126, 163, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 147, 156, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 182, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 122, 123, 124)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 149, 156, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 124, 147, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (166, 176, 90, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (119, 143, 146, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 133, 139, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 181, 183, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 135, 136, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 154, 34, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 158, 163, 173)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 131, 151, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 154, 155, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 116, 117, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 156, 166, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 155, 156, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (159, 163, 173, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 138, 177, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 32, 41, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 146, 165, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 134, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (145, 158, 165, 198)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 151, 49, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (151, 49, 65, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 132, 161, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (145, 158, 160, 198)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 132, 140, 177)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 115, 65, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 166, 90, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (123, 128, 146, 155)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 154, 176, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 187, 45, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 135, 168, 180)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 119, 123, 128)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 116, 139, 169)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 142, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 132, 133, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 170, 64, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 127, 137, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 155, 158, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (142, 152, 164, 200)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 115, 98, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 174, 186, 198)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (116, 139, 169, 191)));
}

void CTestRGInterface4::createProperties(RGInterface& rgInterface)
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

  rgInterface.saveProperty(RGPropertyType::propElementFormation,
  elementFormation);
}

namespace
{

const std::string FORMATION_0 = "Constant_depth_at_0_m";
const std::string FORMATION_1 = "Constant_depth_at_100_m";

} // anonymous namespace

void CTestRGInterface4::createFormationNames(RGInterface& rgInterface)
{
  rgInterface.setFormationName(1, FORMATION_0);
  rgInterface.setFormationName(2, FORMATION_1);
}
