#include "Printers.h"
#include "TestLib.h"
#include <fstream>

/*

See:

http://blogs.msdn.com/b/dhopton/archive/2008/06/12/helpful-internals-of-trx-and-vsmdi-files.aspx
C:\Program Files (x86)\Microsoft Visual Studio 10.0\Xml\Schemas\vstst.xsd
http://trx2html.codeplex.com/ (sources with trx example file)

for divining the trx file format.

At the moment, it looks like the VS test browser doesn't fully recognize test lists,
and apparently the test run id needs to be present in some dictionary. This will probably
go better if we have some vsmdi file and stuff.

Example of publishing to TFS:

mstest /publishresultsfile:test_output.trx /publish:https://sww-sede-team.shell.com/tfs/PTSSW /TeamProject:Geomec /platform:x64 /flavor:Release /publishbuild:"Geomec Test_20131109.1"

*/


using namespace test_lib;


RTXPrinter::RTXPrinter(const std::string &fileName) : m_FileName(fileName)
{
  m_Tests.reserve(1000);
}



void RTXPrinter::OnTestProgramStart(const UnitTest& /*unit_test*/)
{
  m_TestListID = TestLib::getGUID();
}


void RTXPrinter::OnTestProgramEnd(const UnitTest& unit_test)
{
  std::ofstream output;
  output.open(m_FileName.c_str(), std::ios::trunc);

  assert(output.is_open());

  const int total  = unit_test.total_test_count();
  const int failed = unit_test.failed_test_count();
  const int passed = unit_test.successful_test_count();
  const int execed = unit_test.test_to_run_count();

  const std::string name = TestLib::getOS() + " test run " + TestLib::getDateTime();

  output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  output << "<TestRun id=\"" << TestLib::getGUID() << "\" name=\"" << name << "\" runUser=\"" << TestLib::getUser() << "\" xmlns=\"http://microsoft.com/schemas/VisualStudio/TeamTest/2010\">" << std::endl;

  output << "<TestSettings name=\"Default Test Settings\" id=\"" << TestLib::getGUID() << "\">" << std::endl;
  output << " <Execution>" << std::endl;
  output << "   <TestTypeSpecific />" << std::endl;
  output << "   <AgentRule name=\"Execution Agents\">" << std::endl;
  output << "   </AgentRule>" << std::endl;
  output << " </Execution>" << std::endl;
  output << " <Deployment runDeploymentRoot=\"" << name << "\" />" << std::endl;
  output << "</TestSettings>" << std::endl;

  output << "<ResultSummary outcome=\"" << (unit_test.Passed() ? "Passed" : "Failed") << "\">" << std::endl;
  output << "  <Counters total=\"" << total << "\" executed=\"" << execed << "\" passed=\"" << passed << "\" failed=\"" << failed << "\"/>" << std::endl;
  output << "</ResultSummary>" << std::endl;

  output << "<TestDefinitions>" << std::endl;

  for (std::size_t i = 0; i < m_Tests.size(); ++i)
  {
  Test &test = m_Tests[i];

  output << "  <UnitTest id=\"" << test.UnitTest << "\" storage=\"geomec3\\x64\\" << TestLib::getConfiguration() << "\\Test.exe\" name=\"" << test.Method << "\">" << std::endl;
  output << "    <Description>" << test.Class << "." << test.Method << "</Description>" << std::endl;
  output << "    <Execution id=\"" << test.Execution << "\"/>" << std::endl;
  output << "    <TestMethod codeBase=\"geomec3\\x64\\" << TestLib::getConfiguration() << "\\Test.exe\" className=\"" << test.Class << ", Test, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null\" name=\"" << test.Method << "\" />" << std::endl;
  output << "  </UnitTest>" << std::endl;
  }
  output << "</TestDefinitions>" << std::endl;
  output << "<TestLists>" << std::endl;
  output << "  <TestList name=\"All Loaded Results\" id=\"" << TestLib::getGUID() << "\" />" << std::endl;
  output << "  <TestList name=\"Results Not in a List\" id=\"" << m_TestListID << "\" />" << std::endl;
  output << "</TestLists>" << std::endl;

  output << "<TestEntries>" << std::endl;
  for (std::size_t i = 0; i < m_Tests.size(); ++i)
  {
  Test &test = m_Tests[i];

  output << "  <TestEntry testId=\"" << test.UnitTest << "\" executionId=\"" << test.Execution << "\" testListId=\"" << m_TestListID << "\" />" << std::endl;
  }
  output << "</TestEntries>" << std::endl;
  
  output << "<Results>" << std::endl;
  for (std::size_t i = 0; i < m_Tests.size(); ++i)
  {
  Test &test = m_Tests[i];

  // TODO: find out where testType comes from
  output << "  <UnitTestResult executionId=\"" << test.Execution << "\" testId=\"" << test.UnitTest << "\" testListId=\"" << m_TestListID << "\" outcome=\"" << (test.Result ? "Passed" : "Failed") << "\" ";
  output << "testName=\"" << test.Method << "\" testType=\"13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b\" computerName=\"" << TestLib::getHost() << "\" relativeResultsDirectory=\"";
  output << test.Execution << "\" duration=\"" << test.Duration << "\" startTime=\"" << test.Start << "\" endTime=\"" << test.End << "\">" << std::endl;
  if (!test.Result)
  {
      output << "    <Output>" << std::endl;
      output << "      <ErrorInfo>" << std::endl;
      output << "        <Message>" << std::endl;

      for (size_t j = 0; j < test.Messages.size(); ++j)
      {
         output << test.Messages[j] << std::endl;
      }

      output << "        </Message>" << std::endl;
      output << "      </ErrorInfo>" << std::endl;
      output << "    </Output>" << std::endl;

  }
  output << "  </UnitTestResult>" << std::endl;
  }
  output << "</Results>" << std::endl;

  output << "</TestRun>" << std::endl;

  output.close();
}


void RTXPrinter::OnTestStart(const TestInfo& test_info)
{
  m_Tests.push_back(Test());

  Test &test = m_Tests.back();

  test.Start     = TestLib::getDateTimeEx();
  test.UnitTest  = TestLib::getGUID();
  test.Execution = TestLib::getGUID();
  test.Class     = test_info.test_case_name();
  test.Method    = test_info.name();
}

void RTXPrinter::OnTestPartResult(const TestPartResult& test_part_result)
{
  //
  // FIXME: mcr 2020-07-07
  // crash here is the result of something wrong before
  // so this should be only temporary 
  //
  std::string msg;
  if(test_part_result.file_name())
    msg = test_part_result.file_name();
  //
  // this crashes if test_part_result.file_name() return nullptr
  //
  //std::string msg = test_part_result.file_name();
  //

  msg.append("(");

  std::ostringstream stream;
  stream << test_part_result.line_number();
  msg.append(stream.str());

  msg.append("): ");
  msg.append(test_part_result.message());

  m_Tests.back().Messages.push_back(msg);
}

void RTXPrinter::OnTestEnd(const TestInfo& test_info)
{
  Test &test = m_Tests.back();

  test.Result = test_info.result()->Passed();

  unsigned long int ms = (unsigned long int)test_info.result()->elapsed_time(); // in millis
  unsigned long int sc = ms / 1000;
  unsigned long int mn = sc / 60;
  unsigned long int hr = mn / 60;
  mn %= 60;
  sc %= 60;
  ms %= 1000;
  char tm[24];
  sprintf(tm, "%02lu:%02lu:%02lu.%03lu", hr, mn, sc, ms);
  
  test.Duration = tm;
  test.End = TestLib::getDateTimeEx();
}







