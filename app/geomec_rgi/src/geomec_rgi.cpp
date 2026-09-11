
#include <fstream>
#include <iterator>
#include <stdexcept>

#include "AnalysisLogger.h"
#include "ModelLifetimeFacade.h"
#include "ModelBase.h"
#include "GeomecUtils.h"
#include "DianaExecuter.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "SaveModelConsole.h"
#include "AnalysisType.h"
#include "DianaStartUp.h"
#include "RockMechProcessor.h"
#include "GlobalMessage.h"
#include "ValidateModel.h"
#include "VersionNumbers.h"

#include <cstring>
#include <cstdio>
#include <QDir>

#include "VersionManager.h"
#include "GoCadModel.h"
#include "TetraSubHorizon.h"
#include "TetraSubBoundary.h"
#include "version.h"
#include "ApplicationInitialization.h"

#include "ValueMapper.h"

#include "GlobalMessage_CLI.h"

#include "Printer.h"
#include "SettingsIni.h"
#include "QUtil.h"

#include "Global.h"
#include "ProgressFactory_CLI.h"

#pragma message("WARNING: geomec_rgi is deprecated; see WI 116602")
/*
Functionality of geomec_rgi is now handled by InterfaceRGI and the main loop of geomec_shell.

There is a grace period so that both geomec_shell and geomec_rgi can be tested (regression).
Changes to the geomec_rgi project should be avoided, but if that is not possible, they should be
implemented here and in geomec_shell.
*/

namespace {

const QString APPLICATION_NAME = "GEOMEC " xstr(GM_MAJOR) "." xstr(GM_MINOR);
const QString APP_VERSION = "RockMech Processor " xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX " " xstr(GM_BUILD_STRING);

// return codes:
const int RET_OK       = 0; // ok
const int RET_ARGS     = 1; // wrong arguments
const int RET_LOADFAIL = 2; // failed to load model
const int RET_EXECFAIL = 3; // failed to execute rockmech
const int RET_LOGFAIL  = 4; // failed to open logfile

}

static const char* ProgName_DEPRECATED(const char* argv0)
{
  const char* p = strrchr(argv0, '/');
  if(!p)
    p = strrchr(argv0, '\\');

  if(p)
    ++p;

  if(p && *p)
    return p;

  return argv0;
}

static int Usage_DEPRECATED(const char* argv0)
{
  fprintf(stderr, "Usage: %s /ROCKMECHFILE:<rockmechfile> <modelfile> [/OUTPUTDIR:<dir>] [/LOG:<filename>] [/TEMPDIR:<dir>]\n", ProgName_DEPRECATED(argv0));
  fprintf(stderr, "       %s /ROCKMECHFILE:<rockmechfile> [/OUTPUTDIR:<dir>] [/LOG:<filename>] [/TEMPDIR:<dir>]\n", ProgName_DEPRECATED(argv0));
  fprintf(stderr, "       %s --version\n", ProgName_DEPRECATED(argv0));
  fprintf(stderr, "\n");

  return RET_OK;
}

