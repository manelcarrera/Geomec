
#undef NDEBUG

#include <cassert>

#include <QDir>

#include "TestRGInterface.0.h"
#include "TestRGInterface.1.h"
#include "TestRGInterface.2.h"
#include "TestRGInterface.3.h"
#include "TestRGInterface.4.h"
#include "TestRGInterface.5.h"
#include "TestRGInterface.6.h"
#include "TestRGInterface.7.h"
#include "TestRGInterface.8.h"
#include "TestRGInterface.9.h"

#define ASSERT(p) { bool b = (p); assert(b); if (!b) exit(1); }


namespace
{

const QString TEST_0 = "TestRGInterface.0";
const QString TEST_1 = "TestRGInterface.1";
const QString TEST_2 = "TestRGInterface.2";
const QString TEST_3 = "TestRGInterface.3";
const QString TEST_4 = "TestRGInterface.4";
const QString TEST_5 = "TestRGInterface.5";
const QString TEST_6 = "TestRGInterface.6";
const QString TEST_7 = "TestRGInterface.7";
const QString TEST_8 = "TestRGInterface.8";
const QString TEST_9 = "TestRGInterface.9";

QString stripQuotes(const std::string& string)
{
  QString stripped(string.c_str());

  if (stripped.startsWith(QChar('\"')) || stripped.startsWith(QChar('\'')))
  {
  stripped.remove(0, 1);
  }

  if (stripped.endsWith(QChar('\"')) || stripped.endsWith(QChar('\'')))
  {
  stripped.remove(stripped.length() - 1, 1);
  }

  return stripped;
}

} // anonymous namespace

int main(int argc, char* argv[])
{
  ASSERT(argc == 2);

  CTestRGInterface0 CTestRGInterface0(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_0));
  CTestRGInterface1 CTestRGInterface1(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_1));
  CTestRGInterface2 CTestRGInterface2(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_2));
  CTestRGInterface3 CTestRGInterface3(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_3));
  CTestRGInterface4 CTestRGInterface4(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_4));
  CTestRGInterface5 CTestRGInterface5(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_5));
  CTestRGInterface6 CTestRGInterface6(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_6));
  CTestRGInterface7 CTestRGInterface7(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_7));
  CTestRGInterface8 CTestRGInterface8(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_8));
  CTestRGInterface9 CTestRGInterface9(
  QDir::toNativeSeparators(stripQuotes(argv[1]) + '/' + TEST_9));

  return 0;
}
