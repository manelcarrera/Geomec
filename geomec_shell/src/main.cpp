#include "geomec_shell.h"
#include "ArgumentParser.h"
#include "InterfaceBase.h"
#include "InterfaceCORA.h"
#include "InterfaceRGI.h"
#include "InterfaceDiagnostics.h"
#include "InterfaceGM.h"
#include "GlobalMessage_CLI.h"
#include "GlobalMessage.h"
#include "SettingsFile.h"
#include "SettingsIni.h"
#include "QUtil.h"
#include "Printer.h"
#include "GeomecUtils.h" //GetAppPath


//#ifdef _WIN32
//#include "InterfacePy.h"
//#endif

#include "InterfaceContext.h"

#include "VersionNumbers.h"
#include "ApplicationInitialization.h"
#include "DianaStartUp.h"
#include "Global.h"
#include "ProgressFactory_CLI.h"

//qt
#include <QString>
#include <QCoreApplication> //pid

//std
#include <iostream>
#include <map>



namespace gm_shell
{
Printer* printer = Printer::instance(Printer::Shell);

typedef std::map<std::string, CInterfaceBase *> TInterfaceMap;


void PrintUsage(TInterfaceMap& interfaces)
{
  std::cerr << std::endl
      << "Usage: geomec_shell [ --interface <interface> [<version>] ] [ <interface-dependent parameters> ]" << std::endl
      << "with known interfaces:" << std::endl
      << "\tauto\tautomatic detection based on the parameters (default)" << std::endl;
  for (TInterfaceMap::iterator it = interfaces.begin(); it != interfaces.end(); ++it)
  {
  if (it->second->Expose())
      std::cerr << "\t" << it->first << "\t" << it->second->ShortDescription() << std::endl;
  }
  std::cerr << std::endl;
}


}


using namespace gm_shell;


int main(int argc, char *argv[])
{
  GlobalMessage::init( new GlobalMessage_CLI() );

  // Some general initialization
  CApplicationInitialization application;

  //
  Printer::pid( QCoreApplication::applicationPid() );
  //
  // init modules url : set url (file fullpath)
  //
  std::string ver = CSettingsIni::instance()->version_str().toStdString();
  std::string url = QUtil::url(QUtil::eUsrDir::Goemec, "gm_" + ver + ".log");
  for (int i = 0; i < Printer::ModulesNum; i++)
    Printer::instance((Printer::eModule)i)->url(url);
  printer->info("");
  printer->info("gm_shell >> >>");
  printer->info("");
  //
  // binary path
  //
  printer->info("path: %s", GetAppPath().toStdString().c_str());
  printer->info("");
  //
  // params
  //
  std::string s;
  for(int i=0; i<argc;i++)
    s+=(std::string(argv[i])+" ");
  printer->info("params: %s",s.c_str());
  printer->info("");
  //
  // legend
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


  CDianaStartUp* dsu = CDianaStartUp::instance();
  	dsu->SetDianaEnv(); 
  dsu->Print_DianaEnv();

  //
  //
  //
  QString val = ISettings::instance()->getProfileString("Dsa", "ENABLE_IN_NON_GUI_APPS").toUpper();
  _g->dsa(val == "Y");

  _g->prog( new CProgressFactory_CLI );

  
  std::cout << APPLICATION.toStdString() << " " << VERSION.toStdString() << std::endl << std::endl;
  std::cout << "supports files up to version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_REVISION << std::endl << std::endl;

  // Find the interface
  CArgumentParser argParser(argc, argv);

  TInterfaceMap interfaces;
  interfaces["cora"] = new CInterfaceCORA();
  interfaces["rgi"]  = new CInterfaceRGI();
  interfaces["diagnostics"] = new CInterfaceDiagnostics();
  interfaces["gm"] = new CInterfaceGM();
//#ifdef _WIN32
  //  interfaces["py"] = new CInterfacePy();
//#endif

  std::vector<IInterfaceBase *> activeInterfaces;
  //IInterfaceBase *currentInterface = 0;

  std::string iFace;
  argParser.GetInterface(iFace);

  if (interfaces.count(iFace) == 1)
  {
  activeInterfaces.push_back(interfaces[iFace]);
  activeInterfaces.back()->AcceptParameters(argParser, true);
  }
  else if (iFace == "auto")
  {
  for (std::map<std::string, CInterfaceBase *>::iterator it = interfaces.begin(); it != interfaces.end(); ++it)
  {
      if (it->second->AcceptParameters(argParser))
      {
    activeInterfaces.push_back(it->second);
    break;
      }
  }
  }


  // Run the interface
  int retval = 0;

  if (activeInterfaces.empty())
  {
  PrintUsage(interfaces);
  retval = -1;
  }
  else
  {
  while (!activeInterfaces.empty())
  {
      IInterfaceBase *currentInterface = activeInterfaces.back();

      bool quit = false;

      currentInterface->Startup();

    printer->info("gm_shell : getting into cmds loop");

      while (!quit)
      {
    int command = currentInterface->GetCommand();

    printer->info(">> cmd:%s", cmd_cp[command]);

    switch (command)
    {
    case gm_shell::IInterfaceBase::USAGE:
          currentInterface->PrintUsage();
          break;

    case gm_shell::IInterfaceBase::ABORT:
          retval = currentInterface->GetErrorCode();
          // fall-through
    case gm_shell::IInterfaceBase::QUIT:
          quit = true;

          currentInterface->RegisterSlaveChannel(0);
          activeInterfaces.pop_back();
          break;

    case gm_shell::IInterfaceBase::CALCULATE:
    {
          gm_shell::CInterfaceModelContext *pModelContext = currentInterface->GetModelContext();
          if (pModelContext)
      pModelContext->Run();
    }
    break;
    case gm_shell::IInterfaceBase::END:
    {
          gm_shell::CInterfaceModelContext *pModelContext = currentInterface->GetModelContext();
          if (pModelContext)
      pModelContext->End();
      break;
    }
    case gm_shell::IInterfaceBase::CALCULATE_STEP:
    {
          gm_shell::CInterfaceModelContext *pModelContext = currentInterface->GetModelContext();
          if (pModelContext)
      pModelContext->RunStep();
    }
    break;

    case gm_shell::IInterfaceBase::SWITCH_INTERFACE:
    {
          IChannel *channel = currentInterface->GetMasterChannel();
          if (channel)
          {
      if (interfaces.count(channel->SlaveName()) == 1)
      {
              activeInterfaces.push_back(interfaces[channel->SlaveName()]);
              activeInterfaces.back()->RegisterSlaveChannel(channel);
              quit = true;
      }
          }
    }
    break;

    case gm_shell::IInterfaceBase::FOR_ME:
          currentInterface->HandleLastCommand();
          break;

    default:
          /* ignore */
          break;
    }
      }
    printer->info("gm_shell : out of cmds loop");

      currentInterface->Shutdown();
  }
  }

  printer->info("gm_shell << <<");
  return retval;
}