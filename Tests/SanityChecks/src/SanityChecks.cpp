#include "SanityChecks.h"

#include "ModelBase.h"
#include "FemAppEntryTypes.h"
#include "ResultInfo.h"
#include "MaterialResultTree.h"
#include "Result.h"
#include "Utilities4ValueVector.h"
#include "Value.h"
#include "FilterResults.h"
#include "mlMaterial.h"
#include "MaterialHelperFactory.h"
#include "FilterValueTypes.h"
#include "LibraryMaterial.h"

#ifdef _WIN32
#include "GeomecStringTable.h"
#include "resource.h"
#include "resourceIDS.h"
#include <QDateTime>
#include <atlstr.h>
#include <atltime.h>
#include "TimeInterval.h"
#include "StringUtils.h"
#endif

#include "SettingsFile.h"
#include "lbcx.h"

#include "MPKernel.h"
#include "RGSync.h"
#include <QDir>
#include <QFile>
#include "mSleep.h"


#ifdef _WIN32
#include "ICommand.h"
#include "GraphNode.h"
#include "GraphNode_Delegate.h"
#endif

#include <iostream>

#include "ResultTree.h"

namespace test_sanity_checks
{

int LibraryLoad_SanityChecks() { return 0; }




TEST(SanityCheck, verifyValidNames)
{
  CModelBase *pModel = test_lib::TestLib::newModel();

  const CGraphEntryTemp<IResult> *graphEntryTemp = static_cast<const CGraphEntryTemp<IResult>*>(pModel->GraphEntry(MD_BASE_RESULT));
  const CGraphEntryTemp<IResult>::TNodeSet nodeSet = graphEntryTemp->EntryNodes();

  CResultInfo resultInfo;

  for (CGraphEntryTemp<IResult>::TNodeSet::const_iterator node = nodeSet.begin(); node != nodeSet.end(); ++node)
  {
    if (dynamic_cast<const CMaterialResult*>(*node) == 0)
    {
      int componentSize = (*node)->ComponentSize();

      EXPECT_GT(componentSize, 0);

      for (int component = 0; component < componentSize; ++component)
      {
        std::string exportLabel = (*node)->ExportLabel(component).toStdString();

        bool isValid = (*node)->ValidName(exportLabel, resultInfo);

        EXPECT_TRUE(isValid);

        if (!isValid)
          std::cout << exportLabel << " IS NOT VALID" << std::endl;
      }
    }
  }
}

namespace
{

const QString DOT = ".";
const QString TEMPLATE = "%1.%2.%3";

void verifyChildren(const IResult* node)
{
  typedef std::set <QString> TStrings;

  TStrings uniqueCombinations;

  for (unsigned int mode = 0; mode < node->ModeSize(); ++mode)
  {
    for (unsigned int component = 0;
      component < node->ComponentSize(mode); ++component)
    {
      const IResultComponent* resultComponent =
        dynamic_cast <const IResultComponent*> (
          &(node->Component(component, mode)));

      if (resultComponent != 0)
      {
        QString combination = TEMPLATE.arg(resultComponent->Name()).
          arg(resultComponent->Stage().Index()).
          arg(resultComponent->AnalysisType().AnalysisType());
        std::pair <TStrings::iterator, bool> result =
          uniqueCombinations.insert(combination);

        EXPECT_TRUE(result.second);
      }
    }
  }
}

} // anonymous namespace

TEST(SanityChecks, verifyUniquePaths)
{
  CModelBase* modelBase = test_lib::TestLib::newModel();

  typedef CGraphEntryTemp <IResult> TResultEntry;
  typedef std::set <QString> TStrings;

  const TResultEntry* resultEntry =
    static_cast <const TResultEntry*> (modelBase->GraphEntry(MD_BASE_RESULT));
  const TResultEntry::TNodeSet nodeSet = resultEntry->EntryNodes();
  TStrings uniquePaths;

  for (TResultEntry::TNodeSet::const_iterator node = nodeSet.begin();
    node != nodeSet.end(); ++node)
  {
    if (dynamic_cast <const IResult*> (*node) != 0)
    {
      CGraphNode* parent = *node;
      QString path;

      while (parent != 0)
      {
        path += parent->Name() + DOT;
        parent = parent->parent();
      }

      std::pair <TStrings::iterator, bool> result = uniquePaths.insert(path);

      EXPECT_TRUE(result.second);

      verifyChildren(*node);
    }
  }
}

TEST(SanityCheck, isResultsFilterComplete)
{
  CModelBase *pModel = test_lib::TestLib::newModel();

  const CGraphEntryTemp<IResult> *graphEntryTemp = static_cast<const CGraphEntryTemp<IResult>*>(pModel->GraphEntry(MD_BASE_RESULT));
  const CGraphEntryTemp<IResult>::TNodeSet nodeSet = graphEntryTemp->EntryNodes();

  std::vector <QString> exportLabels;
  CResultInfo resultInfo;

  cora::CFilterResults filterResults;

  for (CGraphEntryTemp<IResult>::TNodeSet::const_iterator node = nodeSet.begin(); node != nodeSet.end(); ++node)
  {
    if (dynamic_cast <const CMaterialResult*>(*node) == 0)
    {
      // can't call ComponentSize() when ModeSize() is 0, not fatal

      if ((*node)->ModeSize() == 0)
      {
        std::cout << (*node)->Name().toStdString() << " has ModeSize() == 0" << std::endl;
        continue;
      }

      unsigned int componentSize = (*node)->ComponentSize();

      for (unsigned int component = 0; component < componentSize; ++component)
      {
        if ((*node)->Component(component).Type() == IValueComponentBase::SCALAR)
        {
          QString exportLabel = (*node)->ExportLabel(component);

          exportLabels.push_back(exportLabel);

          bool notPresent = !filterResults.isResultPresent(exportLabel);

          EXPECT_FALSE(notPresent);

          if (notPresent)
            std::cout << exportLabel.toStdString() << " is NOT present in 'CFilterResults'" << std::endl;
        }
      }
    }
  }

  const std::map <QString, bool>& resultsFilter = filterResults.getFilter();

  for (std::map<QString, bool>::const_iterator result = resultsFilter.begin(); result != resultsFilter.end(); ++result)
  {
    bool notPresent = std::find(exportLabels.begin(), exportLabels.end(), result->first) == exportLabels.end();

    if (result->first != "Mud")
      EXPECT_FALSE(notPresent);

    if (notPresent)
      std::cout << (*result).first.toStdString() << " is NOT present in exported labels" << std::endl;
  }
}




namespace test_value_types_filter
{
const QString MATERIAL = "material";

size_t KNOWN_VALUE_TYPES[] = {
  32957, 32958, 32959, 32960, 32961, 32963, 32964, 32965, 32967, 32979, 32981,
  32987, 32989, 32991, 32993, 32995, 33064, 33065, 33066, 33070, 33071, 33072,
  33073, 33075, 33076, 40002, 40003, 40005, 40006, 40007, 40008, 40009, 40010,
  40011, 40012, 40013, 40014, 40015, 40016, 40018, 40019, 40020, 40021, 40022,
  40023, 40024, 40025, 40026, 40027, 40028, 40029, 40030, 40031, 40032, 40033,
  40034, 40035, 40036, 40037, 40038, 40100, 40101, 40109, 40110, 40111, 40112,
  40113, 40114, 40115, 40116, 40117, 40118, 40119, 40120, 40121, 40122, 40123,
  40124, 40125, 40126, 40127, 40128, 40129, 40130, 40131, 40135, 40136, 40137,
  40138, 40140, 40143, 40144, 40145, 40147, 40148, 40149, 40150, 40151, 40152,
  40153, 40156, 40157, 40158, 40159, 40160, 40161, 40162, 40163, 40207, 40208,
  40209, 40210, 40211, 40212, 40213, 40218, 40219
};

std::vector <size_t> VALUE_TYPES(KNOWN_VALUE_TYPES, KNOWN_VALUE_TYPES +
  (sizeof(KNOWN_VALUE_TYPES) / sizeof(KNOWN_VALUE_TYPES[0])));

const QString VALUE_TYPE_IS_NOT_PRESENT = "value type %1 is NOT present "
"in the value types filter or known value types";

bool isValueTypesFilterComplete()
{
  bool isValueTypesFilterComplete = true;

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  int materialModel = 0;
  for (ml::TMaterialCreatorRefPtr materialCreator = f->getMatCreator(materialModel);
    materialCreator != 0;
    materialCreator = f->getMatCreator(++materialModel))
  {
    ml::TMaterialPtr material = materialCreator->OnCreateMaterial();

    materialCreator->Create(MATERIAL, *material);

    cora::CFilterValueTypes filterValueTypes(materialModel);
    CLibraryMaterial* libraryMaterial =
      dynamic_cast <CLibraryMaterial*> (material);

    for (size_t p = 0; (libraryMaterial != 0) &&
      (p < libraryMaterial->ParameterSize()); ++p)
    {
      if (std::find(VALUE_TYPES.begin(), VALUE_TYPES.end(),
        libraryMaterial->Parameter(p).ValueTypeID()) == VALUE_TYPES.end())
      {
        std::cout << VALUE_TYPE_IS_NOT_PRESENT.
          arg(libraryMaterial->Parameter(p).ValueTypeID()).toStdString() <<
          std::endl;
        isValueTypesFilterComplete = false;
      }
    }

    materialCreator->Destroy(material);
  }

  return isValueTypesFilterComplete;
}

void listValueTypes()
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  int materialModel = 0;
  for (ml::TMaterialCreatorRefPtr materialCreator = f->getMatCreator(materialModel);
    materialCreator != 0;
    materialCreator = f->getMatCreator(++materialModel))
  {
    ml::TMaterialPtr material = materialCreator->OnCreateMaterial();

    materialCreator->Create(MATERIAL, *material);

    cora::CFilterValueTypes filterValueTypes(materialModel);
    CLibraryMaterial* libraryMaterial =
      dynamic_cast <CLibraryMaterial*> (material);

    for (size_t p = 0; (libraryMaterial != 0) &&
      (p < libraryMaterial->ParameterSize()); ++p)
    {
      std::cout << "material model " << materialModel << ":" <<
        libraryMaterial->Parameter(p).ValueTypeID() << std::endl;
    }

    materialCreator->Destroy(material);
  }
}

}


