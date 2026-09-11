#include <cstdlib>
#include <iostream>

#include <QString>

#include "geomec_shell.h"
#include "ArgumentParser.h"
#include "InterfaceCORA.h"

#include "ModelData.h"
#include "GetModelInfo.h"
#include "XMLModelInfo.h"
#include "RunModel.h"
#include "ValidateModel.h"
#include "GetModelInfoData.h"
#include "RunModelData.h"
#include "VersionNumber.h"

#include "InterfaceContext.h"

#include "Printer.h"


namespace
{
Printer* printer = Printer::instance(Printer::Shell);

const QString GETMODELINFO = "getmodelinfo";
const QString RUNMODEL = "runmodel";


const QString INVALID_MODEL =
  "invalid Geomec model, not all calculation criteria are met";

bool validateModel(cora::CSummaryResultFile& summaryResultFile,
  CModelData& modelData)
{
  if (modelData() == 0)
  {
  summaryResultFile.setResultValue(
      cora::CSummaryResultFile::RESULT_VALUE_INCONSISTENT);

  for (int l = 0; l < modelData.getAnalysisLogger().LineSize(); ++l)
  {
      summaryResultFile.addAdditionalInformation(
    modelData.getAnalysisLogger().Line(l).c_str());
  }

  return false;
  }

  CValidateModel validateModel(modelData());

  if (!validateModel.checkModel(false))
  {
  summaryResultFile.setResultValue(
      cora::CSummaryResultFile::RESULT_VALUE_INCONSISTENT);
  summaryResultFile.addAdditionalInformation(INVALID_MODEL);

  return false;
  }

  return true;
}

int getModelInfo(int argc, char* argv[])
{
  cora::CGetModelInfoData getModelInfoData(argc, argv);

  if (!validateModel(getModelInfoData.summaryResultFile(),
  getModelInfoData.modelData()))
  {
  return 1;
  }

  if (getModelInfoData.versionNumber() >= cora::MINIMUM_XML_VERSION_NUMBER)
  {
  cora::CXMLModelInfo getXMLModelInfo(getModelInfoData.modelData(),
      getModelInfoData.modelInfoFileName());

  return (getXMLModelInfo() ? 0 : 1);
  }

  cora::CGetModelInfo getModelInfo(getModelInfoData.modelData(),
  getModelInfoData.modelInfoFileName());

  return (getModelInfo() ? 0 : 1);
}

int runModel(int argc, char* argv[])
{
  cora::CRunModelData runModelData(argc, argv);

  if (!validateModel(runModelData.summaryResultFile(),
  runModelData.modelData()))
  {
  return 1;
  }

  cora::CRunModel runModel(runModelData, gm_shell::APPLICATION + " " + gm_shell::VERSION);
  bool runModelSucceeded = runModel();

  if (!runModelSucceeded)
  {
  runModelData.summaryResultFile().setResultValue(
      cora::CSummaryResultFile::RESULT_VALUE_DIVERGENCE);
  }

  return (runModelSucceeded ? 0 : 1);
}

} // anonymous namespace







