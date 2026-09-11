#include "ArgumentParser.h"
#include "InterfaceRGI.h"
#include "GeomecShellUtils.h"
#include "InterfaceContext.h"

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

#include "RGSync.h"
#include "GMCommand.h"
#include "RGInterface.h"
#include "BuildTensor.h"

#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "ValueMapper.h"

#include "InterfaceCORA.h"
#include "TetraEntryTypes.h"
#include "Pressure.h"
#include "Temperature.h"
#include "StrainLoad.h"
#include "DisplacementSupportNode.h"
#include "InterfaceBoundary.h"
#include "FaultPressure.h"
#include "FilosFatalHandler.h"
#include "GeomecFormationIDs2RGI.h"

#include "Printer.h"

namespace {

const QString APPLICATION_NAME = "GEOMEC " xstr(GM_MAJOR) "." xstr(GM_MINOR);
const QString APP_VERSION = "RockMechProcessor " xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX " " xstr(GM_BUILD_STRING);

// return codes:
const int RET_OK       = 0; // ok
const int RET_ARGS     = 1; // wrong arguments
const int RET_LOADFAIL = 2; // failed to load model
const int RET_EXECFAIL = 3; // failed to execute rockmech
const int RET_LOGFAIL  = 4; // failed to open logfile
}

namespace
{
Printer* printer = Printer::instance(Printer::Shell);

const char* rgi_cmd_cp[]={
  "RGI_SETUP", //= IInterfaceBase::FOR_ME + 1,
  "RGI_SHUTDOWN",
  "RGI_LOAD_GRID",
  "RGI_SAVE_GRID",
  "RGI_LOAD_PROPERTY",
  "RGI_SAVE_PROPERTY",
  "RGI_CALCULATE",
  "RGI_DEFINE_INITIAL_DATE",
  "RGI_QUIT",
  "RGI_HIBERNATE",
  "RGI_LIST_AVAILABLE_FAILURE_MECHANISMS",
  "RGI_EVALUATE_LIMIT_STATE_FUNCTIONS",
  "RGI_LIST_MONITORABLE_PROPERTIES",
  "RGI_LOAD_MONITOR_POINTSETS",
  "RGI_EVALUATE_MONITOR_VALUES",
  "RGI_LIST_AVAILABLE_FAILURE_MECHANISMS_RETURN",
  "RGI_EVALUATE_LIMIT_STATE_FUNCTIONS_RETURN",
  "RGI_CALCULATE_END",
};
}

namespace
{

const QString TENSOR_IS_NOT_COMPLETE =
  "Not all components of the residual strain tensor have been loaded, "
  "the tensor will be ignored.";

} // anonymous namespace


