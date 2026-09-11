// Simple wrapper for starting Tests.exe, as GoogleTestRunner/Microsoft's framework
// can't use the project's environment settings.

#include <direct.h>
#include <cstdlib>
#include <sstream>
#include <iostream>

// all three in QUtil lib
#include "SettingsIni.h"
#include "Printer.h"
#include "QUtil.h"



const char *getExePath(const char *exe)
{
  static char *s = 0;
  
  if (!s)
  {
    s = _strdup(exe);
    for (size_t i = strlen(s) - 1; i >= 0; --i)
      if (*(s + i) != '\\' && *(s + i) != '/')
        *(s + i) = 0;
      else
        break;
  }

  return s;
}

void buildPath(const char *cwd)
{
  const char *oldPath = getenv("PATH");
  const char *qtDir   = getenv("QTDIR");
  const char *qwtDir  = getenv("QWTDIR");
  const char *diaRoot = getenv("DIAROOT");
  const char *intel   = getenv("INTEL");

  std::ostringstream newPath;
  newPath << "PATH=";
  newPath << qtDir << "\\bin;";
  newPath << qwtDir << "\\lib;";
#ifdef _DEBUG
  newPath << diaRoot << "\\debug\\bin;";
#else
  newPath << diaRoot << "\\release\\bin;";
#endif
  newPath << diaRoot << "\\share\\CM2\\libs;";
  newPath << diaRoot << "\\share\\SafeNet\\LDK741\\C;";
  newPath << intel << ";";
#if defined(GM_BUILD_TYPE_RELEASE_NIGHTLY) || defined(GM_BUILD_TYPE_RELEASE)
  newPath << "..\\src\\lib\\tbb\\windows\\bin\\intel64\\vc12;";
  newPath << "E:\\DevEnv\\Intel\\mkl;";
#else
  // in VS2013 our cwd is "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\CommonExtensions\Microsoft\TestWindow"
  // and GoogleTestRunner (or the MS API) doesn't pick up the debugging settings for environment
  newPath << cwd << "..\\..\\..\\lib\\tbb\\windows\\bin\\intel64\\vc12;";
  newPath << intel << "\\..\\mkl;";
#endif
  newPath << oldPath;

  _putenv(newPath.str().c_str());
}


#define GM_MODEL_PATH "--gm-model-path="

const char *buildCommand(int argc, char *argv[])
{
  std::ostringstream command;

  command  << "RunTsts.exe";

  bool sawModelPath = false;

  for (int i = 1; i < argc; ++i)
  {
    if (!strncmp(argv[i], GM_MODEL_PATH, strlen(GM_MODEL_PATH)))
      sawModelPath = true;
    command << " " << argv[i];
  }

  if (!sawModelPath)
#if defined(GM_BUILD_TYPE_RELEASE_NIGHTLY) || defined(GM_BUILD_TYPE_RELEASE)
	  command << " " << GM_MODEL_PATH << "../src/Tests/Models/";
#else
	  command << " " << GM_MODEL_PATH << "../../../Tests/Models/";
#endif

  return _strdup(command.str().c_str());
}


int main(int argc, char *argv[])
{
	std::string ver = CSettingsIni::instance()->version_str().toStdString();
	std::string url = QUtil::url(QUtil::eUsrDir::Goemec, "gm_" + ver + ".log");
	for (int i = 0; i < Printer::ModulesNum; i++)
		Printer::instance((Printer::eModule)i)->url(url);


#if defined(GM_BUILD_TYPE_RELEASE_NIGHTLY) || defined(GM_BUILD_TYPE_RELEASE)
  _chdir("E:");
#endif

  _chdir(getExePath(argv[0]));

  buildPath(getExePath(argv[0]));

  const char *command = buildCommand(argc, argv);

  //std::cout << command << std::endl;

  system(command);

  return 0;
}