TEST(SanityChecks, isValueTypesFilterComplete)
{
  test_value_types_filter::listValueTypes();

  EXPECT_TRUE(test_value_types_filter::isValueTypesFilterComplete());
}



#ifdef _WIN32
namespace test_string_table
{

void testEquality(int lowerBoundary, int upperBoundary)
{
  CString cString;
  QString qString;

  for (int i = lowerBoundary; i <= upperBoundary; ++i)
  {
    if (!cString.LoadString(i))
    {
      cString = "";
    }

    /*
     * When cString equals "" the function getStringTableEntry(...) will
     * trigger an assertion if both tables are equal. Therefor we will skip
     * the call to getStringTableEntry(..) when cString equals "".
     */

    if (cString != "")
    {
      qString = getStringTableEntry(i);
      EXPECT_TRUE(qString == (LPCSTR)cString);
    }
  }
}

}


TEST(SanityChecks, testStringTable)
{
  test_string_table::testEquality(IDS_ET_YOUNGMODULUS_TRANS, IDR_MAINFRAME);
  test_string_table::testEquality(ID_NEW_SURFACE, ID_VIEW_STICKTOVIEW);
}






namespace test_qstring_cstring
{

const char* EMPTY = "";
const char* FILLED = "Filled";
const char* NOT_FILLED = "Not Filled";
const char* FILLED_LOWER_CASE = "filled";
const int right = 3;

void testQStringCString()
{
  CString cEmpty = EMPTY;
  QString qEmpty = EMPTY;

  EXPECT_TRUE(cEmpty.GetLength() == qEmpty.length());

  CString cFilled = FILLED;
  QString qFilled = FILLED;

  EXPECT_TRUE(cFilled.GetLength() == qFilled.length());
  EXPECT_TRUE(cFilled.Right(right) == qFilled.right(right));

  const char* equal = FILLED;
  const char* notEqual = NOT_FILLED;

  EXPECT_TRUE((cFilled.Compare(equal) == 0) && (qFilled.compare(equal) == 0));
  EXPECT_TRUE((cFilled.Compare(notEqual) < 0) && (qFilled.compare(notEqual) < 0));

  const char* noCase = FILLED_LOWER_CASE;

  EXPECT_TRUE((cFilled.CompareNoCase(noCase) == 0) &&
    (qFilled.compare(noCase, Qt::CaseInsensitive) == 0));
}

const char* PREFIX_SUFFIX = "prefix_suffix";
const int SUFFIX_LENGTH = 2;
const char* SUFFIX_STRING = "_s";
const char UNDERSCORE = '_';

void testFindAndManipulate()
{
  CString cString = PREFIX_SUFFIX;
  QString qString = PREFIX_SUFFIX;

  int cUnderscore = cString.ReverseFind(UNDERSCORE);
  int qUnderscore = qString.lastIndexOf(UNDERSCORE);

  EXPECT_TRUE(cUnderscore == qUnderscore);

  CString cSuffix = cString.Right(cString.GetLength() - cUnderscore);
  QString qSuffix = qString.right(qString.length() - qUnderscore);

  EXPECT_TRUE(cSuffix == qSuffix);
  EXPECT_TRUE((cSuffix.Left(SUFFIX_LENGTH) == SUFFIX_STRING) &&
    (qSuffix.left(SUFFIX_LENGTH) == SUFFIX_STRING));
}

const double SOME_DOUBLE = 12345.678;
const double ANOTHER_DOUBLE = 1234.5;
const double YET_ANOTHER_DOUBLE = 1.2;

void testFormatting()
{
  CString cName;
  QString qName;

  cName.Format("%6.2f", SOME_DOUBLE);
  qName = QString("%1").arg(SOME_DOUBLE, 6, 'f', 2);

  EXPECT_TRUE((LPCSTR)cName == qName);

  cName.Format("%6.2f", ANOTHER_DOUBLE);
  qName = QString("%1").arg(ANOTHER_DOUBLE, 6, 'f', 2);

  EXPECT_TRUE((LPCSTR)cName == qName);

  cName.Format("%6.2f", YET_ANOTHER_DOUBLE);
  qName = QString("%1").arg(YET_ANOTHER_DOUBLE, 6, 'f', 2);

  EXPECT_TRUE((LPCSTR)cName == qName);
}

const char* EXCLUDE = "_";
const char* WITH_UNDERSCORE = "WITH_UNDERSCORE";
const char* WITHOUTUNDERSCORE = "WITHOUTUNDERSCORE";

void testOldSpanExcluding()
{
  CString cString = WITH_UNDERSCORE;
  QString qString = WITH_UNDERSCORE;

  EXPECT_TRUE(cString.SpanExcluding(EXCLUDE) ==
    qString.left((qString.indexOf(EXCLUDE) != -1 ?
    qString.indexOf(EXCLUDE) : qString.length())));

  cString = cString.SpanExcluding(EXCLUDE);
  qString = qString.left((qString.indexOf(EXCLUDE) != -1 ?
    qString.indexOf(EXCLUDE) : qString.length()));

  cString = WITHOUTUNDERSCORE;
  qString = WITHOUTUNDERSCORE;

  EXPECT_TRUE(cString.SpanExcluding(EXCLUDE) ==
    qString.left((qString.indexOf(EXCLUDE) != -1 ?
    qString.indexOf(EXCLUDE) : qString.length())));

  cString = cString.SpanExcluding(EXCLUDE);
  qString = qString.left((qString.indexOf(EXCLUDE) != -1 ?
    qString.indexOf(EXCLUDE) : qString.length()));
}

const char* CDATE_TIME_FORMAT_0 = "*** %d %b %Y, %H:%M ***";
const char* QDATE_TIME_FORMAT_0 = "'***' dd MMM yyyy',' hh':'mm '***'";

const char* CDATE_TIME_FORMAT_1 = "*** starting runner at %d %b %Y, %H:%M ***";
const char* QDATE_TIME_FORMAT_1 =
"'*** starting runner at' dd MMM yyyy',' hh':'mm '***'";

const char* CDATE_TIME_FORMAT_2 = "*** runner returned at %d %b %Y, %H:%M ***";
const char* QDATE_TIME_FORMAT_2 =
"'*** runner returned at' dd MMM yyyy',' hh':'mm '***'";

void testQDateTimeCTime()
{
  CTime cTime = CTime::GetCurrentTime();
  CString cString = cTime.Format(CDATE_TIME_FORMAT_0);

  QDateTime qDateTime = QDateTime::currentDateTime();
  QString qString = qDateTime.toString(QDATE_TIME_FORMAT_0);

  EXPECT_TRUE(cString.MakeLower() == qString.toLower());

  cTime = CTime::GetCurrentTime();
  cString = cTime.Format(CDATE_TIME_FORMAT_1);

  qDateTime = QDateTime::currentDateTime();
  qString = qDateTime.toString(QDATE_TIME_FORMAT_1);

  EXPECT_TRUE(cString.MakeLower() == qString.toLower());

  cTime = CTime::GetCurrentTime();
  cString = cTime.Format(CDATE_TIME_FORMAT_2);

  qDateTime = QDateTime::currentDateTime();
  qString = qDateTime.toString(QDATE_TIME_FORMAT_2);

  EXPECT_TRUE(cString.MakeLower() == qString.toLower());
}

const char* BEGIN_DATE_TIME = "2009,06,12,18,32,00";
const char* END_DATE_TIME = "2011,06,13,22,47,00";
const char* DATE_TIME_FORMAT = "yyyy,MM,dd,HH,mm,ss";

void testTimeInterval()
{
  QDateTime qBegin =
    QDateTime::fromString(BEGIN_DATE_TIME, DATE_TIME_FORMAT);
  QDateTime qEnd =
    QDateTime::fromString(END_DATE_TIME, DATE_TIME_FORMAT);
  TTimeInterval qTimeInterval = calculateTimeInterval(qBegin, qEnd);
  CTime cBegin(2009, 6, 12, 18, 32, 0);
  CTime cEnd(2011, 6, 13, 22, 47, 0);
  CTimeSpan cTimeInterval = cEnd - cBegin;

  EXPECT_TRUE(qTimeInterval.days == cTimeInterval.GetDays());
  EXPECT_TRUE(qTimeInterval.hours == cTimeInterval.GetHours());
  EXPECT_TRUE(qTimeInterval.minutes == cTimeInterval.GetMinutes());
  EXPECT_TRUE(qTimeInterval.seconds == cTimeInterval.GetSeconds());
}

const char* FINDONEOF_0 = "";
const char* FINDONEOF_1 = "_";
const char* FINDONEOF_2 = " ,;";
const char* FINDONEOF_3 = " ,;|";

const char* FINDONEOF_S = "yyyy|MM|dd|HH|mm|ss";

void testFindOneOf()
{
  CString cString;
  QString qString;

  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_0) == findOneOf(qString, FINDONEOF_0));

  cString = WITH_UNDERSCORE;
  qString = WITH_UNDERSCORE;

  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_1) == findOneOf(qString, FINDONEOF_1));

  cString = WITHOUTUNDERSCORE;
  qString = WITHOUTUNDERSCORE;

  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_1) == findOneOf(qString, FINDONEOF_1));

  cString = DATE_TIME_FORMAT;
  qString = DATE_TIME_FORMAT;

  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_2) == findOneOf(qString, FINDONEOF_2));
  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_3) == findOneOf(qString, FINDONEOF_3));

  cString = FINDONEOF_S;
  qString = FINDONEOF_S;

  int c = cString.FindOneOf(FINDONEOF_3);
  int q = findOneOf(qString, FINDONEOF_3);

  EXPECT_TRUE(cString.FindOneOf(FINDONEOF_3) == findOneOf(qString, FINDONEOF_3));
}