namespace
{
// The part of where we check values for D1 is stolen from CDumpModel
// We mainly check D1, and assume that settings in D0 are part of the set-up and allowed/required
bool ModelIsEmpty(CModelBase *model, std::vector<QString>& warnings)
{
  bool retval = true;

  CDepletionStage *stage = &model->LastDepletionStage();
  CDepletionStage *last_stage = stage;
  int dep_count = 0;
  while (stage)
  {
  ++dep_count;

  stage = stage->Initial() ? 0 : &stage->Previous();

  if (stage)
      last_stage = stage;
  }
  if (dep_count > 2)
  {
  warnings.push_back(QObject::tr("Model contains more than two depletion stages"));
  retval = false;
  }

  stage = last_stage;
  if (!stage->IsMarkedAsInitial())
  {
  warnings.push_back(QObject::tr("Initial stage is not marked as initial"));
  retval = false;
  }
  if (stage->OutputType() != CDepletionStage::RESULTS && stage->OutputType() != CDepletionStage::PHASE && stage->OutputType() != CDepletionStage::BRANCH)
  {
  warnings.push_back(QObject::tr("First depletion stage has not Results or Phase or Branch as output type"));
  retval = false;
  }
  stage = &stage->Next();

  if (stage->OutputType() != CDepletionStage::RESULTS && stage->OutputType() != CDepletionStage::BRANCH)
  {
  warnings.push_back(QObject::tr("Second depletion stage has not Results or Branch as output type"));
  retval = false;
  }

  TFormationBaseEntry& entry = (TFormationBaseEntry&)*model->GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stFormations = entry.EntryNodes();

  for (TFormationBaseEntry::TNodeSet::iterator it = stFormations.begin(); it != stFormations.end(); ++it)
  {
  CPressure *pressure = &(*it)->Pressure(*stage);
  if (pressure->DistributedSize() > 0 || pressure->Type() != CPressure::REPEAT_MODE)
  {
      warnings.push_back(QObject::tr("Second depletion stage of formation '%1' has pressure loads defined").arg((*it)->Name()));
      retval = false;
  }

  CTemperature *temperature = &(*it)->UserTemperature(*stage);
  if (temperature->DistributedSize() > 0 || temperature->Type() != CTemperature::REPEAT_MODE)
  {
      warnings.push_back(QObject::tr("Second depletion stage of formation '%1' has temperature loads defined").arg((*it)->Name()));
      retval = false;
  }

  CStrainLoad *strain = &(*it)->Strain(*stage);
  if (strain->DistributedSize() > 0 || strain->Type() != CStrainLoad::REPEAT_MODE)
  {
      warnings.push_back(QObject::tr("Second depletion stage of formation '%1' has strain loads defined").arg((*it)->Name()));
      retval = false;
  }
  }

  CTetraSubHorizonEntry* pEntryF = (CTetraSubHorizonEntry*)model->GraphEntry(MD_TETRA_SUB_FAULT);

  if (pEntryF)
  {
  CTetraSubHorizonEntry::TNodeSet stFaults = pEntryF->EntryNodes();
  for (CTetraSubHorizonEntry::TNodeSet::const_iterator it = stFaults.begin(); it != stFaults.end(); ++it)
  {
      const CFaultPressure& faultPressure = (*it)->Pressure(*stage);
      const CFaultPressure& prevPressure = (*it)->Pressure(stage->Previous());

      if (faultPressure.DistributedSize() > 0 || faultPressure.Mode() != prevPressure.Mode())
      {
    warnings.push_back(QObject::tr("Second depletion stage of fault '%1' has pressures defined").arg((*it)->Name()));
    retval = false;
      }
  }
  }

  const CBoundaryBase& boundary = model->Boundary();
  const CInterfaceBoundary* pBoundary = dynamic_cast<const CInterfaceBoundary*>(&boundary);

  const CDisplacementSupportNode& dsn = pBoundary->DisplacementSupportNode();
  CDisplacementSupportNode::TCornerNodeVec vcCorners = dsn.getCornerNodes();
  const geo::IPoint& ptRef = dsn.ReferencePoint();

  for (int i = 0; i < vcCorners.size(); ++i)
  {
  const geo::IPoint& pt = *vcCorners[i];
  const CDisplacementSupportNode::CNodalSupportDef* pNsd = dsn.NodalSupportDef(pt);

  geo::CVector vec = dsn.DisplacementFromDistribution(pt, *stage);
  if (!vec.Empty())
  {
      warnings.push_back(QObject::tr("Second depletion stage of Boundary Conditions has displacements defined"));
      retval = false;
  }
  }

  if (model->ResultRegister().ResultsAvailable())
  {
  warnings.push_back(QObject::tr("Model contains results"));
  retval = false;
  }

  return retval;
}

}


