#ifdef WIN32
#define VC_EXTRALEAN
#include <windows.h>
#else
#include <sys/syscall.h>
#include <unistd.h>
#include <asm/unistd.h>
#endif

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#include <Lmcons.h>
#else
#include <uuid/uuid.h>
#endif
}


#include "TestLib.h"
#include "version.h"

//#define DEBUG_FILE_DIFFERENCES
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <cctype>
#include <ctime>


#include "AnalysisLogger.h"
#include "VersionNumbers.h"
#include "ModelLifetimeFacade.h"
#include "GeomecUtils.h"
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "SaveModelTest.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "DianaRunController.h"
#include "HexaModel.h"
#include "BaseEntryTypes.h"
#include "ExportResultData.h"
#include "ExportResultDataEntry.h"
#include "Printer.h"
#include "ISettings.h"
#include "SettingsIni.h"
#include "Global.h"
#include "QUtil.h"

//
// FIXME
//
#include "RunAnalysis_CLI.h"


namespace
{
  Printer* printer = Printer::instance(Printer::Tests);
}

namespace test_lib
{



std::string &FileCompare::ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

std::string &FileCompare::rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

std::string &FileCompare::trim(std::string &s) {
  return ltrim(rtrim(s));
}


// taking care of two issues in different streams writing different scientific notations:
// 'E+' becomes 'e+'
// 'e+0##' becomes 'e+##' for digits #
std::string &FileCompare::normalizeScientific(std::string &s)
{
  size_t i = 0;
  size_t m = s.size();

  while (i < m)
  {
  if (s[i] == 'E' || s[i] == 'e')
  {
      size_t n = i + 1;
      if (n < m && (s[n] == '-' || s[n] == '+'))
      {
    s[i] = 'e';
    
    ++n;

    if (n + 2 < m && s[n] == '0' && std::isdigit(s[n+1]) && std::isdigit(s[n+2]))
    {
          s.erase(n, 1);
          m = s.size();
          i += 2; // for the two digits
    }
    ++i; // for handling the +/-
      }
  }
  ++i;
  }

  return s;
}


bool FileCompare::Identical(std::string file_0, std::string file_1)
{
  std::ifstream f0(file_0.c_str(), std::ifstream::in | std::ifstream::binary);
  std::ifstream f1(file_1.c_str(), std::ifstream::in | std::ifstream::binary);

  // TODO: faster implementation
  return std::equal(std::istreambuf_iterator<char>(f0), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>(f1));
}

bool FileCompare::IdenticalText(std::string file_0, std::string file_1)
{
  std::ifstream f0(file_0.c_str(), std::ifstream::in);
  std::ifstream f1(file_1.c_str(), std::ifstream::in);

  if (!f0.is_open() || !f1.is_open())
  return false;

  while (!f0.eof() && !f1.eof())
  {
  std::string s0, s1;
  
  std::getline(f0, s0);
  std::getline(f1, s1);

  rtrim(s0);
  rtrim(s1);

  normalizeScientific(s0);
  normalizeScientific(s1);

  if (s0 != s1)
  {
#ifdef DEBUG_FILE_DIFFERENCES
      for (int i = 0; i < s0.size(); ++i)
      {
    if (i >= s1.size() || s0[i] != s1[i])
    {
          std::cout << i << " 0:" << (int)s0[i] << " ";
          if (i < s1.size())
      std::cout << i << " 1:" << (int)s1[i] << " ";
    }

      }
      std::cout << std::endl;
#endif
      return false;
  }
  }

  if (!f0.eof() || !f1.eof())
  return false;

  f0.close();
  f1.close();

  return true;
}



bool FileCompare::IdenticalTokens(std::string file_0, std::string file_1, double epsilon)
{

  struct printTokens
  {
  printTokens(std::size_t count, std::string token_0, std::string token_1)
  {
      std::cout << "TOKENS #" << count << " differ:" << std::endl;
      std::cout << "  Token 0: '" << token_0 << "'" << std::endl;
      std::cout << "  Token 1: '" << token_1 << "'" << std::endl;
  }
  printTokens(std::size_t count, std::string token_0, std::string token_1, double d_0, double d_1, double epsilon, double error, double relative_error)
  {
      printTokens(count, token_0, token_1);

      std::streamsize precision = std::cout.precision();

      std::cout << "  Resulting doubles:" << std::endl;
      std::cout << "    Double 0: " << d_0 << " " << std::setprecision(20) << d_0 << std::endl;
      std::cout.precision(precision);
      std::cout << "    Double 1: " << d_1 << " " << std::setprecision(20) << d_1 << std::endl;
      std::cout.precision(precision);
      std::cout << "    Epsilon : " << epsilon << " " << std::setprecision(20) << epsilon << std::endl;
      std::cout.precision(precision);
      std::cout << "    Error   : " << error << " " << std::setprecision(20) << error << std::endl;
      std::cout.precision(precision);
      std::cout << "    Relative: " << relative_error << " " << std::setprecision(20) << relative_error << std::endl;
      std::cout.precision(precision);
  }
  };


  std::ifstream f0(file_0.c_str(), std::ifstream::in|std::ifstream::binary);

  if (!f0.is_open())
  return false;

  std::ifstream f1(file_1.c_str(), std::ifstream::in|std::ifstream::binary);

  if (!f1.is_open())
  {
  f0.close();
  return false;
  }

  std::size_t count = 0;

  while (!f0.eof() && !f1.eof())
  {
  std::string token_0;
  std::string token_1;

  ++count;

  f0 >> token_0;
  f1 >> token_1;

  if (token_0 != token_1)
  {
      // let's find out why they differ

      if (token_0.find('.') != std::string::npos || token_1.find('.') != std::string::npos || // is one a double? (assume the other is too)
          token_0.find('e') != std::string::npos || token_1.find('e') != std::string::npos ||
          token_0.find('E') != std::string::npos || token_1.find('E') != std::string::npos ) 
      {
    double d_0 = atof(token_0.c_str());
    double d_1 = atof(token_1.c_str());

    double error          = fabs(d_0 - d_1);
    double relative_error = fabs(error / d_0);

    if (error > epsilon && relative_error > epsilon)
    {
          printTokens(count, token_0, token_1, d_0, d_1, epsilon, error, relative_error);
          return false;
    }
      }
      else if (token_0.size() == token_1.size()) // let's try to ignore case
      {
    for (std::size_t i = 0; i < token_0.size(); ++i)
    {
          if (tolower(token_0[i]) != tolower(token_1[i]))
          {
      printTokens(count, token_0, token_1);
      return false;
          }
    }
      }
      else
      {
    printTokens(count, token_0, token_1);
    return false;
      }
  }
  }

  if (!f0.eof() || !f1.eof())
  return false;

  f0.close();
  f1.close();

  return true;
}








char TestLib::test_model_path[256];
char TestLib::test_exec_path[256];
std::map<unsigned int, QTextStream*> TestLib::m_osMap;





const char *TestLib::ModelPath()
{
  return test_model_path;
}

bool TestLib::setModelPath(const char *model_path)
{
  memset(test_model_path, 0, sizeof(test_model_path));

  if (strlen(model_path) > 255)
  return false;

  memcpy(test_model_path, model_path, strlen(model_path));

  size_t len = strlen(test_model_path);
  char *last = &test_model_path[len-1];
  if (*last != '/' && *last != '\\')
  {
  if (len > 254)
      return false;

    *++last = '/';
    *++last = '\0';
  }

  return true;
}

const char *TestLib::ExecPath()
{
  return test_exec_path;
}

void TestLib::setExecPath(const char *exec_path)
{
  size_t len = strlen(exec_path);
  if (len > 255) len = 255;

  memset(test_exec_path, 0, sizeof(test_exec_path));
  memcpy(test_exec_path, exec_path, len);

  for (int i = (int)len; i >= 0 && test_exec_path[i] != '/' && test_exec_path[i] != '\\'; --i)
  test_exec_path[i] = '\0';
}




unsigned int TestLib::getThreadID()
{
#ifndef WIN32
  return syscall(__NR_gettid);
#else
  return GetCurrentThreadId();
#endif
};


QTextStream* TestLib::CurrentOutput()
{
  std::map<unsigned int, QTextStream *>::iterator it = m_osMap.find(getThreadID());

  QTextStream *result = it != m_osMap.end() ? it->second : 0;

  return result;
}

void TestLib::CurrentOutput(QTextStream *os)
{
  if (os)
  m_osMap[getThreadID()] = os;
  else
  m_osMap.erase(getThreadID());
}


std::string TestLib::getDateTime()
{
  time_t tt = time(0);
  struct tm *now = localtime(&tt);

  char date[20];
  sprintf(date, "%04d-%02d-%02d %02d_%02d_%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  return std::string(date);
}

std::string TestLib::getDateTimeEx()
{
  time_t tt = time(0);
  struct tm *now = localtime(&tt);

  char date[30];
  sprintf(date, "%04d-%02d-%02dT%02d:%02d:%02d.%06d+%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, 0, 1);

  return std::string(date);
}

std::string TestLib::getGUID()
{
#ifdef WIN32
  UUID uuid;
  UuidCreate(&uuid);

  unsigned char *str;
  UuidToStringA (&uuid, &str);

  std::string s((char *)str);

  RpcStringFreeA (&str);
#else
  uuid_t uuid;
  uuid_generate_random(uuid);
  char s[37];
  uuid_unparse (uuid, s);
#endif
  return s;
}


std::string TestLib::getUser()
{
#ifdef WIN32
  char username[UNLEN + 1];
  DWORD size = sizeof(username);
  GetUserName(username, &size);
  return username;
#else
  // while the code below is correct on linux, it returns garbage when the program is run from a (putty) ssh connection
  // so for now we return an empty string as workaround, so that we can proceed with testing the TFS-linux link
  return "";
#if 0
  char username[256];
  getlogin_r(username, 255);
  return username;
#endif
#endif
}

std::string TestLib::getHost()
{
#ifdef WIN32
  TCHAR hostname[256];
  DWORD size = sizeof(hostname);

  GetComputerNameEx((COMPUTER_NAME_FORMAT)0, hostname, &size);
  return hostname;
#else
  char hostname[256];
  gethostname(hostname, 255);
  return hostname;
#endif
}


std::string TestLib::getOS()
{
#ifdef WIN32
  return "Windows";
#else
  return "Linux";
#endif
}


std::string TestLib::getConfiguration()
{
#ifdef _DEBUG
  return "Debug";
#else
  return "Release";
#endif
}


int TestLib::getTimeZone() // rather crude
{
  time_t tt = time(0);
  struct tm *now = localtime(&tt);

  return now->tm_isdst > 0 ? 2 : 1;
}


QString TestLib::getDefaultAppVersion()
{
  static QString _appVersion = "Geomec " xstr(GM_MAJOR) "." xstr(GM_MINOR);
  return _appVersion;
}

CAnalysisLogger *TestLib::getDefaultAnalysisLogger()
{
  static CAnalysisLogger _logger;
  return &_logger;
}

CModelBase* TestLib::newModel(CAnalysisLogger *pLogger)
{
  CModelBase* pModel = IModelLifetimeFacade::NewMain(HEXA_MODEL, *pLogger, CVersionManager(test_lib::TestLib::getDefaultAppVersion()));

  pModel->NewModel(false);

  return pModel;
}

CModelBase* TestLib::loadModel(QString fileName, CAnalysisLogger *pLogger)
{
  return IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(), fileName, *pLogger);
}

bool TestLib::saveModel(CModelBase& model, QString sModelFile)
{
  return IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(), model, sModelFile, true);
}

//
// run all
//
int TestLib::runModel(CModelBase *pModel, QString sModelFile, CAnalysisType::TAnalysisType anyType, QString sAppVersion)
{
  controller::Params p = controller::Params{ *pModel, anyType, false, false, false, GetGeomecTempPathExt( CTempPath::TEMP_CALCULATION ) };
  return run_model( p, sModelFile,"", sAppVersion );
}

//
// url: model full path
// url_res: export input files generated files: .dcf, .dat
//
int TestLib::run_model(	const controller::Params& pp, 
            const QString& url, 
            const QString& results_path, 
            QString app_version )
{
  if(!&pp.model)
  {
    std::cerr << "Unable to open modelfile " << url.toStdString().c_str() << std::endl;
    return 1;
  }

  QString path;
  QString file;
  SplitPathAndFileName(url, path, file);
  QString name = RemoveExtension(file);

  CDianaExecuter diaexec;
  CSaveModelConsole smc(pp.model, app_version); 
  //
  //CSaveModelTest smc(pp.model, sAppVersion);	// this doesn't save the input files
  //
  CRetrieveDianaFileNamesConsole retr( results_path );

  CDianaRunController drc( pp );
  
  dia::IDianaRunner::RunParams p = { url, name.toStdString(), drc.tmp().toStdString(), &diaexec, &smc, &retr, nullptr, false, _g->dsa() };
  drc.params(p);

  bool res=false;

  if (p.dsa)
  {
    CRunAnalysis_CLI ra(&drc, CDianaRunController::All);
    ra.wait();
    res = ra.res(); // true is success
  }
  else
  {
    res = drc.run(CDianaRunController::All);
  }

  if (!res)
    return 1;

  return 0; // 0 is success
}

int TestLib::runModelWithSave(CModelBase *pModel, QString sModelFile, CAnalysisType::TAnalysisType anyType, QString sAppVersion)
{
  if (!pModel)
  {
  std::cerr << "Unable to open modelfile " << sModelFile.toStdString().c_str() << std::endl;
  return 1;
  }

  QString sModelPath;
  QString sModelFileName;
  SplitPathAndFileName(sModelFile, sModelPath, sModelFileName);
  QString sModelName = RemoveExtension(sModelFileName);

  CDianaExecuter diaexec;
  CSaveModelTest smc(*pModel, sAppVersion);
  CRetrieveDianaFileNamesConsole retr;

  CDianaRunController drc(*pModel, anyType, false, false, false, GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION));