void testSpanExcluding()
{
  CString cString;
  QString qString;

  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_0) == spanExcluding(qString, FINDONEOF_0));

  cString = WITH_UNDERSCORE;
  qString = WITH_UNDERSCORE;

  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_1) == spanExcluding(qString, FINDONEOF_1));

  cString = WITHOUTUNDERSCORE;
  qString = WITHOUTUNDERSCORE;

  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_1) == spanExcluding(qString, FINDONEOF_1));

  cString = DATE_TIME_FORMAT;
  qString = DATE_TIME_FORMAT;

  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_2) == spanExcluding(qString, FINDONEOF_2));
  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_3) == spanExcluding(qString, FINDONEOF_3));

  cString = FINDONEOF_S;
  qString = FINDONEOF_S;

  EXPECT_TRUE(cString.SpanExcluding(FINDONEOF_S) == spanExcluding(qString, FINDONEOF_S));
}

const char* WHITESPACE = " \t\n\v\f\r";

void testTrimRight()
{
  CString cString;
  QString qString;

  EXPECT_TRUE(cString.TrimRight() == trimRight(qString));

  cString = WHITESPACE;
  qString = WHITESPACE;

  EXPECT_TRUE(cString.TrimRight() == trimRight(qString));
}

}


TEST(SanityChecks, testQStringCString)
{
  test_qstring_cstring::testQStringCString();
  test_qstring_cstring::testFindAndManipulate();
  test_qstring_cstring::testFormatting();
  test_qstring_cstring::testOldSpanExcluding();
  test_qstring_cstring::testQDateTimeCTime();
  test_qstring_cstring::testTimeInterval();
  test_qstring_cstring::testFindOneOf();
  test_qstring_cstring::testSpanExcluding();
  test_qstring_cstring::testTrimRight();
}