namespace gm_shell
{

/////////////////////////////////////////////////////////
//
//
//				RGIGlobalMessageObserver
//
//
/////////////////////////////////////////////////////////

CRGIGlobalMessageObserver::CRGIGlobalMessageObserver(GeomecRGI::CRockMechProcessor *RMP)
  : m_pRMP(RMP)
  , m_pRgi(nullptr)
{
  _m()->AttachObserver(this);
}

CRGIGlobalMessageObserver::~CRGIGlobalMessageObserver()
{
  _m()->DetachObserver(this);
}

void CRGIGlobalMessageObserver::setRGInterface(RGInterface *pRgi)
{
  m_pRgi = pRgi;
}

void CRGIGlobalMessageObserver::notifyWarning(const std::string& message)
{
  m_pRMP->AddLogLine(message.c_str(), m_pRgi, false, false);
  printer->info("log : rgi :  %s", message.c_str());
}

void CRGIGlobalMessageObserver::notifyError(const std::string& message)
{
  m_pRMP->AddLogLine(message.c_str(), m_pRgi, false, true);
  printer->info("log : rgi :  %s", message.c_str());
}

/////////////////////////////////////////////////////////
//
//
//				CInterfaceLogWrapperRGI
//
//
/////////////////////////////////////////////////////////

class CInterfaceLogWrapperRGI : public CInterfaceLogWrapper
{
  GeomecRGI::CRockMechProcessor *m_pRMP;
  RGInterface        *m_pRgi;
public:
  CInterfaceLogWrapperRGI(GeomecRGI::CRockMechProcessor *RMP, RGInterface *Rgi) : m_pRMP(RMP), m_pRgi(Rgi) {}
  virtual void AddLine(const QString& line, bool bVerbose = false, bool bFatal = false)
  {
  m_pRMP->AddLogLine(line, m_pRgi, bVerbose, bFatal);
  printer->info("log : rgi :  %s", line.toStdString().c_str());
  }
};


/////////////////////////////////////////////////////////
//
//
//				CChannelRGICORA
//
//
/////////////////////////////////////////////////////////

class CChannelRGICORA : public CChannelCORA
{
  std::vector<RGFailureMechanism> m_failureMechanisms;

  // conversion methods from CORA to RGI
  RGFailure::RGFailureMechamismObject oType(int type)
  {
  switch (type)
  {
  case 1: return RGFailure::Fault;
  case 2: return RGFailure::Formation;
  case 5: return RGFailure::TopSurface;
  case 6: return RGFailure::Well;
  default: return RGFailure::UndefinedObject;
  }
  }

  RGFailure::RGFailureMechanismFailureTypes fType(int type)
  {
  return static_cast<RGFailure::RGFailureMechanismFailureTypes>(type);
  }

  RGFailure::RGFailureMechanismLsfType lType(int type)
  {
  return static_cast<RGFailure::RGFailureMechanismLsfType>(type);
  }

public:
  CChannelRGICORA(IInterfaceBase *master)
  : CChannelCORA(master)
  {
  }

  virtual void AddFailureMechanism(std::string name, int objectType, int failureType, int lsfType, std::vector<double> values)
  {
  RGFailureMechanism fm;

  fm.objectName = name;
  fm.object = oType(objectType);
  fm.failureType = fType(failureType);
  fm.lsfType = lType(lsfType);
  fm.value = values;

  m_failureMechanisms.push_back(fm);
  }

  std::vector<RGFailureMechanism>& FailureMechanisms()
  {
  return m_failureMechanisms;
  }
};

/////////////////////////////////////////////////////////
//
//
//				CRGIFilosFatalStrategy
//
//
/////////////////////////////////////////////////////////

class CRGIFilosFatalStrategy : public IFilosFatalStrategy
{
public:
  CRGIFilosFatalStrategy(GeomecRGI::CRockMechProcessor& rockMechProcessor, RGInterface& rgInterface)
  : m_rockMechProcessor(rockMechProcessor)
  , m_rgInterface(rgInterface)
  {
  }

  virtual ~CRGIFilosFatalStrategy() {}

