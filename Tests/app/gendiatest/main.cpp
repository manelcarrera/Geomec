// TestConsole.cpp : Defines the entry point for the console application.
//

#include <QDir>
#include <QString>
#include <iostream>

#include "AnalysisLogger.h"
#include "AnalysisType.h"
#include "ApplicationInitialization.h"
#include "DepletionStage.h"
#include "DianaEnv.h"
#include "DianaExecuter.h"
#include "GeomecUtils.h"
#include "ModelBase.h"
#include "ModelLifetimeFacade.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "SaveModelConsole.h"

namespace {

const QString APP_VERSION = "gendiatest 1.0";

typedef enum { NONE, L, N, M, C, H } TAnalysisType;

struct TArgs {
  TArgs() : antype(NONE), quadratic(false), restart(false) {}

  QString modelfile;
  TAnalysisType antype;
  bool quadratic;
  bool restart;
};

static bool ParseArguments(TArgs &args, int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    char *p = argv[i];
    if (p) {
      if (*p == '-') {
        ++p;
        while (*p) {
          switch (*p) {
          case 'l':
          case 'L':
            if (args.antype != NONE)
              return false;
            args.antype = L;
            break;
          case 'n':
          case 'N':
            if (args.antype != NONE)
              return false;
            args.antype = N;
            break;
          case 'm':
          case 'M':
            if (args.antype != NONE)
              return false;
            args.antype = M;
            break;
          case 'h':
          case 'H':
            if (args.antype != NONE)
              return false;
            args.antype = H;
            break;
          case 'q':
          case 'Q':
            if (args.quadratic)
              return false;
            args.quadratic = true;
            break;
          case 'r':
          case 'R':
            if (args.restart)
              return false;
            args.restart = true;
            break;
          default:
            return false;
          }

          ++p;
        }
      } else {
        // assume model file name
        if (!args.modelfile.isEmpty())
          return false;
        args.modelfile = argv[i];
      }
    }
  }

  if (args.antype == NONE)
    return false;

  if (args.modelfile.isEmpty())
    return false;

  if (args.quadratic && args.antype != L && args.antype != N) {
    fprintf(stderr, "warning: -q flag not used in combination with -l or -n, ignored\n");
    args.quadratic = false;
  }

  if (args.restart && args.antype != N) {
    fprintf(stderr, "warning: -r flag not used in combination with -n, ignored\n");
    args.restart = false;
  }

  return true;
}

static const char *ProgName(const char *argv0) {
  const char *p = strrchr(argv0, '/');
  if (!p)
    p = strrchr(argv0, '\\');

  if (p)
    ++p;

  if (p && *p)
    return p;

  return argv0;
}

static void Usage(const char *argv0) {
  fprintf(stderr, "Usage: %s <modelfile> <-l|-n|-m|-h> [-q|-r]\n", ProgName(argv0));
  fprintf(stderr, " Mandatory arguments:\n");
  fprintf(stderr, "  modelfile : path to the GEOMEC modelfiles\n");
  fprintf(stderr, "  -l        : linear static analysis\n");
  fprintf(stderr, "  -n        : nonlinear analysis\n");
  fprintf(stderr, "  -m        : mixture analysis\n");
  fprintf(stderr, "  -h        : heat flow analysis\n");
  fprintf(stderr, "\n");
  fprintf(stderr, " Optional arguments:\n");
  fprintf(stderr, "  -q        : quadratic elements (only used with -l or -n)\n");
  fprintf(stderr, "  -r        : create restarts (only used with -n)\n");
  fprintf(stderr, "\n");
}

CModelBase *openDocument(QString fileName, CAnalysisLogger &logger) {
  CModelBase *pModel = 0;
  pModel = IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(), fileName, logger);

  return pModel;
}

} // anonymous namespace

int main(int argc, char *argv[]) {
  QString strCurrentDir = QDir::currentPath();

  CApplicationInitialization consoleApplicationInitialization;

  TArgs args;
  if (!ParseArguments(args, argc, argv)) {
    Usage(argv[0]);
    return 1;
  }

  SetDianaEnvironment();

  CAnalysisLogger logger;
  CModelBase *pModel = openDocument(args.modelfile, logger);
  if (pModel) {
    QString sModelPath;
    QString sModelFileName;
    SplitPathAndFileName(args.modelfile, sModelPath, sModelFileName);
    QString sModelName = RemoveExtension(sModelFileName);

    CDianaExecuter diaexec;
    CSaveModelConsole smc(*pModel, APP_VERSION);
    CRetrieveDianaFileNamesConsole retr(strCurrentDir);

    CAnalysisType::TAnalysisType antype = CAnalysisType::AT_LINEAR;
    switch (args.antype) {
    case L:
      antype = CAnalysisType::AT_LINEAR;
      sModelName += "_l";
      break;
    case N:
      antype = CAnalysisType::AT_NONLIN;
      sModelName += "_n";
      break;
    case M:
      antype = CAnalysisType::AT_MIXTURE;
      sModelName += "_m";
      break;
    case C:
      antype = CAnalysisType::AT_MIXTURE_CONTAINMENT;
      sModelName += "_c";
      break;
    case H:
      antype = CAnalysisType::AT_HEAT;
      sModelName += "_h";
      break;
    default:
      assert(false);
    }

    if (args.quadratic)
      sModelName += "_q";

    if (args.restart) {
      sModelName += "_r";

      // set restart mode for all depletion stages
      CDepletionStageEntry::iterator it;
      for (it = pModel->DepletionStageEntry().begin(); it != pModel->DepletionStageEntry().end(); ++it) {
        if (it->OutputType() != CDepletionStage::BRANCH && it->OutputType() != CDepletionStage::PHASE)
          it->setOutputType(CDepletionStage::PHASE); // we're only interested in the restart, not in the generated files
      }
    }

    pModel->WriteFilosModel(args.modelfile, &diaexec, sModelName.toStdString(), antype, true, args.quadratic, smc,
                            retr);
  } else {
    std::cerr << "Unable to open modelfile " << args.modelfile.toStdString().c_str() << std::endl;
  }

  return 0;
}