#endif // _WIN32




namespace test_utilities_4_values_vector
{

template <typename T>
void fillValues(std::vector <T>& values)
{
  values.push_back(0.0);
  values.push_back(1.0);
  values.push_back(2.0);
  values.push_back(3.0);
  values.push_back(4.0);
  values.push_back(5.0);
  values.push_back(6.0);
  values.push_back(7.0);
  values.push_back(8.0);
  values.push_back(9.0);
}

void testValid(const std::vector <geo::CValue>& values, bool validity)
{
  EXPECT_TRUE(cora::CUtilities4ValueVector::isValueVectorValid(values) == validity);
}

const double AVERAGE = 4.5;

template <typename T>
void testAverage(const std::vector <T>& values)
{
  assert(cora::CUtilities4ValueVector::calculateAverage(values) == AVERAGE);
}

void invalidateSomeValues(std::vector <geo::CValue>& values)
{
  values[2] = geo::CValue();
  values[5] = geo::CValue();
  values[8] = geo::CValue();
}

} // anonymous namespace

TEST(SanityChecks, testUtilities4ValueVector)
{
  std::vector <geo::CValue> values;

  test_utilities_4_values_vector::fillValues(values);

  test_utilities_4_values_vector::testValid(values, true);
  test_utilities_4_values_vector::testAverage(values);

  test_utilities_4_values_vector::invalidateSomeValues(values);

  test_utilities_4_values_vector::testValid(values, false);

  std::vector <double> newValues;

  test_utilities_4_values_vector::fillValues(newValues);
  test_utilities_4_values_vector::testAverage(newValues);
}