  virtual void Execute(const char *message)
  {
  m_rockMechProcessor.AddLogLine(QString("Fatal Filos error: %1").arg(message), &m_rgInterface, false, true);
  m_rockMechProcessor.AddLogLine("EMERGENCY EXIT", &m_rgInterface, false, true);
  }
  
private:
  GeomecRGI::CRockMechProcessor& m_rockMechProcessor;
  RGInterface& m_rgInterface;
};



/////////////////////////////////////////////////////////
//
//
//				CInterfaceRGI
//
//
/////////////////////////////////////////////////////////

CInterfaceRGI::CInterfaceRGI()
  : m_pModelContext(0)
  , m_pRockMechProcessor(0)
  , m_pSync(0)
  , m_pRgi(0)
  , m_pBuildTensor(0)
  , m_bSkipShutdown(false)
  , m_bSaveModel(false)
  , m_bQuit(false)
  , m_bHibernate(false)
  , m_bHibernating(false)
  , m_nCommands(0)
  , m_nCurrentCommand(-1)
  , m_bHaveCalculation(false)
{
}


const char *CInterfaceRGI::ShortDescription() const
{
  return "RockMechProcessor";
}


bool CInterfaceRGI::AcceptParameters(CArgumentParser& argParser, bool bForced)
{
  std::string key;
  std::string value;

  m_argParser = &argParser;

  if (argParser.ArgumentSize() == 1)
  {
  argParser.GetArgument(0, key, value);
  if (key == "version")
  {
      m_commands.push(IInterfaceBase::QUIT);
      return true;
  }
  }

  struct
  {
  const char *key;
  QString    *value;
  } params[] =
  {
  { "rockmechfile",  &parsedData.sRockMechFile },
  { "outputdir",     &parsedData.sOutputDir    },
  { "log",           &parsedData.sLog          },
  { "tempdir",       &parsedData.sTempDir      },
  { 0,               0                         }
  };

  bool bParseError = false;

  for (size_t i = 0; i < argParser.ArgumentSize(); ++i)
  {
  argParser.GetArgument(i, key, value);

  if (!key.empty())
  {
      size_t j;
      for (j = 0; params[j].key; ++j)
      {
    if (key == params[j].key)
    {
          if (!params[j].value->isEmpty())
      bParseError = true;

          *params[j].value = value.c_str();

          break;
    }
      }
      if (!params[j].key)
    bParseError = true;
  }
  else
  {
      if (!parsedData.sModelFile.isEmpty())
          bParseError = true;
      parsedData.sModelFile = value.c_str();
  }
  }

  if (bParseError && (bForced || !parsedData.sRockMechFile.isEmpty()))
  {
  m_error = RET_ARGS;
  m_commands.push(IInterfaceBase::USAGE);
  m_commands.push(IInterfaceBase::ABORT);
  return true;
  }

  if (!parsedData.sRockMechFile.isEmpty())
  return true;

  m_argParser = 0;

  return bForced;
}


void CInterfaceRGI::PrintUsage() const
{
  std::string commandPath;
  m_argParser->GetCommand(commandPath);

  const char *progName = ProgName(commandPath.c_str());

  std::cerr << "Usage: " << progName << " /ROCKMECHFILE:<rockmechfile> [<modelfile>] [/OUTPUTDIR:<dir>] [/LOG:<filename>] [/TEMPDIR:<dir>]" << std::endl
      << "       " << progName << " --version" << std::endl << std::endl;
}


void CInterfaceRGI::Startup()
{
  printer->info("rgi : start up");

  std::cout << "using interface RGI" << std::endl << std::endl;

  if (IsSlave())
  {
  m_pModelContext = m_channel->Master()->GetModelContext();
  }

  if (!m_initialized)
  {
  if (parsedData.sRockMechFile.isEmpty() || !m_commands.empty())
  {
      m_bSkipShutdown = true;
      return;
  }

  bool bLoadModel = !parsedData.sModelFile.isEmpty();
  if (!bLoadModel)
      parsedData.sModelFile = parsedData.sRockMechFile + ".gm5";

  gm_shell::MakeAbsolutePath(parsedData.sModelFile);
  gm_shell::MakeAbsolutePath(parsedData.sRockMechFile);
  gm_shell::MakeAbsolutePath(parsedData.sTempDir);

  if (!parsedData.sOutputDir.isEmpty())
      gm_shell::MakeAbsolutePath(parsedData.sOutputDir);

  if (!parsedData.sLog.isEmpty())
      gm_shell::MakeAbsolutePath(parsedData.sLog);

  m_pRockMechProcessor = new GeomecRGI::CRockMechProcessor(parsedData.sRockMechFile, parsedData.sOutputDir, parsedData.sTempDir);

  m_pGlobalMessageObserver = new CRGIGlobalMessageObserver(m_pRockMechProcessor);

  if (!parsedData.sLog.isEmpty() && !_m()->openlogfile(parsedData.sLog.toStdString()))
  {
      m_pRockMechProcessor->AddLogLine(QString("Unable to open log file '%1'").arg(parsedData.sLog), nullptr, false, true);

      m_error = RET_LOGFAIL;
  }


  m_pModelContext = new CInterfaceModelContext(APP_VERSION.toStdString().c_str());

  if (bLoadModel)
      m_pModelContext->Load(parsedData.sModelFile.toStdString().c_str());
  else
      m_pModelContext->Create(parsedData.sModelFile.toStdString().c_str());

  if (!m_pModelContext->Model())
  {
      m_pRockMechProcessor->AddLogLine(QString("Failed to load model '%1'").arg(parsedData.sModelFile), nullptr, false, true);

      m_error = RET_LOADFAIL;
  }
  else
  {
      // force optimization of filos files
      bool bForceCleanup = true;
      const char *option = getenv("RGI_NO_BRANCH_CLEANUP");
      if (option && (option[0] == 'y' || option[0] == 'Y'))
    bForceCleanup = false;

      m_pModelContext->Model()->CleanupOldBranchResults(bForceCleanup);
  }

  m_pModelContext->OutputPath(parsedData.sOutputDir.isEmpty() ? m_pModelContext->ModelPath() : parsedData.sOutputDir);

  CValueMapper::instance()->DianaDistributedMapping(false);

  const char *licenseRetry = getenv("RGI_LICENSE_RETRY");
  if (licenseRetry)
  {
      QString lr = licenseRetry;
      m_pModelContext->LicenseRetry(lr.toInt());
  }

  m_pRockMechProcessor->SetModel(*m_pModelContext->Model(), !m_pModelContext->Created());
  m_pRockMechProcessor->AddLogLine(QObject::tr("Starting RockMechProcessor with filename '%1'").arg(parsedData.sRockMechFile));
  m_pRockMechProcessor->AddLogLine(QObject::tr("Version: %1").arg(APP_VERSION));

  if (m_pModelContext->Model())
  {
      if (!m_pModelContext->Created())
      {
    if (!m_pModelContext->Validate(true))
    {
          m_pRockMechProcessor->AddLogLine(QObject::tr("Invalid Geomec model, not all calculation criteria are met!"), nullptr, false, true);
          m_error = RET_EXECFAIL;
    }
    m_pModelContext->CopyFiles();
      }

      std::vector<QString> warnings;
      if (!ModelIsEmpty(m_pModelContext->Model(), warnings))
      {
    m_pRockMechProcessor->AddLogLine(QObject::tr("Geomec model hasn't been cleaned up; please resolve the following issues:"), nullptr, false, true);
    for (std::vector<QString>::const_iterator it = warnings.begin(); it != warnings.end(); ++it)
    {
          m_pRockMechProcessor->AddLogLine(QString(">> %1").arg(*it), nullptr, false, true);
    }
      }
  }

  m_bQuit = false;
  m_nCommands = 0;
  m_nCurrentCommand = -1;
  m_bHaveCalculation = false;

  m_pSync = new RGSync(RGSync::Follower, parsedData.sRockMechFile.toStdString());

  m_commands.push(RGI_SETUP);
  //m_commands.push(RGI_LIST_MONITORABLE_PROPERTIES);

  m_initialized = true;
  }
}


void CInterfaceRGI::Shutdown()
{
  printer->info("rgi : shut down");

  if (!IsMaster())
  {
  if (m_bSkipShutdown)
      return;

  delete m_pGlobalMessageObserver;
  delete m_pRockMechProcessor;
  delete m_pModelContext;
  }
}


void CInterfaceRGI::LogCommand(GMCommand& cmd, int command)
{
  QString msg;

  switch (command)
  {
  case RGI_SETUP:
  msg = "Setting up RGInterface";
  break;
  case RGI_SHUTDOWN:
  msg = "Shutting down RGInterface";
  break;
  default:
  msg = QString("Received GMCommand[%1]: %2 (%3)").arg(m_nCurrentCommand).arg(cmd.toStringCmd().c_str()).arg(cmd.toStringPrms().c_str());
  if (m_error != 0 && cmd.getType() != typeCommandQuit && cmd.getType() != typeCommandHibernate)
      msg += QString("  --  command will be ignored due to error code set (%1)").arg(m_error);
  }

  m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, false);
}


