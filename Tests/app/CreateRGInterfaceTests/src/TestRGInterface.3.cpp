#include "TestRGInterface.3.h"
#include "RGInterface.h"

CTestRGInterface3::CTestRGInterface3(const QString& modelName)
: ITestRGInterface()
{
  createTest(modelName);
}

namespace
{

const size_t ANGLE = 45;
const double MINUS_ONE = -1.0;

RGNode rotateX(double x, double y, double z)
{
  double pi = std::acos(MINUS_ONE);
  double sinus = std::sin((ANGLE * pi) / 180);
  double cosinus = std::cos((ANGLE * pi) / 180);
  double rotatedX = x;
  double rotatedY = (cosinus * y) + (-sinus * z);
  double rotatedZ = (sinus * y) + (cosinus * z);

  return RGNode(rotatedX, rotatedY, rotatedZ);
}

} // anonymous namespace

void CTestRGInterface3::createNodes(RGInterface& rgInterface)
{
  rgInterface.addNode(rotateX(87.5014218972, 87.5014218972, 81.2488882631));
  rgInterface.addNode(rotateX(82.1119378474, 61.9264588477, 75.0016184637));
  rgInterface.addNode(rotateX(75, 75, 50));
  rgInterface.addNode(rotateX(62.4993949899, 75.0004084436, 75.0016184637));
  rgInterface.addNode(rotateX(75, 100, 87.5));
  rgInterface.addNode(rotateX(100, 75, 100));
  rgInterface.addNode(rotateX(75, 100, 100));
  rgInterface.addNode(rotateX(100, 100, 100));
  rgInterface.addNode(rotateX(75, 100, 62.5));
  rgInterface.addNode(rotateX(100, 100, 75));
  rgInterface.addNode(rotateX(100, 75, 62.5));
  rgInterface.addNode(rotateX(100, 100, 62.5));
  rgInterface.addNode(rotateX(75, 75, 100));
  rgInterface.addNode(rotateX(100, 75, 87.5));
  rgInterface.addNode(rotateX(100, 100, 87.5));
  rgInterface.addNode(rotateX(75, 100, 75));
  rgInterface.addNode(rotateX(0, 25, 87.5));
  rgInterface.addNode(rotateX(25, 0, 100));
  rgInterface.addNode(rotateX(0, 25, 100));
  rgInterface.addNode(rotateX(25, 25, 100));
  rgInterface.addNode(rotateX(12.4985781028, 12.4985781028, 81.2488882631));
  rgInterface.addNode(rotateX(25, 0, 87.5));
  rgInterface.addNode(rotateX(36.9244319405, 23.8472454173, 74.9983815363));
  rgInterface.addNode(rotateX(25, 25, 50));
  rgInterface.addNode(rotateX(25, 0, 62.5));
  rgInterface.addNode(rotateX(23.8472454173, 36.9244319405, 74.9983815363));
  rgInterface.addNode(rotateX(0, 25, 62.5));
  rgInterface.addNode(rotateX(0, 0, 62.5));
  rgInterface.addNode(rotateX(0, 0, 75));
  rgInterface.addNode(rotateX(0, 0, 87.5));
  rgInterface.addNode(rotateX(25, 0, 75));
  rgInterface.addNode(rotateX(100, 50, 62.5));
  rgInterface.addNode(rotateX(100, 50, 87.5));
  rgInterface.addNode(rotateX(13.1971276545, 86.4186760259, 79.0359578863));
  rgInterface.addNode(rotateX(37.572139739, 76.8202742449, 74.9588670599));
  rgInterface.addNode(rotateX(25, 75, 50));
  rgInterface.addNode(rotateX(23.3543627112, 62.7795424703, 73.1170128749));
  rgInterface.addNode(rotateX(0, 100, 87.5));
  rgInterface.addNode(rotateX(25, 75, 100));
  rgInterface.addNode(rotateX(0, 100, 100));
  rgInterface.addNode(rotateX(25, 100, 100));
  rgInterface.addNode(rotateX(0, 75, 62.5));
  rgInterface.addNode(rotateX(25, 100, 75));
  rgInterface.addNode(rotateX(0, 100, 62.5));
  rgInterface.addNode(rotateX(25, 100, 62.5));
  rgInterface.addNode(rotateX(0, 100, 75));
  rgInterface.addNode(rotateX(25, 100, 87.5));
  rgInterface.addNode(rotateX(50, 75, 50));
  rgInterface.addNode(rotateX(48.058491354, 49.7911694726, 74.9318703654));
  rgInterface.addNode(rotateX(50, 75, 100));
  rgInterface.addNode(rotateX(50, 100, 87.5));
  rgInterface.addNode(rotateX(50, 100, 62.5));
  rgInterface.addNode(rotateX(50, 100, 75));
  rgInterface.addNode(rotateX(75, 50, 50));
  rgInterface.addNode(rotateX(0, 50, 62.5));
  rgInterface.addNode(rotateX(25, 50, 50));
  rgInterface.addNode(rotateX(0, 75, 87.5));
  rgInterface.addNode(rotateX(0, 75, 75));
  rgInterface.addNode(rotateX(50, 100, 100));
  rgInterface.addNode(rotateX(59.0463613335, 32.884063301, 71.4307864405));
  rgInterface.addNode(rotateX(50, 50, 50));
  rgInterface.addNode(rotateX(50, 25, 50));
  rgInterface.addNode(rotateX(87.5014218972, 12.4985781028, 75.0016184637));
  rgInterface.addNode(rotateX(100, 25, 75));
  rgInterface.addNode(rotateX(100, 25, 62.5));
  rgInterface.addNode(rotateX(100, 0, 62.5));
  rgInterface.addNode(rotateX(25, 50, 100));
  rgInterface.addNode(rotateX(50, 50, 100));
  rgInterface.addNode(rotateX(50, 0, 75));
  rgInterface.addNode(rotateX(66.1878476167, 14.6671405417, 67.8803533646));
  rgInterface.addNode(rotateX(50, 0, 87.5));
  rgInterface.addNode(rotateX(75, 0, 62.5));
  rgInterface.addNode(rotateX(75.6418642021, 37.6743873554, 74.7628582456));
  rgInterface.addNode(rotateX(75, 25, 50));
  rgInterface.addNode(rotateX(50, 25, 100));
  rgInterface.addNode(rotateX(100, 0, 75));
  rgInterface.addNode(rotateX(75, 25, 100));
  rgInterface.addNode(rotateX(75, 0, 75));
  rgInterface.addNode(rotateX(100, 0, 87.5));
  rgInterface.addNode(rotateX(0, 50, 100));
  rgInterface.addNode(rotateX(0, 50, 87.5));
  rgInterface.addNode(rotateX(75, 50, 100));
  rgInterface.addNode(rotateX(100, 25, 87.5));
  rgInterface.addNode(rotateX(100, 50, 75));
  rgInterface.addNode(rotateX(0, 75, 100));
  rgInterface.addNode(rotateX(50, 0, 62.5));
  rgInterface.addNode(rotateX(75, 0, 87.5));
  rgInterface.addNode(rotateX(50, 0, 50));
  rgInterface.addNode(rotateX(75, 0, 50));
  rgInterface.addNode(rotateX(0, 75, 50));
  rgInterface.addNode(rotateX(0, 100, 50));
  rgInterface.addNode(rotateX(25, 100, 50));
  rgInterface.addNode(rotateX(75, 0, 100));
  rgInterface.addNode(rotateX(100, 0, 100));
  rgInterface.addNode(rotateX(50, 0, 100));
  rgInterface.addNode(rotateX(100, 75, 75));
  rgInterface.addNode(rotateX(0, 50, 75));
  rgInterface.addNode(rotateX(100, 50, 100));
  rgInterface.addNode(rotateX(50, 100, 50));
  rgInterface.addNode(rotateX(0, 50, 50));
  rgInterface.addNode(rotateX(100, 25, 50));
  rgInterface.addNode(rotateX(100, 50, 50));
  rgInterface.addNode(rotateX(0, 25, 75));
  rgInterface.addNode(rotateX(25, 0, 50));
  rgInterface.addNode(rotateX(0, 25, 50));
  rgInterface.addNode(rotateX(100, 0, 50));
  rgInterface.addNode(rotateX(75, 100, 50));
  rgInterface.addNode(rotateX(100, 75, 50));
  rgInterface.addNode(rotateX(100, 100, 50));
  rgInterface.addNode(rotateX(0, 0, 50));
  rgInterface.addNode(rotateX(100, 25, 100));
  rgInterface.addNode(rotateX(0, 0, 100));
  rgInterface.addNode(rotateX(88.0743580394, 36.9244319405, 136.92322192));
  rgInterface.addNode(rotateX(88.0743580394, 38.0767780796, 113.07677808));
  rgInterface.addNode(rotateX(86.7429302457, 13.6900390331, 128.89539884));
  rgInterface.addNode(rotateX(100, 25, 125));
  rgInterface.addNode(rotateX(73.8705719296, 38.5294058177, 124.181915299));
  rgInterface.addNode(rotateX(75, 25, 150));
  rgInterface.addNode(rotateX(100, 25, 150));
  rgInterface.addNode(rotateX(75, 50, 150));
  rgInterface.addNode(rotateX(100, 50, 125));
  rgInterface.addNode(rotateX(88.0743580394, 61.9232219204, 136.92322192));
  rgInterface.addNode(rotateX(100, 50, 150));
  rgInterface.addNode(rotateX(25.3168050156, 62.4700041714, 123.416301558));
  rgInterface.addNode(rotateX(38.6588421489, 75.6190080164, 125.531150234));
  rgInterface.addNode(rotateX(47.7345370323, 47.9380130112, 124.237038876));
  rgInterface.addNode(rotateX(36.8623209215, 87.2482861073, 113.997675497));
  rgInterface.addNode(rotateX(11.9765210463, 37.484287274, 114.188863968));
  rgInterface.addNode(rotateX(12.0478572995, 63.4543842126, 112.862393477));
  rgInterface.addNode(rotateX(0, 50, 125));
  rgInterface.addNode(rotateX(11.9256419606, 63.0755680595, 136.92322192));
  rgInterface.addNode(rotateX(13.7574936729, 86.0777379238, 129.988851816));
  rgInterface.addNode(rotateX(25, 75, 150));
  rgInterface.addNode(rotateX(25, 50, 150));
  rgInterface.addNode(rotateX(23.6210858267, 37.8791682316, 126.395535051));
  rgInterface.addNode(rotateX(0, 25, 137.5));
  rgInterface.addNode(rotateX(25, 25, 150));
  rgInterface.addNode(rotateX(0, 50, 150));
  rgInterface.addNode(rotateX(11.2601138293, 36.6452307082, 136.350441568));
  rgInterface.addNode(rotateX(0, 75, 137.5));
  rgInterface.addNode(rotateX(0, 75, 150));
  rgInterface.addNode(rotateX(0, 50, 137.5));
  rgInterface.addNode(rotateX(0, 25, 150));
  rgInterface.addNode(rotateX(14.1493516451, 13.9835772403, 129.490489515));
  rgInterface.addNode(rotateX(0, 25, 125));
  rgInterface.addNode(rotateX(25, 0, 125));
  rgInterface.addNode(rotateX(37.7962768347, 12.0516121187, 113.157622432));
  rgInterface.addNode(rotateX(36.8159650978, 11.8983451248, 136.818627753));
  rgInterface.addNode(rotateX(25, 0, 112.5));
  rgInterface.addNode(rotateX(0, 25, 112.5));
  rgInterface.addNode(rotateX(0, 0, 112.5));
  rgInterface.addNode(rotateX(25, 0, 137.5));
  rgInterface.addNode(rotateX(0, 0, 125));
  rgInterface.addNode(rotateX(0, 0, 137.5));
  rgInterface.addNode(rotateX(63.4839544844, 11.3008379812, 112.298834923));
  rgInterface.addNode(rotateX(50, 0, 125));
  rgInterface.addNode(rotateX(75, 0, 125));
  rgInterface.addNode(rotateX(50, 0, 112.5));
  rgInterface.addNode(rotateX(0, 50, 112.5));
  rgInterface.addNode(rotateX(88.0743580394, 61.9232219204, 113.07677808));
  rgInterface.addNode(rotateX(100, 75, 125));
  rgInterface.addNode(rotateX(100, 50, 112.5));
  rgInterface.addNode(rotateX(63.0755680595, 23.8472454173, 124.998381536));
  rgInterface.addNode(rotateX(61.9232219204, 11.9256419606, 136.92322192));
  rgInterface.addNode(rotateX(86.5551514064, 86.2519152086, 131.379038379));
  rgInterface.addNode(rotateX(100, 100, 137.5));
  rgInterface.addNode(rotateX(100, 75, 137.5));
  rgInterface.addNode(rotateX(36.941300193, 23.9466375262, 125.064905379));
  rgInterface.addNode(rotateX(50, 0, 150));
  rgInterface.addNode(rotateX(50, 25, 150));
  rgInterface.addNode(rotateX(75, 0, 150));
  rgInterface.addNode(rotateX(50, 100, 112.5));
  rgInterface.addNode(rotateX(25, 100, 112.5));
  rgInterface.addNode(rotateX(75, 0, 137.5));
  rgInterface.addNode(rotateX(100, 75, 112.5));
  rgInterface.addNode(rotateX(25, 0, 150));
  rgInterface.addNode(rotateX(0, 100, 125));
  rgInterface.addNode(rotateX(0, 75, 125));
  rgInterface.addNode(rotateX(0, 75, 112.5));
  rgInterface.addNode(rotateX(0, 100, 112.5));
  rgInterface.addNode(rotateX(100, 25, 112.5));
  rgInterface.addNode(rotateX(63.3255381877, 73.8030988309, 125.064817754));
  rgInterface.addNode(rotateX(100, 50, 137.5));
  rgInterface.addNode(rotateX(100, 0, 150));
  rgInterface.addNode(rotateX(100, 0, 137.5));
  rgInterface.addNode(rotateX(75, 0, 112.5));
  rgInterface.addNode(rotateX(50, 0, 137.5));
  rgInterface.addNode(rotateX(100, 0, 112.5));
  rgInterface.addNode(rotateX(100, 25, 137.5));
  rgInterface.addNode(rotateX(100, 0, 125));
  rgInterface.addNode(rotateX(61.9232219204, 88.0743580394, 113.07677808));
  rgInterface.addNode(rotateX(50, 100, 125));
  rgInterface.addNode(rotateX(61.9232219204, 88.0743580394, 136.92322192));
  rgInterface.addNode(rotateX(75, 100, 125));
  rgInterface.addNode(rotateX(75, 100, 112.5));
  rgInterface.addNode(rotateX(50, 50, 150));
  rgInterface.addNode(rotateX(50, 75, 150));
  rgInterface.addNode(rotateX(50, 100, 150));
  rgInterface.addNode(rotateX(75, 75, 150));
  rgInterface.addNode(rotateX(25, 100, 137.5));
  rgInterface.addNode(rotateX(25, 100, 125));
  rgInterface.addNode(rotateX(75, 100, 150));
  rgInterface.addNode(rotateX(75, 100, 137.5));
  rgInterface.addNode(rotateX(0, 100, 137.5));
  rgInterface.addNode(rotateX(0, 100, 150));
  rgInterface.addNode(rotateX(25, 100, 150));
  rgInterface.addNode(rotateX(36.9244319405, 88.0743580394, 136.92322192));
  rgInterface.addNode(rotateX(0, 0, 150));
  rgInterface.addNode(rotateX(50, 100, 137.5));
  rgInterface.addNode(rotateX(100, 75, 150));
  rgInterface.addNode(rotateX(100, 100, 125));
  rgInterface.addNode(rotateX(100, 100, 112.5));
  rgInterface.addNode(rotateX(100, 100, 150));
}