namespace test_qtfileregistry
{

const QString FILE_NAME = "TestFileRegistry.ini";
const QString FILE_NAME_SYS = "TestFileRegistry_sys.ini";
const QString SECTION_NAME = "TestFileRegistry";
const QString STRING_ENTRY_NAME = "TestFileRegistryString";
const QString INT_ENTRY_NAME = "TestFileRegistryInt";
const QString STRING_VALUE = "TestFileRegistry";
const int INT_VALUE = 42;
const QString NEW_STRING_VALUE = "writeFileRegistry";
const int NEW_INT_VALUE = 666;

// system : 'write' not implemented so value can't be changed
// TODO: Implement system write: int and string ... and system read int
void readFileRegistry(const QString& string, int integer, bool only_user = false)
{
  ISettings* registry = ISettings::instance();

  QString stringValue =
    registry->getProfileString(SECTION_NAME, STRING_ENTRY_NAME);

  EXPECT_TRUE(stringValue == string);

  int intValue = registry->getProfileInt(SECTION_NAME, INT_ENTRY_NAME);
  EXPECT_TRUE(intValue == integer);

  if( only_user )
	  return;

  stringValue = registry->getSystemString(SECTION_NAME, STRING_ENTRY_NAME);
  EXPECT_TRUE(stringValue == string);
}

void writeFileRegistry(const QString& string, int integer)
{
  ISettings* registry = ISettings::instance();

  // usr
  EXPECT_TRUE(registry->writeProfileString(SECTION_NAME, STRING_ENTRY_NAME, string));
  EXPECT_TRUE(registry->writeProfileInt(SECTION_NAME, INT_ENTRY_NAME, integer));

  // system : only 'get' implemented
}

}

TEST(SanityChecks, QtFileRegistry)
{
  ISettings::cleanup();

  QString current = QDir::currentPath();

  QString tmpPath = QString(test_lib::TestLib::ModelPath()) + "tmp_" + test_qtfileregistry::FILE_NAME;
  QString tmpPath_sys = QString(test_lib::TestLib::ModelPath()) + "tmp_" + test_qtfileregistry::FILE_NAME_SYS;

  bool res = QFile::copy(test_lib::TestLib::ModelPath() + test_qtfileregistry::FILE_NAME, tmpPath);
  bool res_ = QFile::copy(test_lib::TestLib::ModelPath() + test_qtfileregistry::FILE_NAME, tmpPath_sys);

  CSettingsFile::init(tmpPath, tmpPath_sys);

  test_qtfileregistry::readFileRegistry(test_qtfileregistry::STRING_VALUE, test_qtfileregistry::INT_VALUE);
  test_qtfileregistry::writeFileRegistry(test_qtfileregistry::NEW_STRING_VALUE, test_qtfileregistry::NEW_INT_VALUE);
  test_qtfileregistry::readFileRegistry(test_qtfileregistry::NEW_STRING_VALUE, test_qtfileregistry::NEW_INT_VALUE, true );
  test_qtfileregistry::writeFileRegistry(test_qtfileregistry::STRING_VALUE, test_qtfileregistry::INT_VALUE);
}