void CInterfaceRGI::HandleLastCommand()
{
  int command = m_commands.front();
  m_commands.pop();

  bool retval = true;
  
  bool bOldSaveModel = m_bSaveModel;

  try
  {
  GMCommand gmc;
  if (m_nCurrentCommand >= 0 && m_nCurrentCommand < m_nCommands)
      gmc = m_pRgi->getCommandN(m_nCurrentCommand);

  LogCommand(gmc, command);

  // JH: This is basically a duplicate of the above LogCommand
    printer->info(">> rgi : cmd:%s", rgi_cmd_cp[command-IInterfaceBase::FOR_ME-1]); // be carefull with this !!!!

  switch(command)
  {
  case RGI_SETUP:
      RGISetup(); 
      break;
  case RGI_SHUTDOWN:
      RGIShutdown();
      break;
  case RGI_LOAD_GRID:
      if (!m_error)
      {
    retval = m_pRockMechProcessor->LoadGrid(*m_pRgi);
    m_bSaveModel = true;
      }
      break;
  case RGI_SAVE_GRID:
      if (!m_error)
    retval = m_pRockMechProcessor->SaveGrid(*m_pRgi);
      break;
  case RGI_LOAD_PROPERTY:
      if (!m_error)
      {
    retval = m_pRockMechProcessor->LoadProperty(*m_pRgi, gmc.getProperty());
    m_bSaveModel = true;
      }
      break;
  case RGI_SAVE_PROPERTY:
      if (!m_error)
    retval = m_pRockMechProcessor->SaveProperty(*m_pRgi, gmc.getProperty());
      break;
  case RGI_CALCULATE:
      if (!m_error)
      {
    if (!m_pBuildTensor->isTensorComplete())
    {
          QString message = TENSOR_IS_NOT_COMPLETE;
          m_pRockMechProcessor->AddLogLine(message, m_pRgi, false, false);
    }
    if (!m_pModelContext->HasRun())
    {
          if (m_pRgi->getCurrentDepletionStage().getDepletionStage() == 0)
          {
      m_pRockMechProcessor->AddLogLine("Cannot calculate the initial stage", m_pRgi, false, true);
      retval = false;
      break;
          }

          if (!m_pRockMechProcessor->EnsureDepletionStageAvailable(*m_pRgi)) // add it if no properties were loaded
          {
      retval = false;
      break;
          }
    }
    {
          const char *save = getenv("RGI_SAVE_BRANCHES");

          bool do_step = !((save && (save[0] == 'y' || save[0] == 'Y')) || !m_bHibernating);

          CInterfaceLogWrapperRGI logWrapper(m_pRockMechProcessor, m_pRgi);
          FilosFatalHandler::instance()->Strategy(new CRGIFilosFatalStrategy(*m_pRockMechProcessor, *m_pRgi));

          retval = do_step ? m_pModelContext->RunStep(&logWrapper) : m_pModelContext->Run(&logWrapper);

          if (!m_pModelContext->LicenseError().isEmpty())
      m_pRockMechProcessor->AddLogLine(m_pModelContext->LicenseError(), m_pRgi, false, true);
          else if (!m_pModelContext->GetCalculationResult())
          {
      QString msg = "Analysis failed";
      if (m_pModelContext->HaveResults())
              msg += ", partial results are available";
      m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, true);
          }
    }
    m_bSaveModel = true;
      }
      break;
  case RGI_CALCULATE_END:
      m_pModelContext->End();
      break;
  case RGI_DEFINE_INITIAL_DATE:
      if (!m_error)
      {
    retval = m_pRockMechProcessor->DefineInitialDate(*m_pRgi, gmc.getDatePrm());
    m_bSaveModel = true;
      }
      break;
  case RGI_QUIT:
      m_bQuit = true;
      
      if (m_bHibernating && m_bHaveCalculation)
      {
    m_pRockMechProcessor->AddLogLine("Cleaning up calculation files");
    m_pModelContext->CleanUpAfterRun();
      }
      else
      {
    if (m_bHibernating)
          m_pRockMechProcessor->AddLogLine("Not cleaning up calculation files, because there was no calculation");
    else
          m_pRockMechProcessor->AddLogLine("Not cleaning up calculation files, because not in hibernation mode");
      }
      break;
  case RGI_HIBERNATE:
      m_bHibernate = true;
      break;
  case RGI_LIST_AVAILABLE_FAILURE_MECHANISMS:
      if (!m_error)
      {
    m_commands.push(SWITCH_INTERFACE);
    m_commands.push(RGI_LIST_AVAILABLE_FAILURE_MECHANISMS_RETURN);

    m_channel = new CChannelRGICORA(this);
    m_channel->Commands().push_back(CInterfaceCORA::CORA_GET_FAILURE_MODES);
      }
      break;
  case RGI_LIST_AVAILABLE_FAILURE_MECHANISMS_RETURN:
      if (!m_error)
      {
    assert(IsMaster());

    m_pRgi->setAvailableFailureMechanismsInModel(static_cast<CChannelRGICORA *>(m_channel)->FailureMechanisms());
    
    delete m_channel;
    m_channel = 0;
      }
      break;
  case RGI_EVALUATE_LIMIT_STATE_FUNCTIONS:
      if (!m_error)
      {
    m_commands.push(SWITCH_INTERFACE);
    m_commands.push(RGI_EVALUATE_LIMIT_STATE_FUNCTIONS_RETURN);

    m_channel = new CChannelRGICORA(this);
    m_channel->Commands().push_back(CInterfaceCORA::CORA_RUNMODEL);
      }
      break;
  case RGI_EVALUATE_LIMIT_STATE_FUNCTIONS_RETURN:
      if (!m_error)
      {
    assert(IsMaster());
    delete m_channel;
    m_channel = 0;

    m_pRockMechProcessor->AddLogLine("Evaluating limit state functions not implemented yet", m_pRgi, false, true);
      }
      break;
  case RGI_LIST_MONITORABLE_PROPERTIES:
      if (!m_error)
      {
    m_pRockMechProcessor->ListMonitorableProperties(*m_pRgi);
      }
      else // for testing (uncomment commands.push_back in Startup()
      {
    std::map<std::string, const IValueComponentBase *> properties;
    m_pRockMechProcessor->CollectMonitorablePropertiesFromModel(properties, false);

    retval = false;
      }
      break;
  case RGI_LOAD_MONITOR_POINTSETS:
      if (!m_error)
      {
    retval = m_pRockMechProcessor->LoadMonitorPointSets(*m_pRgi);
      }
      break;
  case RGI_EVALUATE_MONITOR_VALUES:
      if (!m_error)
      {
    if (!m_pModelContext->HasRun())
    {
          m_pRockMechProcessor->AddLogLine("No monitor values: calculation hasn't run", m_pRgi, false, true);
          retval = false;
    }
    else if (!m_pModelContext->HaveResults())
    {
          m_pRockMechProcessor->AddLogLine("No monitor values: calculation has no results", m_pRgi, false, true);
          retval = false;
    }
    else
    {
          retval = m_pRockMechProcessor->EvaluateMonitorValues(*m_pRgi);
    }
      }
      break;
  }
  }

  catch(RGInterface::Exception& e)
  {
  QString l;
  l = QString("Caught interface error '%1'").arg(e.what());
  m_pRockMechProcessor->AddLogLine(l);
  retval = false;
  m_bSaveModel = false;
  }

  catch(std::runtime_error e)
  {
  QString l;
  l = QString("Caught interface error '%1'").arg(e.what());
  m_pRockMechProcessor->AddLogLine(l);
  retval = false;
  m_bSaveModel = false;
  }

  if (!retval)
  m_error = RET_EXECFAIL;

  if (m_bSaveModel != bOldSaveModel)
  {
  QString msg = QString("Flag to save model has changed from '%1' to '%2'").arg(bOldSaveModel ? "true" : "false").arg(m_bSaveModel ? "true" : "false");
  m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, false);
  }

  if (m_commands.empty())
  {
  ++m_nCurrentCommand;
  if (m_bQuit || m_bHibernate || m_nCurrentCommand > m_nCommands)
  {
      if (m_bQuit && m_nCurrentCommand <= m_nCommands)
    m_commands.push(RGI_SHUTDOWN);
      if (!m_bQuit)
      {
    if (m_bHibernate)
          m_commands.push(RGI_SHUTDOWN);
    else
          m_bQuit = !m_bHibernating;
      }
  }
  else
  {
      GMCommand gmc = m_pRgi->getCommandN(m_nCurrentCommand);
      m_commands.push(gmc.getType() + RGI_LOAD_GRID);
  }
  }

}


