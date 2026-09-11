#include "Environment.h"
#include "ISettings.h"

#include "QUtil.h"
#include "Printer.h"
#include "util_.h"

#include <QDir>
#include <QStringList>

namespace {
#ifdef _WIN32
  const QString defaultInputExeName = "in51.exe";
  const QString defaultDianaExeName = "gm42.exe";
  const QString defaultFgvExeName	= "fgv4gm.exe";
#else
  const QString defaultInputExeName = "in51";
  const QString defaultDianaExeName = "gm42";
  const QString defaultFgvExeName	= "fgv4gm";
#endif

}


CEnvironment* CEnvironment::instance()
{
  if(!m_singleton)
    m_singleton = new CEnvironment();

  return m_singleton;
}

CEnvironment::CEnvironment()
{
}

QString CEnvironment::DIAPATH			= "DIAPATH";
QString CEnvironment::DIASHARE			= "DIASHARE";
QString CEnvironment::DIALIB			= "DIALIB";
QString CEnvironment::DIASLIB			= "DIASLIB";
QString CEnvironment::DIAERRPATH		= "DIAERRPATH";
QString CEnvironment::STP0				= "STP0";

QString CEnvironment::DianaExeName		= "DianaExeName";
QString CEnvironment::InputExeName		= "InputExeName";
QString CEnvironment::FgvExeName		= "FgvExeName";

QString CEnvironment::APP_SPC_LOGGING	= "APP_SPC_LOGGING";

// Diana default: Local dirs
//
// Order: 
// 1)Registry 
// 2)Config file 
// 3)Default location
// 
// All Diana vars can be generated having 'DIAPATH'
//

//
// -- ENVAVRS --
// Windows 10:
// https://pureinfotech.com/list-environment-variables-windows-10/
// Linux:
// https://www.cyberciti.biz/faq/linux-list-all-environment-variables-env-command/
//
QString CEnvironment::get( const QString key ) const
{
	QString value;

	if( key.isEmpty() )
	{
		Printer::instance(Printer::Gen)->error( "env : get : key is empty");
		return value;
	}

	ISettings* settings = ISettings::instance();

	QStringList DIANA_VARS = QStringList() 
		<< CEnvironment::DIAPATH
		<< CEnvironment::DIASHARE
		<< CEnvironment::DIALIB
		<< CEnvironment::DIASLIB
		<< CEnvironment::DIAERRPATH
		<< CEnvironment::STP0;

	//
	// vars depending on DIAPATH
	//
	bool b_diana_var = DIANA_VARS.contains( key );

	//
	// we assume diana local by default
	//
	bool b_diana_local = true;
	if( b_diana_var )
	{
		QString key_ = settings->getProfileString("Paths", "DianaLocal" ).toUpper();
		b_diana_local = (key_ != "N" && key_ != "NO"); // key not present means diana local
	}

	// 
	//	1) not diana var 
	//
	//	or 
	//
	//	2) diana var but not local
	//
	if( !b_diana_var || !b_diana_local )
	{
		//
		// 1) ENVVAR
		//
		value = getenv( key.toStdString().c_str() );
		Printer::instance(Printer::Gen)->debug( "env : get : getenv : %s -> %s", key.toStdString().c_str(), value.toStdString().c_str() );
		//
		// 2) Registry / Config file
		//
		// "TEMP" is requested on Geomec app creation when settings is still not initialized
		//
		if( value.isEmpty() && settings ) 
			value = settings->getSystemString("Paths", key );
		//
		//
		//
	}

	Printer::instance(Printer::Gen)->debug( "env : get : %s -> %s ... diana_var: %s ... local: %s", key.toStdString().c_str(), value.toStdString().c_str(), b_diana_var ? "yes":"no", b_diana_local ? "yes":"no" );

	//
	// DIAPATH when diana local
	//
	if( value.isEmpty() )
	{
		if( key == DIAPATH )
		{
			//
			// regular case : diana local
			//
			value = QString::fromStdString( QUtil::url( util::GetAppPath(), "diana" ) ); 
		}
		else
		{
			if( key == DianaExeName || 
				key == InputExeName || 
				key == FgvExeName )
			{
				value = key == DianaExeName ?	
					defaultDianaExeName :
					key == InputExeName ?	
						defaultInputExeName : 
						defaultFgvExeName;
			}
			else if( key == APP_SPC_LOGGING )
			{
				value = "YES";
			}
			else if( b_diana_var )
			{
				QString basepath = get( DIAPATH );
				if( basepath.isEmpty() )
				{
					// ERROR: DIAPATH must exists
					Printer::instance(Printer::Gen)->error( "env : get : DIAPATH non-existant");
				}
				else
				{
					QString folder;
					if( key ==  DIASHARE )
						folder = "/share";
					else if( key ==  DIALIB )
						folder = "/lib";
					else if( key ==  DIASLIB )
						folder = "/share/lib";
					else if( key ==  STP0 )
						folder = "/binseg/ap";
					else if( key ==  DIAERRPATH )
						folder = "/share/src";

					value = basepath + folder;
					if( key ==  STP0 )
						value += ( "/" + get( DianaExeName ) );
				}
			}
		}
	}

	return value;
}

CEnvironment* CEnvironment::m_singleton = 0;