namespace test_environment
{


void testExistingEnvironmentVariables()
{
  char* getenvPath = getenv("PATH");
  char* diGetenvPath = DiGetenv("PATH");

  EXPECT_TRUE(getenvPath != 0);
  EXPECT_TRUE(diGetenvPath != 0);
  EXPECT_TRUE(strcmp(getenvPath, diGetenvPath) == 0);
}

const char* TEST_NATIVE_2_DIANA = "testNative2Diana";
const char* ASSIGNMENT = "%1=%2";
const char* EMPTY = "";

// See comment @ TEST(SanityChecks, Environment)
void testNative2Diana()
{
  QString assignment = QString(ASSIGNMENT).arg(TEST_NATIVE_2_DIANA).
    arg(TEST_NATIVE_2_DIANA);

  EXPECT_TRUE(putenv(strdup((char*)assignment.toStdString().c_str())) == 0);

  std::cout << "putenv(strdup(assignment.toStdString().c_str())) == 0" << std::endl;

  char* diana = DiGetenv(TEST_NATIVE_2_DIANA);

#if defined(linux)
  EXPECT_TRUE(diana != 0);
  std::cout << "diana != 0" << std::endl;
#else // !defined(linux)
# if defined(NDEBUG)
  EXPECT_TRUE(diana != 0);
  std::cout << "diana != 0" << std::endl;
# else // !defined(NDEBUG)
  EXPECT_TRUE(diana != 0);
  std::cout << "diana != 0" << std::endl;
# endif  // defined(NDEBUG)
#endif  // defined(linux)

  if (diana != 0)
  {
    EXPECT_TRUE(strcmp(diana, TEST_NATIVE_2_DIANA) == 0);
    std::cout << "strcmp(diana, TEST_NATIVE_2_DIANA) == 0" << std::endl;
  }

  assignment = QString(ASSIGNMENT).arg(TEST_NATIVE_2_DIANA).
    arg(EMPTY);

  EXPECT_TRUE(putenv(strdup((char*)assignment.toStdString().c_str())) == 0);
  std::cout << "putenv(strdup(assignment.toStdString().c_str())) == 0" << std::endl;
}

const char* TEST_DIANA_2_NATIVE = "testDiana2Native";

void testDiana2Native()
{
  QString assignment = QString(ASSIGNMENT).arg(TEST_DIANA_2_NATIVE).
    arg(TEST_DIANA_2_NATIVE);

  EXPECT_TRUE(vDiSetenv(assignment.toStdString().c_str()) == 0);
  std::cout << "vDiSetenv(assignment.toStdString().c_str()) == 0" << std::endl;

  char* native = getenv(TEST_DIANA_2_NATIVE);

#if defined(linux)
  EXPECT_TRUE(native != 0);
  std::cout << "native != 0" << std::endl;
#else // !defined(linux)
# if defined(NDEBUG)
  EXPECT_TRUE(native != 0);
  std::cout << "native != 0" << std::endl;
# else // !defined(NDEBUG)
  EXPECT_TRUE(native != 0);
  std::cout << "native != 0" << std::endl;
# endif  // defined(NDEBUG)
#endif  // defined(linux)

  if (native != 0)
  {
    EXPECT_TRUE(strcmp(native, TEST_DIANA_2_NATIVE) == 0);
    std::cout << "strcmp(native, TEST_DIANA_2_NATIVE) == 0" << std::endl;
  }

  assignment = QString(ASSIGNMENT).arg(TEST_DIANA_2_NATIVE).
    arg(EMPTY);

  EXPECT_TRUE(vDiSetenv(assignment.toStdString().c_str()) == 0);
  std::cout << "vDiSetenv(assignment.toStdString().c_str()) == 0" << std::endl;
}

const char* TEST_NATIVE = "testNative";

// See comment @ TEST(SanityChecks, Environment)
void testNative()
{
  QString assignment = QString(ASSIGNMENT).arg(TEST_NATIVE).arg(TEST_NATIVE);

  EXPECT_TRUE(putenv(strdup((char*)assignment.toStdString().c_str())) == 0);
  std::cout << "putenv(strdup(assignment.toStdString().c_str())) == 0" << std::endl;

  char* native = getenv(TEST_NATIVE);

  EXPECT_TRUE(native != 0);
  std::cout << "native != 0" << std::endl;
  EXPECT_TRUE(strcmp(native, TEST_NATIVE) == 0);
  std::cout << "strcmp(native, TEST_NATIVE) == 0" << std::endl;

  assignment = QString(ASSIGNMENT).arg(TEST_NATIVE).arg(EMPTY);

  EXPECT_TRUE(putenv(strdup((char*)assignment.toStdString().c_str())) == 0);
  std::cout << "putenv(strdup(assignment.toStdString().c_str())) == 0" << std::endl;
}

const char* TEST_DIANA = "testDiana";

void testDiana()
{
  QString assignment = QString(ASSIGNMENT).arg(TEST_DIANA).arg(TEST_DIANA);

  EXPECT_TRUE(vDiSetenv(assignment.toStdString().c_str()) == 0);
  std::cout << "vDiSetenv(assignment.toStdString().c_str()) == 0" << std::endl;

  char* diana = DiGetenv(TEST_DIANA);

  EXPECT_TRUE(diana != 0);
  std::cout << "diana != 0" << std::endl;

  EXPECT_TRUE(strcmp(diana, TEST_DIANA) == 0);
  std::cout << "strcmp(diana, TEST_DIANA) == 0" << std::endl;

  assignment = QString(ASSIGNMENT).arg(TEST_DIANA).arg(EMPTY);

  EXPECT_TRUE(vDiSetenv(assignment.toStdString().c_str()) == 0);
  std::cout << "vDiSetenv(assignment.toStdString().c_str()) == 0" << std::endl;
}

}

