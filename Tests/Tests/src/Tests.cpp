#include <iostream>
#include <exception>
#include <gtest/gtest.h>
#include "Printers.h"
#include "TestLib.h"
#include "SanityChecks.h"
#include "Fw51.h"
#include "tstMaterials.h"
#include "ModelOperations.h"
#include "ApplicationInitialization.h"
#include "GlobalMessage_CLI.h"
#include "GlobalMessage.h"
#include "DianaStartUp.h"
#include "SettingsIni.h"
#include "QUtil.h"
#include "Printer.h"
#include "ISettings.h"
#include "GeomecUtils.h" //GetAppPath

#include "Global.h"
#include "ProgressFactory_CLI.h"

//qt
#include <QCoreApplication> //pid

namespace
{
	Printer* printer = Printer::instance(Printer::Tests);
}


// ensure the linker is not dismissing these libraries
int loaded = 0
  || test_sanity_checks::LibraryLoad_SanityChecks()
  || test_fw51::LibraryLoad_Fw51()
  || test_materials::LibraryLoad_Materials()
  || test_model_operations::LibraryLoad_ModelOperations()
  ;


#define PARAM_GM_MODEL_PATH	"--gm-model-path="
#define PARAM_INTERACTIVE "--interactive"

// https://github.com/google/googletest/blob/master/googletest/docs/advanced.md#getting-the-current-tests-name
class MinimalistPrinter : public ::testing::EmptyTestEventListener 
{
    // Called before a test starts.
    virtual void OnTestStart(const ::testing::TestInfo& test_info) 
	{
   	  printer->info( "" );
	  printer->info( "--%s.%s-- [TEST]", test_info.test_case_name(), test_info.name() );
	  printer->info( "" );
    }

    // Called after a failed assertion or a SUCCESS().
    /*virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) 
	{
      printf("%s in %s:%d\n%s\n",
             test_part_result.failed() ? "*** Failure" : "Success",
             test_part_result.file_name(),
             test_part_result.line_number(),
             test_part_result.summary());
    }*/

    // Called after a test ends.
    /*virtual void OnTestEnd(const ::testing::TestInfo& test_info) 
	{
	  printer->info( "" );
    }*/
  };

GTEST_API_ int main(int argc, char **argv) {
  test_lib::TestLib::setExecPath(argv[0]);

  testing::InitGoogleTest(&argc, argv);

    bool interactive = false;

  for (int i = 1; i < argc; ++i)
  {
	 size_t j = strlen(PARAM_GM_MODEL_PATH);
	 if (strncmp(argv[i], PARAM_GM_MODEL_PATH, j) == 0)
	 {
     if (!test_lib::TestLib::setModelPath(&argv[i][j]))
     {
       std::cerr << "Path to models is too long" << std::endl;
       exit(-1);
     }
     continue;
	 }

   j = strlen(PARAM_INTERACTIVE);
	 if (strncmp(argv[i], PARAM_INTERACTIVE, j) == 0)
	 {
     interactive = true;
   }
  }

  UnitTest& unit_test = *UnitTest::GetInstance();
  TestEventListeners& listeners = unit_test.listeners();
  listeners.Append(new RTXPrinter("test_output.trx"));

  //
  //
  //
  listeners.Append(new MinimalistPrinter);

  //listeners.Append(new testing::internal::XmlUnitTestResultPrinter("test_results.xml"));

  GlobalMessage::init(new GlobalMessage_CLI());

  CApplicationInitialization consoleApplicationInitialization(test_lib::TestLib::getDefaultAppVersion());

  Printer::pid( QCoreApplication::applicationPid() );
	//
	// Set logs file
	// one file per version
	//
	std::string ver = CSettingsIni::instance()->version_str().toStdString();
	std::string url = QUtil::url(QUtil::eUsrDir::Goemec, "gm_" + ver + ".log");
	for (int i = 0; i < Printer::ModulesNum; i++)
		Printer::instance((Printer::eModule)i)->url(url);
  	//
	//
	//
	printer->info( "" );
	printer->info( "tests >> >>" );
	printer->info( "" );
	printer->info("path: %s", GetAppPath().toStdString().c_str());
	printer->info("");
	//
	//
	//
	printer->info( "--legend--" );
	for (int i = 0; i < Printer::ModulesNum; i++)
	{
		Printer* p = Printer::instance((Printer::eModule)i);
		std::pair<std::string, std::string>& t = p->m_cfg.modules_s_m.at((Printer::eModule)i);
		p->info(t.second.c_str());
	}
	printer->info("");
	//
 	// Config file
	//
	CSettingsIni::instance()->init();
	CSettingsIni::instance()->print_();

  _g->dsa(ISettings::instance()->getProfileString("Dsa", "ENABLE_IN_NON_GUI_APPS").toUpper() == "Y");

  _g->prog( new CProgressFactory_CLI );

	CDianaStartUp* dsu = CDianaStartUp::instance();
	dsu->SetDianaEnv();
	dsu->Print_DianaEnv();
  
  int result = -1;
  
  try
  {
    result = RUN_ALL_TESTS();
  }
  catch (std::exception const &e)
  {
    std::cerr << argv[0] << ": caught exception: " << e.what() << std::endl;
  }

  if (interactive)
    std::cin.ignore();

  	printer->info( "" );
	printer->info( "tests << <<" );
	printer->info("");

  return result;
}