void CTestRGInterface3::createElements(RGInterface& rgInterface)
{
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 2, 3, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (5, 6, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 11, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (10, 11, 12, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 2, 4)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 15, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 15, 16)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 18, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 20, 21, 22)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 23, 24, 25)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 21, 23, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 27, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 22, 29, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 28, 29, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 3, 4, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 2, 3, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 16, 4, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 14, 2, 33)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 14, 2)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 36, 37)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 39, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 36, 37, 42)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 37, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 43, 44, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 43, 44, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 38, 46, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 39, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 37, 48, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 47, 50, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 52, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 32, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 51, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 42, 55, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 37, 57, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 36, 44, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 36, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 43, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (47, 50, 51, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 60, 61, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 39, 47, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 65, 66)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 23, 26, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 49, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 69, 70, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 65, 66, 72)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 45, 48, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 73, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 67, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 66, 72, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 73, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 72, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 76, 78, 79)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 56, 61, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 19, 20, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 49, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 20, 26, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 26, 67, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 49, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 26, 27, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 24, 25, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 26, 55, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 73, 77, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 68, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 73, 77, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 60, 73, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 73, 82, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 33, 73, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (54, 60, 61, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 49, 60, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 24, 26, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 60, 62, 70)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 61, 62, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 57, 67, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 60, 70, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 62, 70, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 69, 70, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 25, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 76, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 71, 75, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 22, 29, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 77, 79, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 77, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 72, 88, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 70, 74, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 70, 86, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 22, 23, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (4, 49, 68, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 37, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 2, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 4, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 16, 4, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 4, 52, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 4, 5, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 4, 48, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 50, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 42, 56, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 45, 91, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (77, 87, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 70, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (73, 77, 82, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 75, 77, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (77, 79, 87, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 23, 67, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 23, 71, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 71, 75, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 67, 80, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 21, 22, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 18, 20, 21)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (57, 67, 81, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 55, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (4, 5, 51, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 56, 62)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 5, 51, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 11, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 37, 49, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 37, 39, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 60, 68, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 20, 80, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 60, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 4, 50, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 31, 69)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 39, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 22, 23, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 81, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 78, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 70, 73, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 33, 6, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 32, 84, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 38, 39, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 48, 52, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 72, 74, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 36, 42, 44)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 55, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 42, 55, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (45, 48, 52, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 32, 54, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (65, 72, 74, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 32, 54, 65)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 36, 45, 48)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 37, 42, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 37, 56, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 55, 58, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 65, 73, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (24, 62, 86, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 24, 55, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (64, 65, 73, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (11, 2, 32, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 4, 48, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 3, 32, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 49, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 4, 52, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 39, 57, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 39, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 37, 42, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 23, 25, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (69, 70, 78, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (54, 65, 73, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 78, 79, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 54, 60, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (71, 75, 87, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (65, 66, 72, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 49, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (49, 54, 60, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (62, 70, 88, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 22, 71, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 68, 77, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 65, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 21, 26, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 72, 76, 78)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (69, 70, 71, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (69, 70, 78, 87)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 73, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 4, 49, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (18, 20, 22, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 24, 86, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (22, 23, 31, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 26, 81, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 31, 69, 71)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 37, 67, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 62, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (20, 21, 22, 23)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 18, 19, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 49, 60, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 105, 24, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 20, 21, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 65, 72, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 106, 65, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 5, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 62, 70, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 4, 50, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 5, 6, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 4, 5, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 3, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 16, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 108, 12, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 48, 52)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 11, 2, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 15, 16, 5)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (4, 50, 68, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 57, 67, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (48, 49, 54, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 45, 52, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 43, 51, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 41, 47, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 45, 48, 92)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 42, 46, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 38, 39, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 43, 47, 51)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (41, 47, 50, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 108, 109, 12)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 15, 5, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 43, 45, 53)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 5, 6, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 39, 40, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (54, 60, 73, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 50, 51, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 11, 3, 32)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 11, 12, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 42, 44, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 44, 45, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 108, 3, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 21, 28, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 38, 46, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (48, 52, 92, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 32, 73, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 35, 43, 45)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 37, 39, 57)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 26, 49, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 57, 58, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 42, 44, 46)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 42, 55, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 38, 57, 58)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (67, 80, 81, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 33, 73, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 57, 81, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 49, 56, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (26, 27, 55, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 48, 49, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (3, 52, 9, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 110, 25, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (105, 24, 27, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 25, 28, 31)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 70, 71, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 26, 49, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (71, 75, 93, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (110, 25, 27, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 25, 69, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 6, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (38, 39, 41, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 10, 15, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 37, 48, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 4, 49, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 54, 65, 73)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 14, 33, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 2, 84, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 49, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (16, 4, 52, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 2, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (4, 48, 49, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 33, 82, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 24, 25, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 82, 83, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (102, 108, 11, 3)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 64, 73, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (32, 65, 73, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 77, 79, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 77, 79, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 77, 82, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 76, 79, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 102, 32, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 64, 66, 76)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (14, 2, 33, 84)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 65, 74, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (60, 70, 73, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (75, 77, 87, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (101, 54, 65, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (106, 65, 66, 89)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 21, 29, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (37, 49, 56, 61)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 2, 33, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 82, 83, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 48, 49)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 17, 21, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 17, 21, 29)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (23, 24, 26, 56)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 3, 4, 54)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 17, 26, 81)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (63, 70, 72, 74)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 105, 24, 55)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 105, 110, 27)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 2, 4, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 14, 15, 96)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (35, 4, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (36, 44, 90, 91)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (34, 43, 46, 47)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 72, 86, 88)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (100, 42, 56, 90)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 51, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (17, 18, 21, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (1, 13, 14, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (107, 3, 9, 99)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (21, 23, 24, 26)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 77, 79, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 21, 27, 28)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (103, 26, 27, 97)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (2, 49, 54, 60)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (33, 64, 73, 83)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (104, 24, 25, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (108, 11, 3, 9)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (39, 40, 57, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (70, 72, 78, 86)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 18, 19, 30)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 114, 115, 116)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 115, 117, 118)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 118, 119, 120)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 114, 115, 117)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 117, 121, 122)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 119, 120, 123)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 117, 120, 122)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 120, 122, 123)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 126, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 127, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 128, 129, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 128, 129, 130)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 129, 130, 131)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 129, 131, 132)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 131, 132, 133)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 131, 134, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 137, 138, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 130, 135, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 140, 141, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 137, 138, 143)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 137, 139, 144)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 139, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 139, 144, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 134, 135, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 138, 139, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 147, 148)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 149, 150, 151)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 152, 153, 154)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 152, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 151, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 128, 130, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 156, 157, 158)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 144, 145, 150)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 128, 135, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 159, 67, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 159, 19, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 150, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 160, 161, 162)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (156, 157, 163, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (161, 165, 166, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 156, 157, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 156, 164, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 164, 169, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (164, 169, 170, 171)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 157, 163, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 172, 173, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 118, 164, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 164, 171, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 155, 163, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 75, 77, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (160, 175, 6, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 155, 157, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 75, 93, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 155, 75, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 155, 158, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 135, 137, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 134, 135, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (137, 144, 148, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 178, 179)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 137, 144, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (173, 180, 40, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 163, 168, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (163, 164, 168, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 148, 156, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 156, 163, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 114, 82, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 145, 150, 151)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 121, 160)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 114, 116, 121)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 117, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 114, 181, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 120, 122, 182)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 160, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 121, 122, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 171, 174, 184)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 118, 174, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 155, 186, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 117, 163, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 164, 169, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 181, 186, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 115, 119, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 186, 188, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 157, 185, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 147, 148, 156)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 149, 18, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 155, 163, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 158, 18, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 20, 75, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 191, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (191, 192, 193, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (182, 191, 192, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 191, 193, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 179, 80, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 195, 59, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 191, 50, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 165, 182, 191)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 6, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 121, 122, 160)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 160, 161)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 122, 160, 182)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 82, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 130, 131, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 118, 163, 164)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 138, 141, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 131, 132, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 126, 134)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 133, 134, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 140, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 129, 39, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 159, 179, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 138, 139, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 116, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 126, 134, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 127, 132, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 129, 130, 159)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 192, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 197, 198, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 182, 191, 193)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 200, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 198, 199, 202)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 182, 193, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 199, 202, 203)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 204, 205, 206)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 127, 132)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 132, 173, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 132, 133, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 173, 179, 180)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 180, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 172, 191, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 200, 204)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 163, 168, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (173, 179, 39, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 147, 148, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (143, 154, 176, 208)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 144, 168, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 147, 168, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (163, 168, 170, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 147, 149)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 156, 164, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 120, 126, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 160, 182, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 179, 39, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 120, 163, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 168, 20, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 68, 77, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 157, 158, 186)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (156, 163, 164, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 155, 156, 158)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 68, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 165, 191, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 193, 199, 203)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 160, 161, 165)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 175, 6)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (192, 193, 207, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 193, 197, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 39, 67, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 191, 50, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 172, 191, 192)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 135, 144, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 126, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 126, 182, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 127, 132, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 132, 201, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 161, 165, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 123, 199, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 132, 133, 140)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 196, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 160, 165, 182)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 118, 120, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 165, 182)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 122, 123, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 127, 191, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 132, 140, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 130, 131, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 182, 196, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (198, 200, 207, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 132, 133)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (197, 198, 206, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 137, 143, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 197, 206, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 135, 168, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 148, 152)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (161, 165, 175, 211)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 147, 149, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 125, 133, 134)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 130, 139, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 129, 159, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 144, 150, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (137, 144, 148, 168)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 134, 139, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 132, 179, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (200, 201, 207, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 173, 179, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 39, 41, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (192, 201, 207, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 167, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 127, 192, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 50, 68, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 127, 39, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 175, 195, 211)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 195, 211, 212)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (202, 203, 210, 213)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (199, 202, 203, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 165, 175, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 166, 203, 213)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 127, 191, 192)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 166, 194, 211)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 117, 77, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 114, 77, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 135, 20, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 139, 144, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 19, 20, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 130, 159, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 122, 182, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 173, 39, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 191, 192)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 147, 149, 156)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 168, 67, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 20, 67, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 39, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 126, 163, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 186, 77, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 115, 181, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 172, 41, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 150, 159, 19)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 182, 50, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 130, 150, 159)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 163, 164, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 134, 168, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 182, 197, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (172, 191, 195, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (160, 162, 175, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 135, 137, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 152, 154, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 137, 144, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 118, 119, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 119, 184, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 164, 170, 171)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 135, 139, 144)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 114, 117, 121)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (137, 148, 169, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 197, 198, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 139, 142, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 120, 126, 182)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (182, 193, 197, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 116, 121, 183)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 164, 168, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 126, 163, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 157, 164, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 148, 156, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 133, 140, 204)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 165, 182, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 122, 123, 199)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 163, 170, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 135, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 126, 182, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 133, 140, 141)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 116, 183, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 163, 168, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 123, 183, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (118, 174, 184, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 133, 197, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 119, 185, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 121, 160, 162)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (156, 164, 174, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 126, 50, 68)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 186, 93, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (181, 186, 188, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (181, 186, 77, 93)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 144, 153, 154)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 157, 174, 185)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 160, 82, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 182, 50, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 165, 199, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 202, 203, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (156, 157, 164, 174)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (135, 137, 139, 144)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 115, 116, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (137, 148, 169, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 155, 157, 186)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 149, 158, 18)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 18, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 18, 20, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (146, 148, 152, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 67, 80, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 149, 156, 158)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 168, 20, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (126, 67, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 157, 186, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 143, 154, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 137, 138, 139)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 181, 77, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 152, 169, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (181, 188, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 149, 18, 19)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 158, 186, 95)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 133, 196, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 126, 182, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 167, 183, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 199, 203, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 166, 194, 203)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 203, 210, 213)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (128, 130, 145, 150)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 191, 194, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 194, 195, 211)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 195, 212, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 175, 195, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (172, 191, 192, 194)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 133, 204, 206)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 39, 67, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 198, 202, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 192, 201, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 182, 191, 50)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 172, 173, 192)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 163, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (120, 126, 182, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 165, 175)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 140, 178, 204)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (212, 6, 7, 8)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 200, 204, 206)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 126, 134, 135)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 167, 210, 213)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 173, 180, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (192, 193, 194, 203)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 129, 132, 39)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 132, 173, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (173, 179, 180, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 182, 193, 197)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 126, 135, 67)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (175, 212, 6, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 173, 192, 201)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 115, 118, 119)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 178, 204)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 140, 141, 205)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 200, 206, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 160, 162, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (130, 131, 140, 178)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (137, 148, 168, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 177, 179, 180)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (193, 198, 207, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 159, 178, 179)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 191, 195, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (131, 134, 141, 142)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (173, 39, 40, 41)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 133, 134, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 131, 133, 134)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (125, 192, 193, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 133, 206, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 159, 67, 80)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 149, 150, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (129, 132, 178, 179)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 149, 150, 19)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (149, 150, 19, 20)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 146, 149, 151)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 137, 168, 170)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (132, 200, 201, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (164, 169, 171, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (112, 149, 150, 151)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (134, 168, 170, 196)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 148, 152, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (164, 171, 174, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 144, 154, 176)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (133, 140, 204, 205)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (161, 165, 166, 211)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (121, 122, 161, 167)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (173, 39, 40, 85)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (160, 161, 165, 175)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (172, 191, 194, 195)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (148, 152, 169, 187)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 193, 194, 203)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 165, 167, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 175, 6, 7)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (122, 123, 183, 210)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (160, 161, 162, 175)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (13, 160, 182, 82)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (127, 41, 50, 59)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (192, 193, 203, 209)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 181, 186, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 121, 162, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (117, 126, 68, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 117, 118, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (165, 166, 167, 213)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 185, 189, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 189, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 188, 190)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (198, 200, 206, 207)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (144, 145, 151, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (114, 116, 121, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 117, 118, 120)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 144, 145, 153)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (155, 163, 75, 77)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (147, 155, 156, 163)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 114, 162, 181)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (115, 116, 181, 188)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (181, 77, 93, 94)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (136, 139, 142, 145)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (111, 114, 162, 98)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (113, 119, 123, 189)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (168, 20, 67, 75)));
  rgInterface.addElement(RGElement(RGElementType::typeTE12L,
  ITestRGInterface::createNodes <RGNodeId> (124, 126, 67, 68)));
}

void CTestRGInterface3::createProperties(RGInterface& rgInterface)
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
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
  elementFormation.push_back(2);
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

const std::string FORMATION_0 = "Constant_depth_at_50_m";
const std::string FORMATION_1 = "Constant_depth_at_100_m";

} // anonymous namespace

void CTestRGInterface3::createFormationNames(RGInterface& rgInterface)
{
  rgInterface.setFormationName(1, FORMATION_0);
  rgInterface.setFormationName(2, FORMATION_1);
}
