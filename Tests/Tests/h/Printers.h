#ifndef _TEST_PRINTERS_H__
#define _TEST_PRINTERS_H__


#include <gtest/gtest.h>



using namespace testing;




class RTXPrinter : public EmptyTestEventListener
{
public:
  RTXPrinter(const std::string &fileName);

private:
  // Called before all test activities start.
  virtual void OnTestProgramStart(const UnitTest& unit_test);

  // Called after all test activities have ended.
  virtual void OnTestProgramEnd(const UnitTest& unit_test);

  // Called before a test starts.
  virtual void OnTestStart(const TestInfo& test_info);

  // Called after a failed assertion or a SUCCEED() invocation.
  virtual void OnTestPartResult(const TestPartResult& test_part_result);

  // Called after a test ends.
  virtual void OnTestEnd(const TestInfo& test_info);


  const std::string m_FileName;

  std::string m_TestListID;


  struct Test
  {
  std::string UnitTest;
  std::string Class;
  std::string Method;
  std::string Execution;
  bool Result;
  std::vector<std::string> Messages;
  std::string Duration;
  std::string Start;
  std::string End;
  };

  std::vector<Test> m_Tests;

};






#endif