CInterfaceModelContext *CInterfaceRGI::GetModelContext()
{
  return m_pModelContext;
}


void CInterfaceRGI::RGISetup()
{
  if (!m_bQuit)
  {
  m_bHibernate = false;

  m_pSync->lock();

  m_pRgi = new RGInterface(parsedData.sRockMechFile.toStdString());
  m_pGlobalMessageObserver->setRGInterface(m_pRgi);
  m_pBuildTensor = new GeomecRGI::CBuildTensor;

  m_nCurrentCommand = -1;
  m_error = 0;

  /*
      A loaded property is immediately destroyed after use (after
      execution of 'LoadProperty'). The loading of the 6 properties
      'propElementResidualStrainLoading_*' will require the persistence of
      the created object, hence the initial declaration below.
      As a side-effect the created object can be checked for completeness.
      When more tensors have to be supported the current implementation
      has to be revisited.
  */

  if (m_pModelContext->Model() && !m_pModelContext->Created())
  {
      m_pRockMechProcessor->CheckModel(*m_pRgi);
  }

  if (m_pRockMechProcessor->HandleFatalErrors(m_pRgi))
  {
      m_error = RET_EXECFAIL;
      m_commands.push(RGI_SHUTDOWN);
      m_commands.push(IInterfaceBase::ABORT);
  }

  m_nCommands = m_pRgi->getNumCommands();

  if (m_nCommands == 0)
  {
      QString l = QString("The RGI project '%1' does not contain any commands ('%2' commands)").arg(parsedData.sRockMechFile).arg(m_nCommands);
      m_pRockMechProcessor->AddLogLine(l, m_pRgi, false, true);
      m_bQuit = true;
  }

  bool commandHibernateOrQuitEncountered = false;

  // check for quit command so we now what to do when an exception is caught
  for(int i = 0; i < m_nCommands; ++i)
  {
      GMCommand gmc(m_pRgi->getCommandN(i));

      if(gmc.getType() == typeCommandHibernate)
      {
    commandHibernateOrQuitEncountered = true;
    m_bHibernating = true;
    break;
      }

      if(gmc.getType() == typeCommandQuit)
      {
    commandHibernateOrQuitEncountered = true;
    break;
      }
  }

  if (!commandHibernateOrQuitEncountered)
  {
      QString l = QString("The RGI project '%1' does not contain either the 'typeCommandHibernate' or the 'typeCommandQuit' command, the 'typeCommandQuit' command is used as the default last command").arg(parsedData.sRockMechFile);
      m_pRockMechProcessor->AddLogLine(l, m_pRgi, false, true);
  }

  if (m_pModelContext && m_pModelContext->Model() && !m_pModelContext->Created())
  {
      GeomecRGI::CGeomecFormationIDs2RGI::SetFormations(*m_pRgi, *m_pModelContext->Model());
  }
  }
}