  dia::IDianaRunner::RunParams p = { sModelFile, sModelName.toStdString(), drc.tmp().toStdString(), &diaexec, &smc, &retr, nullptr, false, _g->dsa() };
  drc.params(p);

  bool res = false;

  if (p.dsa)
  {
  CRunAnalysis_CLI ra(&drc, CDianaRunController::All);
  ra.wait();
  res = ra.res(); // true is success
  }
  else
  {
  res = drc.run(CDianaRunController::All);
  }

  if (!res)
  return 1;

  return 0; // 0 is success
}

//
// run 'First' once + run 'Next' n times
//
// code is different from the method before: 'runModel'
//
// TODO: what are the use case of each one? can be unified? (in order to run one only code) 
//
int TestLib::runStepsModel(CModelBase *pModel, QString sModelFile, QString sAppVersion)
{
  if(!pModel)
  {
    std::cerr << "Unable to open modelfile " << sModelFile.toStdString().c_str() << std::endl;
    return 1;
  }

  CDepletionStage* depletionStage = &(pModel->DepletionStageEntry().LastStage());

  while (depletionStage && !depletionStage->Initial())
  {
    depletionStage->setOutputType(CDepletionStage::PHASE);
    depletionStage = &(depletionStage->Previous());
  }

  QString sModelPath;
  QString sModelFileName;
  SplitPathAndFileName(sModelFile, sModelPath, sModelFileName);
  QString sModelName = RemoveExtension(sModelFileName);

  CDianaExecuter diaexec;
  CSaveModelConsole smc(*pModel, sAppVersion);
  CRetrieveDianaFileNamesConsole retr;

  CDianaRunController drc(*pModel, CAnalysisType::AT_NONLIN, false, false, false, GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION));

  dia::IDianaRunner::RunParams p = { sModelFile, sModelName.toStdString(), drc.tmp().toStdString(), &diaexec, &smc, &retr, nullptr, false, _g->dsa() };
  drc.params(p);

  bool res=false;

  if (p.dsa)
  {
    CRunAnalysis_CLI ra(&drc, CDianaRunController::First);
    ra.wait();
    res = ra.res(); // true is success
  }
  else
  {
    res = drc.run(CDianaRunController::First);
  }

  if (!res)
    return 1;

  while (!drc.EndStage().Last())
  {
    if (p.dsa)
    {
      CRunAnalysis_CLI ra(&drc, CDianaRunController::Next);
      ra.wait();
      res = ra.res(); // true is success
    }
    else
    {
      res = drc.run(CDianaRunController::Next);
    }

    if (!res)
      return 1;
  }
  //
  // TODO: verify if with last updates it's still needed
  //
  // this was crashing in ModelOperationsBatch in the test after doing 'run 1'
  //

  //
  // this releases resources to leave a clean environment for next execution
  //
  drc.push_(ClearController);
  //
  // FIXME: race condition
  //
  std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 200ms., not enough / 300ms. is Ok // 500ms. just in case
  //
  //
  //
  return 0; // 0 is success
}

