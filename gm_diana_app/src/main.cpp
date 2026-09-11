#include "Printer.h"
#include "QUtil.h"
#include "SettingsIni.h"
#include "GeomecInterface.h"

namespace
{
  Printer* printer = Printer::instance(Printer::DiaE);
}

int main_(int argc, char *argv[])
{
  int a=0;
  for(;;){ if( a ) break; }
  return 0;
}

//
// taskkill /IM "gm_diana_app.exe" /F
//
int main(int argc, char *argv[])
{
  try
  {
    CSettingsIni::instance()->init();
    //
    //
    //
    Printer::pid( std::stoll( argv[2] ) );
    //
    //
    //
    std::string filename = "gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log";
    std::string url = QUtil::url(QUtil::eUsrDir::Goemec, filename);
    //
    // many of these modules are not needed as they are never used, only DiaE and RunU
    //
    for (int i = 0; i < Printer::ModulesNum; i++)
      Printer::instance((Printer::eModule)i)->url(url);

    printer->info( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
    printer->info( "gm_diana_app >>" );
    //
    //
    //
    std::string token=argv[1];
    GI gi(token);
    //
    // TODO: 
    //		- what if no command line params
    //		- what if it's not a number?
    //
    printer->info("args: token: %s ... gm pid: %s",argv[1],argv[2]);
    //
    // wait till GI frees the cond var
    //
    gi.wait();
    //
    //
    //
    printer->debug( "gm_diana_app : end try block <<" );
  } // quit 'listen' and 'send' threads on GI destructor (IListenerIpc)
  //
  // kill is not ctached here
  //
  catch(...)
  {
    printer->error( "gm_diana_app exception catched !!!!" );
  }
  printer->info( "gm_diana_app <<" );
  printer->info( "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" );
  return 0;
}