void CInterfaceRGI::RGIShutdown()
{
  if (m_pRgi->getDepletionStagesNumber() > 0)
  m_pModelContext->Stage(m_pRgi->getCurrentDepletionStage().getDepletionStage());

  m_pRgi->dumpModel();

  // in hibernation mode we need to do saving on quit here, as it seems that dynamo kills our thread after the lock is released
  if (m_bQuit)
  {
  if (m_bSaveModel)
  {
      bool retVal = m_error == 0;
      QString fileName = m_pRockMechProcessor->getSaveModelName(parsedData.sModelFile, m_pModelContext->Stage(), retVal, !m_pModelContext->Created());

      if (retVal || m_pModelContext->Stage() == 0)
      {
    QString msg = QString("Quitting (save model flag is 'true'), and saving '%1'").arg(fileName);
    m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, false);
    m_pModelContext->SaveAs(fileName);
    
    m_pModelContext->MoveFiles();
      }
      else
      {
    QString msg = QString("Quitting(save model flag is 'true'), but NOT saving '%1' -- error received (%2)").arg(fileName).arg(m_error);
    m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, false);
      }

      m_bSaveModel = false;
  }
  else
  {
      QString msg = QString("Quitting (save model flag is 'false')");
      m_pRockMechProcessor->AddLogLine(msg, m_pRgi, false, false);
  }

  m_pRockMechProcessor->AddLogLine("Closing log");
  _m()->closelogfile();
  }

  m_pGlobalMessageObserver->setRGInterface(nullptr);

  delete m_pRgi;
  delete m_pBuildTensor;

  m_pSync->release();
      
  if (m_bHibernate)
  {
  m_pSync->waitFor(-1);

  m_commands.push(RGI_SETUP);
  }
}




}