void TestLib::closeModel(CModelBase **pModel)
{
  (*pModel)->setDeleting(true);
  IModelLifetimeFacade::Close(*pModel);
  (*pModel) = 0;
}

const IPointSet *TestLib::getPointSet(const CModelBase *pModel, const QString& pointset)
{
  const TPointSetEntry& entry = static_cast<const TPointSetEntry&>(*pModel->GraphEntry(MD_BASE_POINTSET));
  const TPointSetEntry::TNodeSet stNodes = entry.EntryNodes();

  for (TPointSetEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  if ((*it)->Name() == pointset)
      return *it;
  }

  return nullptr;
}


void TestLib::importResultMacroLibrary(TMacros& macros, CModelBase *pModel, QString sExportMacroLib)
{
  CExportResultDataEntry *pExportResultDataEntry = dynamic_cast<CExportResultDataEntry *>(pModel->GraphEntry(MD_BASE_EXPORT_MACROS));

  if (pExportResultDataEntry)
  {
  if (!sExportMacroLib.isEmpty())
      pExportResultDataEntry->Import(sExportMacroLib);

  CExportResultDataEntry::TNodeSet stNodes = pExportResultDataEntry->EntryNodes();

  std::copy(stNodes.begin(), stNodes.end(), std::inserter(macros, macros.begin()));
  }
}

void TestLib::Log(const char * msg )
{
  // to be implemented
  //Printer::instance()->debug( "%s", msg );
  printer->debug("log : test : %s", msg);
}

void TestLib::Log(QString *msg)
{
  if (msg)
  Log(msg->toStdString().c_str());
}

}
