#ifndef _MODEL_OPERATIONS_H_
#define _MODEL_OPERATIONS_H_

#include "AnalysisLogger.h"
#include "TestLib.h"
#include <gtest/gtest.h>

#include <vector>

namespace test_model_operations {

int LibraryLoad_ModelOperations();
int LibraryLoad_SpecialModelOperations();

//
// input: geomec_shell command line parameters
//
// output: geomec_shell response dumped to std_out to be parsed and evaluated
//
void execute_geomec_shell(const char *params, QString *captureOutput = 0);
void execute_geomec_cora(const char *params, QString *captureOutput = 0);

class ModelOperations : public ::testing::Test {
protected:
  QString m_ModelsPath;
  CAnalysisLogger m_Logger;

  ModelOperations() : m_ModelsPath(test_lib::TestLib::ModelPath()) {}
};

class ModelOperationsExport : public ::testing::Test {
protected:
  QString m_ModelsPath;
  CAnalysisLogger m_Logger;

  QString m_model;

  ModelOperationsExport() : m_ModelsPath(test_lib::TestLib::ModelPath()), m_model("ResultsBranched.gm5") {}
};

class ModelOperationsDsa : public ::testing::Test {
protected:
  QString m_ModelsPath;
  CAnalysisLogger m_Logger;

  ModelOperationsDsa() : m_ModelsPath(test_lib::TestLib::ModelPath()) {}
};

class ModelOperationsIpc : public ::testing::Test {
protected:
  ModelOperationsIpc() {}
};

class ModelOperationsGui : public ::testing::Test {
protected:
  ModelOperationsGui() {}
};

class ModelOperationsCORA : public ::testing::Test {
protected:
  QString m_ModelsPath;
  QString m_OutputPath;
  CAnalysisLogger m_Logger;

  ModelOperationsCORA();

  void some_grouped_tests();
};

class ModelOperationsRGI : public ::testing::Test {
protected:
  QString m_ModelsPath;
  QString m_OutputPath;
  CAnalysisLogger m_Logger;

  ModelOperationsRGI();

  void some_grouped_tests();
  void monitoring_points_test();

  void monitoring_points_problem_test();

  void log_messages_test();
};

class ModelOperationsSkua : public ::testing::Test {
protected:
  QString m_ModelsPath;
  QString m_OutputPath;
  CAnalysisLogger m_Logger;

  ModelOperationsSkua();

  void import_test();
};

struct Command {
  typedef enum {
    NOP = 0, // params:
    LOAD,    // m_ModelPath + filename, or if empty load previous path
    SAVE,    // save to previous path [must be under m_OutputPath]
    SAVE_AS, // save to m_OutputPath + filename
    CLOSE,
    RUN,    // 1 if in steps
    IMPORT, // mesh gocad.so | gocad gocad.so # # | petrel petrel.txt U U
    MESH    // 0 = invalidate, 1 = create, nothing = auto
  } Opcode;

  Opcode opcode;
  QString params;

  Command() : opcode(NOP) {}
};

struct Script {
  std::vector<Command> commands;

  Script(const char *);
};

class ModelOperationsBatch : public ::testing::TestWithParam<const char *> {
  QString m_ModelsPath;
  QString m_OutputPath;
  CAnalysisLogger m_Logger;

protected:
  ModelOperationsBatch();

  void execute();
};

typedef ModelOperationsBatch ModelOperationsShell;

} // namespace test_model_operations

#endif