namespace gm_shell
{


CChannelCORA::CChannelCORA(IInterfaceBase *master)
  : IChannel(master, "cora")
{
}



const char *CInterfaceCORA::ShortDescription() const
{
  return "CORA";
}

bool CInterfaceCORA::AcceptParameters(CArgumentParser& argParser, bool bForced)
{
  std::string key;
  std::string value;

  m_argParser = &argParser;

  for (size_t i = 0; i < argParser.ArgumentSize(); ++i)
  {
  argParser.GetArgument(i, key, value);
  if (value == "getmodelinfo")
  {
      m_commands.push(CORA_GETMODELINFO);
      return true;
  }
  else if (value == "runmodel")
  {
      m_commands.push(CORA_RUNMODEL);
      return true;
  }
  }

  if (bForced)
  {
  m_error = 1;
  m_commands.push(IInterfaceBase::USAGE);
  m_commands.push(IInterfaceBase::ABORT);
  return true;
  }

  m_argParser = 0;

  return false;
}


void CInterfaceCORA::PrintUsage() const
{
  std::cerr << std::endl
      << "geomec_shell <modeldata> " << GETMODELINFO.toStdString() << " [version number] <modelinfofile>" << std::endl
      << "\t\t<summary_result_file>" << std::endl
      << "\t<modeldata> is the filename of the Geomec model (gm4)" << std::endl
      << "\t[version number] is the optional version number" << std::endl
      << "\t\tthe model info will be text output when the version number" << std::endl
      << "\t\tis absent or less than \"2.0\"" << std::endl
      << "\t\tthe model info will be XML output when the version number" << std::endl
      << "\t\tis equal to or greater than \"2.0\"" << std::endl
      << "\t<modelinfofile> the filename where the model info will be printed to" << std::endl
      << "\t<summary_result_file> the filename for Geomec output especially for" << std::endl
      << "\t\tCORA" << std::endl << std::endl
      << "geomec_shell <modeldata> " << RUNMODEL.toStdString() << " <version number> <parameterfile>" << std::endl
      << "\t\t<selectedlsfs> <responseparameterfile> <summary_result_file>" << std::endl
      << "\t\t[" << cora::CRunModelData::MINIMUM_OUTPUT.toStdString() << "]" << std::endl
      << "\t<modeldata> is the filename of the Geomec model (gm4)" << std::endl
      << "\t<version number> is the version number of the interface" << std::endl
      << "\t<parameterfile> is the filename of the file with the altered parameters" << std::endl
      << "\t<selectedlsfs> is a filename with the actual limit state functions" << std::endl
      << "\t<responseparameterfile> the filename with the resulting" << std::endl
      << "\t\tresponseparameters" << std::endl
      << "\t<summary_result_file> the filename for Geomec output especially for" << std::endl
      << "\t\tCORA" << std::endl
      << "\t[" << cora::CRunModelData::MINIMUM_OUTPUT.toStdString() << "] is an optional flag to indicate that the user is only " << std::endl
      << "\t\tinterested in the response parameters and therefore geomec " << std::endl
      << "\t\tdoes not have to write output to the gm4 file" << std::endl << std::endl;
}

void CInterfaceCORA::Startup()
{
  printer->info("using interface CORA");

  std::cout << "using interface CORA" << std::endl << std::endl;

  m_initialized = true;
}

void CInterfaceCORA::HandleLastCommand()
{
  int command = m_commands.front();
  m_commands.pop();

  int retval = 1;

  switch(command)
  {
  case CORA_GETMODELINFO:
  if (m_argParser->ArgumentSize() >= 4 && m_argParser->ArgumentSize() <= 5) 
      retval = getModelInfo((int)m_argParser->ArgumentSize() + 1, m_argParser->_argv());
  break;
  case CORA_RUNMODEL:
  if (m_argParser->ArgumentSize() >= 7)
      retval = runModel((int)m_argParser->ArgumentSize() + 1, m_argParser->_argv());
  break;
  case CORA_GET_FAILURE_MODES:
  retval = GetFailureModes();
  break;
  case CORA_EVALUATE_LSF:
  retval = 0; // not implemented yet
  break;
  }

  if (retval != 0)
  {
  m_error = retval;
  m_commands.push(IInterfaceBase::USAGE);
  m_commands.push(IInterfaceBase::ABORT);
  }

}



void handleObjects(CChannelCORA& channel, const cora::TObjects& objects, cora::TFailureModes& failureModes)
{
  std::vector<double> values;

  for (cora::TObjects::const_iterator object = objects.begin(); object != objects.end(); ++object)
  {
  for (cora::TFailureModes::iterator failureMode = failureModes.begin(); failureMode != failureModes.end(); ++failureMode)
  {
      if ((*failureMode)->getObject() == *object)
      {
    for (int i = 0; i < 7; ++i)
          channel.AddFailureMechanism((*object)->name().toStdString(), (*object)->getObjectType(), (*failureMode)->getFailureModeType(), i, values);
      }
  }
  //const cora::TParameters parameters = (*object)->getParameters();
  //for (cora::TParameters::const_iterator parameter = parameters.begin(); parameter != parameters.end(); ++parameter)
  //{
  //  channel.AddFailureMechanism()
  //}
  }
}



int CInterfaceCORA::GetFailureModes()
{
  if (IsSlave())
  {
  CModelBase *model = m_channel->Master()->GetModelContext()->Model();

  CChannelCORA *channel = dynamic_cast<CChannelCORA *>(m_channel);

  if (!model || !channel)
      return 0;

  cora::CGetFaultInfo& faultInfo(cora::CGetFaultInfo::instance(model));
  cora::CGetFormationInfo& formationInfo(cora::CGetFormationInfo::instance(model));
  cora::CGetHorizonInfo& horizonInfo(cora::CGetHorizonInfo::instance(model));
  cora::CGetWellPathInfo& wellPathInfo(cora::CGetWellPathInfo::instance(model));
  //cora::CGetSurfaceInfo& surfaceInfo(cora::CGetSurfaceInfo::instance(model));
  //cora::CGetNonMeshedSurfaceInfo& getNonMeshedSurfaceInfo(cora::CGetNonMeshedSurfaceInfo::instance(model));

  cora::CGetFailureModeInfo getFailureModeInfo(model);
  cora::TFailureModes failureModes = getFailureModeInfo.getFailureModes();

  handleObjects(*channel, faultInfo.getObjects(), failureModes);
  handleObjects(*channel, formationInfo.getObjects(), failureModes);
  handleObjects(*channel, horizonInfo.getObjects(), failureModes);
  handleObjects(*channel, wellPathInfo.getObjects(), failureModes);
  }
  return 0;
}



}