static int Version_DEPRECATED()
{
  fprintf(stdout, "** %s\n", APP_VERSION.toStdString().c_str());
  fprintf(stdout, "** Supports files up to version (%d, %d, %d)\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);

  return RET_ARGS;
}

static void MakeAbsolutePath_DEPRECATED(QString& strPath)
{
  if(QDir::isRelativePath(strPath))
  {
    strPath = QDir::current().absoluteFilePath(strPath);
    strPath = QDir::current().cleanPath(strPath);
    strPath = QDir::toNativeSeparators(strPath);
  }
}

namespace
{


void copyFile_DEPRECATED(const QString& destination, const QString& source)
{
  if (source == destination)
  {
    return;
  }

  std::ifstream
    input(source.toStdString().c_str(), std::ios::in | std::ios::binary);
  std::ofstream
    output(destination.toStdString().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

  if (input.fail())
  {
    throw std::runtime_error((source +
      " could not be opened for reading").toStdString().c_str());
  }

  if (output.fail())
  {
    throw std::runtime_error((destination +
      " could not be opened for writing").toStdString().c_str());
  }

  output << input.rdbuf();

  if (output.fail())
  {
    throw std::runtime_error((destination +
      " could not be written to").toStdString().c_str());
  }

  // there *may* be an issue on windows with automatic flush/close on destruction of ofstream object,
  // so we do it explicitly. It can't hurt. (Bug 83505.)
  output.flush();
  output.close();
}

void moveFile_DEPRECATED(const QString& destination, const QString& source)
{
  if (source == destination)
  {
    return;
  }

  copyFile_DEPRECATED(destination, source);

  if (remove(source.toStdString().c_str()) != 0)
  {
    throw std::runtime_error((source +
      " could not be removed").toStdString().c_str());
  }
}

} // anonymous namespace

static bool RunRockMech_DEPRECATED(CModelBase& model, const QString& sRockMechFile, const QString& sModelFile, const QString& sOutputDir, const QString& sTempDir, bool validate, bool bAppendStageNumber)
{
  QString sModelPath;
  QString sModelFileName;
  SplitPathAndFileName(sModelFile, sModelPath, sModelFileName);
  QString sModelName = RemoveExtension(sModelFileName);

  CDianaExecuter diaexec;
  CSaveModelConsole smc(model, APP_VERSION);
  CRetrieveDianaFileNamesConsole retr;

  // when validate is true an existing model is implied

  GeomecRGI::CRockMechProcessor rmp(sRockMechFile, sOutputDir, sTempDir);
  rmp.SetModel(model, validate);
  rmp.AddLogLine(QObject::tr("Starting RockMechProcessor with filename '%1'").arg(sRockMechFile));

  if (validate)
  {
    CValidateModel validateModel(&model);

    if (!validateModel.checkModel(true))
    {
      rmp.AddLogLine(QObject::tr("Invalid Geomec model, not all calculation criteria are met!"));
      return false;
    }
  }

  int nStage = 0;
  bool bRet = true;
  bool bSaveModel = false;

  QString source = QDir::toNativeSeparators(sModelPath + "/" + sModelFileName);
  QString destination = QDir::toNativeSeparators(
    (sOutputDir.isEmpty() ? sModelPath : sOutputDir) + "/" + sModelFileName);

  copyFile_DEPRECATED(destination, source);

  rmp.executeRockMechCommands_DEPRECATED(nStage, sModelName.toStdString(), &diaexec, destination, bRet, smc, retr, bSaveModel, validate);

  moveFile_DEPRECATED(source, destination);

  if(bRet || bSaveModel)
  {
    QString fileName = rmp.getSaveModelName(sModelFile, nStage, bRet, bAppendStageNumber);

    if(!bRet || nStage == 0)
      IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(APP_VERSION), model, fileName, true);
  }

  return bRet;
}

bool GetParameter_DEPRECATED(const char* arg, const char* magic, const char** value)
{
  if(strlen(arg) > strlen(magic) &&
    (QString::compare(QString(arg).left(int(strlen(magic))),
      magic, Qt::CaseInsensitive) == 0))
  {
    if(!*value) // only allowed once
    {
      *value = arg + strlen(magic);
      return true;
    }
  }

  return false;
}

int main_DEPRECATED(int argc, char* argv[])
{
  GlobalMessage::init( new GlobalMessage_CLI() );
  CApplicationInitialization consoleApplicationInitialization;

  	Printer::instance()->url( QUtil::url( 
		QUtil::eUsrDir::Goemec, 
		"gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log" ) );
	Printer::instance()->info( "" );
	Printer::instance()->info( "gm_rgi > main" );

	// Config file
	CSettingsIni::instance()->init();
	CSettingsIni::instance()->print_();

	CDianaStartUp* dsu = CDianaStartUp::instance();
  	dsu->SetDianaEnv(); 
	dsu->Print_DianaEnv();

	_g->prog( new CProgressFactory_CLI );

	
  // parse arguments
  if (argc < 2)
  {
    Version_DEPRECATED();

    return Usage_DEPRECATED(argv[0]);
  }
  else if ((argc == 2) && (QString(argv[1]) == "--version"))
  {
    return Version_DEPRECATED();
  }

  const char* sRockMechFile = 0;
  const char* sOutputDir = 0;
  const char* sLog = 0;
  const char* sModelFile = 0;
  const char* sTempDir = 0;

  const char rockmagic[] = "/ROCKMECHFILE:";
  const char outputmagic[] = "/OUTPUTDIR:";
  const char logmagic[] = "/LOG:";
  const char tempmagic[] = "/TEMPDIR:";

  typedef struct
  {
    const char*  magic;
    const char** value;
  } TParameter;

  TParameter params[] =
  {
    { rockmagic, &sRockMechFile },
    { outputmagic, &sOutputDir },
    { logmagic, &sLog },
    { tempmagic, &sTempDir },
    { 0, 0 }
  };

  for (int i = 1; i < argc; ++i)
  {
    bool bParamFound = false;
    for (int j = 0; params[j].magic != 0; ++j)
    {
      if (GetParameter_DEPRECATED(argv[i], params[j].magic, params[j].value))
      {
        bParamFound = true;
        break;
      }
    }

    if (!bParamFound)
    {
      // assume model file
      if (sModelFile) // only allowed once
        return Usage_DEPRECATED(argv[0]);
      sModelFile = argv[i];
    }
  }

  // mandatory parameters
  if (!sRockMechFile)
    return Usage_DEPRECATED(argv[0]);

  // ok, let's give it a try

  QString strModelFile(sModelFile != 0 ? sModelFile : sRockMechFile);
  QString strOutputDir(sOutputDir ? sOutputDir : QString());
  QString strRockMechFile(sRockMechFile);
  QString strLogFile(sLog);
  QString strTempDir(sTempDir);

  // make absolute file paths
  MakeAbsolutePath_DEPRECATED(strModelFile);
  MakeAbsolutePath_DEPRECATED(strRockMechFile);
  MakeAbsolutePath_DEPRECATED(strTempDir);

  if (!strOutputDir.isEmpty())
    MakeAbsolutePath_DEPRECATED(strOutputDir);

  if (!strLogFile.isEmpty())
    MakeAbsolutePath_DEPRECATED(strLogFile);

  if (!strLogFile.isEmpty() && !_m()->openlogfile(strLogFile.toStdString()))
  {
    fprintf(stderr, "Unable to open log file '%s'\n", strLogFile.toStdString().c_str());
    return RET_LOGFAIL;
  }

  CAnalysisLogger logger;
  CModelBase* pModel = 0;
  QString version(APP_VERSION);
  CVersionManager versionManager(version);
  bool validate = false;

  if (sModelFile != 0)
  {
    pModel = IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(), strModelFile, logger);
    validate = true;
  }
  else
  {
    pModel = IModelLifetimeFacade::NewMain(GOCAD_MODEL, logger, versionManager);
    pModel->Name(strModelFile);
    pModel->NewModel(false);

    CTetraSubHorizon* top =
      new CTetraSubHorizon("Top Horizon", false, *pModel);
    CTetraSubHorizon* bottom =
      new CTetraSubHorizon("Bottom Horizon", false, *pModel);
    CTetraSubBoundary& boundary =
      dynamic_cast <CTetraModel*> (pModel)->SubBoundary();

    boundary.TopHorizon().LinkTo(*top);
    boundary.BottomHorizon().LinkTo(*bottom);

    strModelFile += ".gm5";
  }

  if (!pModel)
  {
    _m()->outstream() << "Failed to load model '" << strModelFile.toStdString() << "'" << std::endl;
    return RET_LOADFAIL;
  }

  // force optimization of filos files
  bool bForceCleanup = true;
  const char *option = getenv("RGI_NO_BRANCH_CLEANUP");
  if (option && (option[0] == 'y' || option[0] == 'Y'))
    bForceCleanup = false;

  pModel->CleanupOldBranchResults(bForceCleanup);

  try
  {
    CValueMapper::instance()->DianaDistributedMapping(false);
    
    if (!RunRockMech_DEPRECATED(*pModel, strRockMechFile, strModelFile, strOutputDir,
      strTempDir, validate, sModelFile != 0))
    {
      _m()->outstream() << "Failed to execute RockMech processing" << std::endl;
      return RET_EXECFAIL;
    }
  }

  catch (const std::exception& e)
  {
    _m()->outstream() << e.what() << std::endl;
    return RET_EXECFAIL;
  }

	return RET_OK;
}


int main(int argc, char* argv[])
{
  return main_DEPRECATED(argc, argv);
}