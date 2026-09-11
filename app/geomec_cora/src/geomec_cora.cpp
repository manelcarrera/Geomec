#include <cstdlib>
#include <iostream>
#include <sstream>  // this header-file solved the errors below!

/*
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(93): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(94): error C2059: syntax error : '{'
22>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(375) : see reference to class template instantiation 'std::basic_stringbuf<_Elem,_Traits,_Alloc>' being compiled
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(94): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(113): error C2059: syntax error : 'string'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(114): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(457): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(459): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(458): error C2059: syntax error : '{'
22>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(469) : see reference to class template instantiation 'std::basic_istringstream<_Elem,_Traits,_Alloc>' being compiled
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(458): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(462): error C2059: syntax error : 'string'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(463): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(551): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(553): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(552): error C2059: syntax error : '{'
22>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(563) : see reference to class template instantiation 'std::basic_ostringstream<_Elem,_Traits,_Alloc>' being compiled
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(552): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(556): error C2059: syntax error : 'string'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(557): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(651): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(653): warning C4003: not enough actual parameters for macro 'str'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(652): error C2059: syntax error : '{'
22>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(663) : see reference to class template instantiation 'std::basic_stringstream<_Elem,_Traits,_Alloc>' being compiled
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(652): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(656): error C2059: syntax error : 'string'
22>C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\include\sstream(657): error C2334: unexpected token(s) preceding '{'; skipping apparent function body
 */

#include <QString>

#include "VersionNumbers.h"
#include "ModelData.h"
#include "XMLModelInfo.h"
#include "RunModel.h"
#include "geomec_cora.h"
#include "DianaStartUp.h"
#include "ValidateModel.h"
#include "GetModelInfoData.h"
#include "RunModelData.h"
#include "ApplicationInitialization.h"
#include "VersionNumber.h"

#include "GlobalMessage_CLI.h"

#include "Printer.h"
#include "SettingsIni.h"
#include "QUtil.h"

#include "Global.h"
#include "ProgressFactory_CLI.h"


#pragma message("WARNING: geomec_cora is deprecated; see WI 116602")
/*
Functionality of geomec_cora is now handled by InterfaceCORA of geomec_shell.

There is a grace period so that both geomec_shell and geomec_cora can be tested (regression).
Changes to the geomec_cora project should be avoided, but if that is not possible, they should be
implemented here and in geomec_shell.
*/