/* MCR 2018-09-28: 
	'testNative2Diana' and 'testNative' crashes with gcc 7.3.0 and C++14
  JH 2018-10-29:
  'QString::toStdString::c_str' goes out of scope, and 'putenv' retains
  a pointer to invalid data.
  See: https://wiki.sei.cmu.edu/confluence/display/c/POS34-C.+Do+not+call+putenv%28%29+with+a+pointer+to+an+automatic+variable+as+the+argument
  For these tests we use 'strdup' now, but in production code that would
  cause memory leaks. Use vDiSetenv there. */
TEST(SanityChecks, Environment)
{
  test_environment::testExistingEnvironmentVariables();
  test_environment::testNative2Diana();
  test_environment::testDiana2Native();
  test_environment::testNative();
  test_environment::testDiana();
}



#ifdef _WIN32

// We get a segfault on linux; TODO: find out what's happening there

namespace test_tbb_rgi
{


class MasterTask : public tbb::task
{
  tbb::concurrent_vector<QString>& output;
  const char *name;

public:
  MasterTask(tbb::concurrent_vector<QString>& output, const char *name) : output(output), name(name) {}

  virtual tbb::task *execute()
  { 
    output.push_back("-- Master");

    RGSync sync(RGSync::Lead, name);

    sync.init();
    sync.lock();

    output.push_back("-- Master : do RE simulation!");

    // sleep some time to mimic the simulation.
    mSleep(1000);

    output.push_back("-- Master : release resource");
    sync.release();

    // let the other process do its work.
    output.push_back("-- Master : RE waits for GM");
    sync.waitFor(-1);

    output.push_back("-- Master : continue");

    return 0;
  }
};

class SlaveTask : public tbb::task
{
  tbb::concurrent_vector<QString>& output;
  const char *name;

public:
  SlaveTask(tbb::concurrent_vector<QString>& output, const char *name) : output(output), name(name) {}

  virtual tbb::task *execute()
  { 
    output.push_back("-- Slave");

    RGSync sync(RGSync::Follower, name);

    output.push_back("-- Slave :  GM waits for RE");
    sync.waitFor(-1);
    sync.lock();

    output.push_back("-- Slave :  do GM actions !");
    mSleep(1000);

    output.push_back("-- Slave : release resource");
    sync.release();

    return 0;
  }
};

class Run
{
  tbb::task& task;
public:
  Run(tbb::task& task) : task(task) {}
  void operator()() const { task.execute(); }
};

}

#ifdef _WIN32
#define PATH_DELIM  "\\"
#else
#define PATH_DELIM  "/"
#endif

TEST(SanityChecks, TBBAndRGISync)
{
  tbb::concurrent_vector<QString> output;
  const char *name = "tbb_rgi";

  QDir cwd;
  cwd.mkdir(name);
  EXPECT_TRUE(QFile::exists(name));

  tbb::task_group g;
  test_tbb_rgi::MasterTask master(output, name);
  test_tbb_rgi::SlaveTask slave(output, name);

  g.run(test_tbb_rgi::Run(master));
  
  mSleep(50); // give the master some time to set up the lock

  g.run(test_tbb_rgi::Run(slave));
  
  g.wait();

  for (tbb::concurrent_vector<QString>::const_iterator it = output.begin(); it != output.end(); ++it)
    std::cout << it->toStdString() << std::endl;

  EXPECT_TRUE(output.back() == "-- Master : continue");

  EXPECT_TRUE(cwd.rmdir(name));
}
#endif


} // namespace test_sanity_checks





#ifdef _WIN32
namespace
{

class Simple : public CGraphNode
{
public:
  Simple();
  ~Simple();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
};

Simple::Simple()
: CGraphNode(1)
{
}

Simple::~Simple()
{
}

unsigned int Simple::IconId() const
{
  return 0;
}

unsigned int Simple::TypeId() const
{
  return 0;
}

class Simple_Delegate : public CGraphNode_Delegate
{
public:
  Simple_Delegate(Simple* simple);
  ~Simple_Delegate();

