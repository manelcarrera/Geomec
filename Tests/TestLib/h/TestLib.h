#ifndef _TEST_LIB_H__
#define _TEST_LIB_H__

#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>


#include "TestFormation.h"
#include "TestHorizon.h"
#include "TestMaterial.h"
#include "TestSoilModel.h"
#include "TestSoilModelFactory.h"


class CModelBase;
class CAnalysisLogger;
class CExportResultData;
class IPointSet;

#include "AnalysisType.h"

namespace controller{ struct Params; }

namespace test_lib
{





class FileCompare
{
public:
  static inline std::string &ltrim(std::string &s);
  static inline std::string &rtrim(std::string &s);
  static inline std::string &trim(std::string &s);

  static inline std::string &normalizeScientific(std::string &s);

  static bool Identical(std::string file_0, std::string file_1);
  static bool IdenticalText(std::string file_0, std::string file_1);
  static bool IdenticalTokens(std::string file_0, std::string file_1, double epsilon = 1e-6);
};





class TestLib
{
  static char test_model_path[256];
  static char test_exec_path[256];

  static std::map<unsigned int, QTextStream*> m_osMap;

public:
  static const char *ModelPath();
  static bool setModelPath(const char *model_path);

  static const char *ExecPath();
  static void setExecPath(const char *firstArg);

  static unsigned int getThreadID();

  // TODO: make these thread-safe (when we run tests in parallel, if ever)
  static QTextStream* CurrentOutput();
  static void CurrentOutput(QTextStream *os);

  static std::string getDateTime();
  static std::string getDateTimeEx();
  static std::string getGUID();
  static std::string getUser();
  static std::string getHost();
  static std::string getOS();
  static std::string getConfiguration();
  static int getTimeZone();
  static QString getDefaultAppVersion();
  static CAnalysisLogger *getDefaultAnalysisLogger();

  static CModelBase* newModel(CAnalysisLogger *pLogger = getDefaultAnalysisLogger());
  static CModelBase* loadModel(QString fileName, CAnalysisLogger *pLogger = getDefaultAnalysisLogger());
  static int runModel(CModelBase *pModel, QString sModelFile, CAnalysisType::TAnalysisType anyType = CAnalysisType::AT_NONLIN, QString sAppVersion = getDefaultAppVersion());
  //
  // to run Export
  //
  static int run_model(	const controller::Params& p, 
						const QString& sModelFile,
						const QString& results_path=QString(), 
						QString app_version  = getDefaultAppVersion() );

  static int runModelWithSave(CModelBase *pModel, QString sModelFile, CAnalysisType::TAnalysisType anyType = CAnalysisType::AT_NONLIN, QString sAppVersion = getDefaultAppVersion());
  static int runStepsModel(CModelBase *pModel, QString sModelFile, QString sAppVersion = getDefaultAppVersion());
  static bool saveModel(CModelBase& model, QString sModelFile);
  static void closeModel(CModelBase **pModel);

  static const IPointSet *getPointSet(const CModelBase *pModel, const QString& pointset);

  typedef std::set<CExportResultData *> TMacros;
  static void importResultMacroLibrary(TMacros& macros, CModelBase *pModel, QString sExportMacroLib = QString());

  static void Log(const char *);
  static void Log(QString *);
};


}


#endif