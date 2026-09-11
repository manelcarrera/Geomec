#include "headers.h"

//own
#include "Printer.h"
#include "SettingsIni.h"
#include "QUtil.h"
#include "DianaInterface.h"
//qt
#include <QString> 
#include <QProcess> 
#include <QStringList>
//std
#include <chrono>

/////////////////////////////////////////////////
//
//			kill a process
//
/////////////////////////////////////////////////

//
// Test: 
// 
// 1) Is a process running
// 2) Kill a process
//
void Class_02::start()
{
	CSettingsIni::instance()->init();

	//Printer::multiprocess( true );
	std::string filename = "gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log";
	Printer::instance()->url( QUtil::url( QUtil::eUsrDir::Goemec, filename ) );
	Printer::instance()->info( "" );
	Printer::instance()->info( " main_app_qt >>>>>>" );
	Printer::instance()->info( "" );

	QString result;
	//
	// FIXME: only for windows
	//
	//const QString cmd = "cmd /c taskkill /IM diana_app_new.exe /F";
	const QString cmd = "cmd /c \"taskkill /IM /F \"diana_app_new.exe\"\"";

	//
	// FIXME: code needs to be adapted to last update
	//
	//gm::di::launch( "diana_app_new.exe" );

	std::this_thread::sleep_for(std::chrono::milliseconds( 5*1000 ));

	//
	// 1)sending all together instead of bin + prams
	// it doesn't work
	// returns by timeout
	//
	// 2)with params:
	// dont work either
	// but it return inmediatelly
	//
	// conclusion:
	//
	// I don't see why it's not working... this was working fine before in 'Geomec.cpp'
	//
	// %COMSPEC%
	// C:\WINDOWS\system32\cmd.exe
	// it doesn't work even giving the full binary path 
	//
	// 'cmd' and 'cmd.exe' no one does the work
	//
	/*	QString bin = "C:\\WINDOWS\\system32\\cmd.exe";
	// quotation marks not needed when cmd is executed fromn command line
	QStringList params = QStringList() << "/c" <<  "taskkill /IM /F \"diana_app_new.exe\"";

	int res = gm::di::exe( bin,	params, result );

	Printer::instance()->info( "taskkill : res:%d : '%s'", res, result.toStdString().c_str() );*/

	//std::string res_ = gm::di::exec( "cmd /c \"taskkill /IM /F diana_app_new.exe\"" ); 
	/*std::string res_ = gm::di::exec( "cmd /c taskkill /IM /F diana_app_new.exe" ); 
	Printer::instance()->info( "taskkill : '%s'", res_.c_str() );*/


	//
	// definetely this works!
	//
	// to check it out:
	// tasklist | find "diana_app_new.exe" -> quotation marks mandatory here
	//

	Printer::instance()->info( "running: '%d'", gm::di::running("diana_app_new.exe") );

	//
	// FIXME
	//
	//int res = gm::di::exe( "taskkill",	QStringList() << "/IM" << "/F" << "diana_app_new.exe", result );

	QProcess::execute("taskkill /im diana_app_new.exe /f");

	Printer::instance()->info( "running: '%d'", gm::di::running("diana_app_new.exe") );

	//return 0;
}