  CGraphNode* test() { return getGraphNode(); }

private:
  REGISTER_DELEGATE(Simple, Simple_Delegate);
};

Simple_Delegate::Simple_Delegate(Simple* simple)
: CGraphNode_Delegate(simple)
{
}

Simple_Delegate::~Simple_Delegate()
{
}

template <class NODE_TYPE, class DELEGATE_TYPE>
  DELEGATE_TYPE* testTemplate(NODE_TYPE& simple)
{
  std::string name = typeid(simple).name();

  name = typeid(&simple).name();

  DELEGATE_TYPE* simpleDelegate =
    static_cast <DELEGATE_TYPE*> (simple.getDelegate());

  return simpleDelegate;
}

void testDelegate()
{
  Simple simple;
  Simple_Delegate* simpleDelegate =
    testTemplate <Simple, Simple_Delegate> (simple);
  std::string name = typeid(simpleDelegate).name();

  EXPECT_TRUE(simpleDelegate->test() != 0);
  EXPECT_TRUE((name == "class `anonymous namespace'::Simple_Delegate *") ||
    (name == "class `anonymous namespace'::Simple_Delegate * __ptr64"));
}

template <class T>
  class SimpleTemplate : public CGraphNode
{
public:
  SimpleTemplate();
  ~SimpleTemplate();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
};

template <class T>
  SimpleTemplate <T> ::SimpleTemplate()
: CGraphNode(1)
{
}

template <class T>
  SimpleTemplate <T> ::~SimpleTemplate()
{
}

template <class T>
  unsigned int SimpleTemplate <T> ::IconId() const
{
  return 0;
}

template <class T>
  unsigned int SimpleTemplate <T> ::TypeId() const
{
  return 0;
}

template <class T>
  class SimpleTemplate_Delegate : public CGraphNode_Delegate
{
public:
  SimpleTemplate_Delegate(SimpleTemplate <T> * simple);
  ~SimpleTemplate_Delegate();

  CGraphNode* test() { return getGraphNode(); }

private:
  REGISTER_DELEGATE(SimpleTemplate <T>, SimpleTemplate_Delegate <T>);
};

template <class T>
  SimpleTemplate_Delegate <T> ::SimpleTemplate_Delegate(
    SimpleTemplate <T> * simple)
: CGraphNode_Delegate(simple)
{
  ACTIVATE_TEMPLATE_DELEGATE(SimpleTemplate <T>, SimpleTemplate_Delegate <T>);
}

template <class T>
  SimpleTemplate_Delegate <T> ::~SimpleTemplate_Delegate()
{
}

void testTemplateDelegate()
{
  SimpleTemplate <Simple> * anotherSimple = new SimpleTemplate <Simple> ();
  SimpleTemplate_Delegate <Simple> * anotherSimpleDelegate =
    new SimpleTemplate_Delegate <Simple> (anotherSimple);

  EXPECT_TRUE(anotherSimpleDelegate->test() != 0);

  delete anotherSimpleDelegate;
  delete anotherSimple;

  SimpleTemplate <Simple> simple;
  SimpleTemplate_Delegate <Simple> * simpleDelegate =
    testTemplate <SimpleTemplate <Simple> , SimpleTemplate_Delegate <Simple> >
      (simple);

  EXPECT_TRUE(simpleDelegate->test() != 0);

  std::string name = typeid(simpleDelegate).name();

  EXPECT_TRUE((name == "class `anonymous namespace'::SimpleTemplate_Delegate<class `anonymous namespace'::Simple> *") ||
    (name == "class `anonymous namespace'::SimpleTemplate_Delegate<class `anonymous namespace'::Simple> * __ptr64"));
}

class A
{
public:
  virtual ~A() {};
};

class B : public A
{
public:
  virtual ~B() {};
};

template <class T>
  class UnaryTemplate
{
public:
  UnaryTemplate()
  {
    std::string name = typeid(T).name();
  }
};

void testTypeID()
{
  A* a = new A;
  std::string name = typeid(a).name();

  EXPECT_TRUE((name == "class `anonymous namespace'::A *") ||
    (name == "class `anonymous namespace'::A * __ptr64"));
  name = typeid(*a).name();
  EXPECT_TRUE(name == "class `anonymous namespace'::A");

  delete a;

  A* b = new B;

  name = typeid(b).name();
  EXPECT_TRUE((name == "class `anonymous namespace'::A *") ||
    (name == "class `anonymous namespace'::A * __ptr64"));
  name = typeid(*b).name();
  EXPECT_TRUE(name == "class `anonymous namespace'::B");

  delete b;

  B* c = new B;

  name = typeid(c).name();
  EXPECT_TRUE((name == "class `anonymous namespace'::B *") ||
    (name == "class `anonymous namespace'::B * __ptr64"));
  name = typeid(*c).name();
  EXPECT_TRUE(name == "class `anonymous namespace'::B");

  delete c;

  UnaryTemplate <A> unaryTemplateA;

  name = typeid(UnaryTemplate <A>).name();
}

}

TEST(SanityChecks, Delegate)
{
  testTypeID();
  testDelegate();
  testTemplateDelegate();
}

#endif // _WIN32