namespace
{

void header_DEPRECATED()
{
  std::cout << APPLICATION.toStdString() << " " << VERSION.toStdString() <<
  std::endl << std::endl;
  std::cout << "supports files up to version " << VERSION_MAJOR << "." <<
  VERSION_MINOR << "." << VERSION_REVISION << std::endl << std::endl;
}

const QString GETMODELINFO = "getmodelinfo";
const QString RUNMODEL = "runmodel";

void usage_DEPRECATED(int /*argc*/, char* /*argv*/[])
{
  std::cerr << std::endl << "geomec_cora.exe <modeldata> " <<
  GETMODELINFO.toStdString() << " [version number] <modelinfofile>" <<
  std::endl << "\t\t" << "<summary_result_file>" << std::endl;
  std::cerr << "\t" << "<modeldata> is the filename of the Geomec model " <<
  "(gm4)" << std::endl;
  std::cerr << "\t" << "[version number] is the optional version number" <<
  std::endl << "\t\t" << "the model info will be text output when the "
  "version number" << std::endl << "\t\t" << "is absent or less than "
  "\"2.0\"" << std::endl << "\t\t" << "the model info will be XML output "
  "when the version number" << std::endl << "\t\t" << "is equal to or "
  "greater than \"2.0\"" << std::endl;
  std::cerr << "\t" << "<modelinfofile> the filename where the model info " <<
  "will be printed to" << std::endl;
  std::cerr << "\t" << "<summary_result_file> the filename for Geomec " <<
  "output especially for" << std::endl << "\t\t" << "CORA" << std::endl <<
  std::endl;
  std::cerr << "geomec_cora.exe <modeldata> " << RUNMODEL.toStdString() <<
  " <version number> <parameterfile>" << std::endl << "\t\t" <<
  "<selectedlsfs> <responseparameterfile> <summary_result_file>" <<
  std::endl << "\t\t" << "[" <<
  cora::CRunModelData::MINIMUM_OUTPUT.toStdString() << "]" << std::endl;
  std::cerr << "\t" << "<modeldata> is the filename of the Geomec model " <<
  "(gm4)" << std::endl;
  std::cerr << "\t" << "<version number> is the version number of the " <<
  "interface" << std::endl;
  std::cerr << "\t" << "<parameterfile> is the filename of the file with " <<
  "the altered parameters" << std::endl;
  std::cerr << "\t" << "<selectedlsfs> is a filename with the actual limit " <<
  "state functions" << std::endl;
  std::cerr << "\t" << "<responseparameterfile> the filename with the " <<
  "resulting" << std::endl << "\t\t" << "responseparameters" << std::endl;
  std::cerr << "\t" << "<summary_result_file> the filename for Geomec " <<
  "output especially for" << std::endl << "\t\t" << "CORA" << std::endl;
  std::cerr << "\t" << "[" <<
  cora::CRunModelData::MINIMUM_OUTPUT.toStdString() <<
  "] is an optional flag to indicate that the user is only " << std::endl <<
  "\t\t" << "interested in the response parameters and therefore geomec " <<
  std::endl << "\t\t" << "does not have to write output to the gm4 file" <<
  std::endl << std::endl;
}

const QString INVALID_MODEL =
  "invalid Geomec model, not all calculation criteria are met";

bool validateModel_DEPRECATED(cora::CSummaryResultFile& summaryResultFile,
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

int getModelInfo_DEPRECATED(int argc, char* argv[])
{
  cora::CGetModelInfoData getModelInfoData(argc, argv);

  if (!validateModel_DEPRECATED(getModelInfoData.summaryResultFile(),
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

int runModel_DEPRECATED(int argc, char* argv[])
{
  cora::CRunModelData runModelData(argc, argv);

  if (!validateModel_DEPRECATED(runModelData.summaryResultFile(),
  runModelData.modelData()))
  {
  return 1;
  }

  cora::CRunModel runModel(runModelData, APPLICATION + " " + VERSION);
  bool runModelSucceeded = runModel();

  if (!runModelSucceeded)
  {
  runModelData.summaryResultFile().setResultValue(
      cora::CSummaryResultFile::RESULT_VALUE_DIVERGENCE);
  }

  return (runModelSucceeded ? 0 : 1);
}

} // anonymous namespace

int main_DEPRECATED(int argc, char* argv[])
{
  GlobalMessage::init( new GlobalMessage_CLI() );

  CApplicationInitialization consoleApplicationInitialization;

  Printer::instance()->url( QUtil::url( 
    QUtil::eUsrDir::Goemec, 
    "gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log" ) );
  Printer::instance()->info( "" );
  Printer::instance()->info( "gm_cora > main" );

  // Config file
  CSettingsIni::instance()->init();
  CSettingsIni::instance()->print_();

  _g->prog( new CProgressFactory_CLI );

  CDianaStartUp* dsu = CDianaStartUp::instance();
  	dsu->SetDianaEnv(); 
  dsu->Print_DianaEnv();

  header_DEPRECATED();

  int result = 1;

  if ((argc >= 5) && (argc <= 6) && (argv[2] == GETMODELINFO))
  {
  result = getModelInfo_DEPRECATED(argc, argv);
  }
  else if ((argc >= 8) && (argv[2] == RUNMODEL))
  {
  result = runModel_DEPRECATED(argc, argv);
  }

  if (result != 0)
  {
  usage_DEPRECATED(argc, argv);
  }

  return result;
}

int main(int argc, char* argv[])
{
  return main_DEPRECATED(argc, argv);
}