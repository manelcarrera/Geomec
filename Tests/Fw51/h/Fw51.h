#ifndef _FW51_H__
#define _FW51_H__

#include "TestLib.h"
#include <gtest/gtest.h>

// Some of these templates may be useful for other tests (not fw51) IF they also use the input/output scheme
// Also, this can be cleaned up a little further by using type traits; not sure yet what VS supports

namespace test_fw51 {

int LibraryLoad_Fw51();

#define TEST_INIT_INFO_MAXSIZE 8

template <class T> class TestInitInfo {
public:
  const char *message;
  void (*init)(T &);
};

template <class T> class TestInfo {
public:
  const char *fileBase;
  TestInitInfo<T> inits[TEST_INIT_INFO_MAXSIZE];
};

template <class T> std::ostream &operator<<(std::ostream &os, const TestInfo<T> &info) { return os << info.fileBase; }

template <class T> class Fw51BaseTestTemplate : public ::testing::TestWithParam<T> {
protected:
  std::string m_ModelsPath;
  std::string m_OutputPath;

  std::string m_FileBase;

  std::string m_Expected;
  std::string m_Output;

  Fw51BaseTestTemplate() : m_ModelsPath(test_lib::TestLib::ModelPath()), m_OutputPath(test_lib::TestLib::ModelPath()) {
    m_ModelsPath.append("Fw51Input/");
    m_OutputPath.append("Fw51Output/");
  }

  void SetUp() {
    m_FileBase = ::testing::TestWithParam<T>::GetParam().fileBase;

    m_Output = m_OutputPath + m_FileBase + ".new";
    m_Expected = m_ModelsPath + m_FileBase + ".res";
  }
};

class Fw51BaseTest : public ::testing::TestWithParam<const char *> {
protected:
  std::string m_ModelsPath;
  std::string m_OutputPath;

  std::string m_FileBase;

  std::string m_Expected;
  std::string m_Output;

  Fw51BaseTest() : m_ModelsPath(test_lib::TestLib::ModelPath()), m_OutputPath(test_lib::TestLib::ModelPath()) {
    m_ModelsPath.append("Fw51Input/");
    m_OutputPath.append("Fw51Output/");
  }

  void SetUp() {
    std::string fileName = GetParam();

    fileName = fileName.substr(0, fileName.find(' '));

    m_FileBase = fileName.substr(0, fileName.rfind('.'));

    m_Output = m_OutputPath + m_FileBase + ".new";
    m_Expected = m_ModelsPath + m_FileBase + ".res";
  }
};

template <class T> class Fw51InputBaseTestTemplate : public Fw51BaseTestTemplate<T> {
protected:
  std::string m_Input;

  void SetUp() {
    Fw51BaseTestTemplate<T>::SetUp();

    std::string fileName = ::testing::TestWithParam<T>::GetParam();

    fileName = fileName.substr(0, fileName.find(' '));

    m_Input = Fw51BaseTestTemplate<T>::m_ModelsPath + fileName;
  }
};

class Fw51InputBaseTest : public Fw51BaseTest {
protected:
  std::string m_Input;

  void SetUp() {
    Fw51BaseTest::SetUp();

    std::string fileName = GetParam();

    fileName = fileName.substr(0, fileName.find(' '));

    m_Input = m_ModelsPath + fileName;
  }
};

} // namespace test_fw51

#endif
