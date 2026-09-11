#pragma once

#include "SelectorHelper.h"
#include "SelectorDlg.h"

#include "QUtil.h"
#include "Printer.h"
#include "Xml.h"
#include "util_.h"


//app::exec
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <stdio.h>


// Qt
#include <QProcess>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QSettings>
#include <QDir>
#include <QString>

namespace
{

	// "LOCALAPPDATA" ?
	static const QString SETTINGS_PATH_USR = QString( getenv("APPDATA") ) + "/" + "Geomec";
	static const QString SETTINGS_PATH_SYS = QString( getenv("ProgramData") ) + "/" + "Geomec"; // by now not used

	//static const QString FILENAME = "geomec.sys.ini";
	static const QString FILENAME = "geomec.ini";

	// Just for the version
	static const QString SECTION = "Settings";
	static const QString ENTRY = "version";
	static const QString NOTSET = "NOTSET";

	static const std::string OIVARCH = "arch-Windows-x86_64-msvc14-Release";
	// FIXME
	static const std::string PATH_OIVHOME = "C:\\aWork\\aOIV\\OpenInventorC++971-Windows-Visual2015-x86_64";


	static const QString VERSION_HC = "5.7.0.4_build_19241.1";



	static const QString DIR_FILTER = "*_build_*";

	static const QString GEOMEC_BIN = "Geomec.exe";

	static const QString REG_PATH = "HKEY_LOCAL_MACHINE\\SOFTWARE\\TNO";

	Printer* printer = Printer::instance(Printer::Sel);
}

///////////////////////////////////////////////////////////////////////
// -> 0) logs
///////////////////////////////////////////////////////////////////////

// we can use this to suppress qDebug output, but it won't suppress Qt internal messages
// for that we need preprocessor definitions
void myMessageOutput(
	QtMsgType type, 
	const QMessageLogContext &context, 
	const QString &msg)
{
}

//QString LOGS_FILE = QString::fromStdString( Printer::DEFAULT_FOLDER ) + "/geomec_qdebug.log";
//QFile g_qdebug_out_file( LOGS_FILE );
//QFile g_qdebug_out_file;
void qdebug_msg_handler( 
	QtMsgType type, 
	const QMessageLogContext & context, 
	const QString & msg )
{
	std::string url_ = QUtil::url( printer->folder(), "geomec_qdebug.log");
	QFile g_qdebug_out_file( url_.c_str() );

    QString txt;

	//QString s_time = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
	QString s_time = QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss");

    switch (type) 
	{
		// (%s:%u, %s)
		// context.file, context.line, context.function
		case QtDebugMsg:	txt = QString("Debug: %1: %2").arg( s_time, msg); break;
		case QtWarningMsg:	txt = QString("Warning: %1").arg(msg); break;
		case QtCriticalMsg: txt = QString("Critical: %1").arg(msg); break;
		case QtFatalMsg:	txt = QString("Fatal: %1").arg(msg); break;
    }
    
    g_qdebug_out_file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts( &g_qdebug_out_file );
    ts << txt << endl;
	g_qdebug_out_file.close(); // ????
}

/////////////////////////////////////////////////////////////////////// 
// -> 1) app
/////////////////////////////////////////////////////////////////////// 

namespace app
{

/////////////////////////////////////////////////////////////////////// 
// -> 1b) app::paths
/////////////////////////////////////////////////////////////////////// 

namespace paths
{

static const QString DELIM = ";";
static const std::string KEY = "location";

// https://www.askvg.com/list-of-environment-variables-in-windows-xp-vista-and-7/
// default locations: Temp, usr Desktop and Documents
// app::paths::
QStringList default_()
{
	QStringList list; 
	QString usr = getenv("USERPROFILE"); // also "HOMEPATH"
	QString appdata = getenv("APPDATA");

	list << QUtil::url( QString( getenv("PROGRAMFILES") ), "Geomec - SVS" )
		 << usr
		 << QUtil::url( usr, "Desktop")
		 << QUtil::url( usr, "Documents")
		 << QUtil::url( usr, "Downloads")
		 << appdata
		 << QUtil::url( appdata, "Geomec")
		 << getenv("LOCALAPPDATA")
		 << getenv("TMP")
		 << getenv("TEMP"); // C:\Users\Username\AppData\Local\Temp

	list.removeDuplicates();
	return list;
}

// from config file
// app::paths::
QStringList get()
{
	QStringList paths;
	QString val = QString::fromStdString( CXml::get( app::CONFIG_FILE, paths::KEY ) );
	
	if( val != "NOT_FOUND" )
	{
		val.replace( "\n", DELIM );
		paths = val.split( DELIM );
		for( auto& path : paths )
		{
			path = path.simplified();
			if( path.isEmpty() )
				paths.removeOne( path );
		}
		paths.removeDuplicates();
	}
	return paths;
}

// get path for a given version
// app::paths::
QString get( const QString& version )
{
	QStringList paths = app::paths::get() << app::paths::default_();
	paths.removeDuplicates();
	foreach( QString path, paths )
		if( QDir( QUtil::url(path,version ) ).exists() )
			return path;
	return QString();
}

// to conffig file
// app::paths::
void write( const QStringList& paths )
{
	CXml::update( CONFIG_FILE, paths::KEY, paths.join( DELIM ).toStdString() );
}

// discard duplicates
// app::paths::
void add( const QString& path )
{
	CXml::update( CONFIG_FILE, paths::KEY, ( get() << path ).join( DELIM ).replace( DELIM, "\n" ).toStdString() );
}

// from config file paths with no installations
// app::paths::
void tidy()
{
	QStringList paths = paths::get();

	for( auto& path : paths )
	{
		QStringList l = version::list( path );
		if( l.isEmpty() )
			paths.removeOne( path );
	}
	paths::write( paths ); 
}

} // paths



#ifdef KK
void read_output()
{

	printer->debug("out:%s",
		p1.readAllStandardOutput().constData() );

	/*while( p1.canReadLine())
	   printer->debug(
		   "out: %s", p1.readLine().toStdString().c_str() );*/
}

void read_error()
{
	printer->debug("err:%s",
		p1.readAllStandardError().constData() );

	/*while( p1.canReadLine())
	   printer->debug(
		   "err: %s", p1.readLine().toStdString().c_str() );*/
}

#endif

// path: version fullpath
// app::
TMap envvars( const std::string& path )
{
	TMap map_;

	//std::string gm_path_v1 = QUtil::url( VERSIONS_PATH, version ).toStdString();

	// 1a) diana
	std::string diana_path	= QUtil::url( path, "Diana" );
	std::string diana_path_1 = QUtil::url( diana_path, "bin");
	std::string diana_path_2 = QUtil::url( diana_path, "binseg\\ap");

	std::vector< std::string > path_v = 
	{ 
		path, 

		diana_path_1, 
		diana_path_2

		//qt_path_2, qt_path_3
	};

	std::string path_v_s;
	for( auto p : path_v )
	{
		path_v_s += ( p + ";" );
		//printer->debug("Path: + %s", p.c_str() );
	}

#if 0
  // 1b) dll
	std::string dll_path = QUtil::url( path, "dll");
	std::vector< std::string > dll_v = {"flex","oiv","qt","qt/platforms","qwt","tbb","usr","other"};
	for( auto p : dll_v )
	{
		path_v_s += ( QUtil::url( dll_path, p ) + ";" );
		//printer->debug("Path: + %s", QUtil::url( dll_path, p ).c_str() );
	}

	path_v_s.pop_back();
	//return path_v_s;
#endif
	// 2)

	map_[ "Path" ] = path_v_s;
	//map_[ "QT_QPA_PLATFORM_PLUGIN_PATH" ] = qt_platforms_path;
	map_[ "QT_QPA_PLATFORM_PLUGIN_PATH" ] = QUtil::url( path, "platforms" );

	// OIV
	// FIXME: HC
	//map_[ "OIVARCH" ] = OIVARCH; // ??????????
	//map_[ "OIVHOME" ] = PATH_OIVHOME;
	//map_["OIV_DLL_DEBUG"]="c:\aWork\aOIV\OpenInventorC++971-Windows-Visual2015-x86_64\arch-Windows-x86_64-msvc14-Debug\bin";
	//map_["OIV_LICENSE_DEBUG"]="C:\Apps\oiv_debug.txt";
	map_[ "OIV_LICENSE_FILE" ] = QUtil::url( path, "password.dat");

	// License: 2x ?

	return map_;
}

// system call instead of by QProcess
// not used
// app::
void launch_system_call( 
	const QString& path, 
	const QString& file )
{
	TMap envvars_m = app::envvars( path.toStdString() ); 

	printer->debug("Work around !!!");

	std::string cmd=
	//QString::asprintf("set path=%s && set QT_QPA_PLATFORM_PLUGIN_PATH=%s && Geomec.exe %s", 
	QString::asprintf(
"set path=%s && \
set QT_QPA_PLATFORM_PLUGIN_PATH=%s && \
geomec_launcher.exe", 
		envvars_m[ "Path" ].c_str(),
		envvars_m[ "QT_QPA_PLATFORM_PLUGIN_PATH" ].c_str(), //.toStdString();
		file ).toStdString(); // gm5 file full path to open (if it's passed as a paremeter)

	//std::string cmd = "set path=" + envvars_m[ "Path" ] + " && Geomec.exe " + argv[ 1 ];

	printer->debug("cmd:%s", cmd.c_str());
	system( cmd.c_str() );
}

// data request
// app::
bool launch( 
	const QString& path, 
	const QString& bin, 
	const QString& param,
	const TMap& envvars_m,
	QString& result )
{
	QProcess p1;

	if( envvars_m.size() )
	{
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		for( auto& kv : envvars_m )
			env.insert( kv.first.c_str(), kv.second.c_str() );
		p1.setProcessEnvironment( env );
	}

	QString current = QDir::currentPath();
	if( path.isEmpty() )
		p1.setProgram( bin );
	else
	{
		QDir::setCurrent( path );
		//
		// ??????? mcr 2020-03-19
		//
		p1.setProgram( bin + "\\" + bin );
	}
	if( !param.isEmpty() )
		p1.setArguments( QStringList() << param );
	p1.start();
	if (!p1.waitForStarted())
        return false;

	if( !p1.waitForFinished() )
        return false;
    QByteArray result_ = p1.readAll();
	result = QString::fromStdString( result_.toStdString() );

	QDir::setCurrent( current );
	return true;
}

// detached
// app::
void launch( 
	const QString& path, 
	const QString& bin, 
	const QString& param,
	const TMap& envvars_m )
{
	printer->debug("");
	printer->debug("app::launch");
	printer->debug("path: %s",path.toStdString().c_str());
	printer->debug("bin: %s",bin.toStdString().c_str());
	printer->debug("param: %s",param.toStdString().c_str());
	for( auto& kv : envvars_m )
		printer->debug("%s: %s",kv.first.c_str(), kv.second.c_str());

	QProcess p1;

	if( envvars_m.size() )
	{
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		for( auto& kv : envvars_m )
			env.insert( kv.first.c_str(), kv.second.c_str() );
		p1.setProcessEnvironment( env );
	}
	
	QString current = QDir::currentPath();
	if( path.isEmpty() )
		p1.setProgram( bin );
	else
	{
		QDir::setCurrent( path );
		p1.setProgram( path + "\\" + bin );
	}
	if( !param.isEmpty() )
		p1.setArguments( QStringList() << param );
	p1.startDetached();
	QDir::setCurrent( current );
}
// needed to get geomec_shell version / currently not used
// app::
std::string exec( const char* cmd ) 
{
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr< FILE, decltype(&_pclose) > pipe( _popen(cmd, "r"), _pclose );

    //if (!pipe)
    //    throw std::runtime_error("popen() failed!");
    
	while( fgets( buffer.data(), (int)buffer.size(), pipe.get() ) != nullptr )
        result += buffer.data();

	return result;
}

} // app



////////////////////////////////////////////////////////////////////////// 
//
//				2) version: get, set, read
//
////////////////////////////////////////////////////////////////////////// 

// from/to usr/sys config file
namespace version
{
static const std::string KEY = "version";

//version::
QString config_file_url( version::eType type_ )
{
	QString path = type_ == version::User ? SETTINGS_PATH_USR : SETTINGS_PATH_SYS;
	return QUtil::url( path, FILENAME );
}

// selector.xml
//version::
void unset()
{
	CXml::update( app::CONFIG_FILE, version::KEY, "" );
}

// geomec.ini
//version::
void unset_OLD()
{
	QSettings settings( version::config_file_url(), QSettings::IniFormat );
	settings.remove( QUtil::url( SECTION, ENTRY ) );
}

// from selector.xml
//version::
QString read()
{
	std::string res = CXml::get( app::CONFIG_FILE, version::KEY );
	return res == CXml::NOT_FOUND ? "" : QString::fromStdString( res );
}
// from geomec.ini
//version::
QString read_OLD()
{
	QSettings settings( version::config_file_url(), QSettings::IniFormat );
	return settings.value( QUtil::url( SECTION, ENTRY ), NOTSET ).toString();
}

// to selector.xml
//version::
void set( const QString& version )
{
	CXml::update( app::CONFIG_FILE, version::KEY, version.toStdString() );
}
// to geomec.ini
//version::
void set_OLD( const QString& version )
{
	QSettings settings( version::config_file_url(), QSettings::IniFormat );
	settings.setValue( QUtil::url( SECTION, ENTRY ), version ); //section + '/' + entry
}

//version::
QStringList list( const QString& path )
{
	QDir dir( path, DIR_FILTER, QDir::Name, QDir::Dirs | QDir::NoDot | QDir::NoDotDot ); // ????
	return dir.entryList();
}

//version::
QStringList list( const QStringList& paths )
{
	QStringList versions_l;

	for( const auto& path : paths )
		versions_l << list( path );

	return versions_l;
}

//version::
std::pair< 
	std::vector< QString >, 
	std::vector< QString > > list_ext( const QStringList& paths )
{
	std::vector< QString > version_v;
	std::vector< QString > paths_v;

	for( const auto& path : paths )
	{
		QDir dir( path, DIR_FILTER, QDir::Name, QDir::Dirs | QDir::NoDot | QDir::NoDotDot ); // ????
		foreach( QString folder, dir.entryList() )
		{
			// sometimes version is removed but folder still there
			QString selector_url = ( QStringList() << path << folder << "geomec_selector.exe" ).join("\\");
			if( QFileInfo( selector_url ).exists() )
			{
				version_v.push_back( folder );
				paths_v.push_back( path );
			}
		}

	}
	return std::pair< std::vector< QString >, std::vector< QString > >( 
		version_v, 
		paths_v );
}

// ask the user to choose one
//version::
std::pair< QString, bool > get_GUI( 
	const std::pair< 
		std::vector< QString >, 
		std::vector< QString > >& versions )
{
	QString version_;
	bool check = false;

	//QDir dir( VERSIONS_PATH, DIR_FILTER, QDir::Name, QDir::Dirs | QDir::NoDot | QDir::NoDotDot ); // ????
	//QStringList versions_l = dir.entryList();

	// idx, checked?
	CSelectorDlg dlg( versions );
	int res1 = dlg.exec();

	if( res1 )
	{
		int idx = dlg.id();
		version_ = versions.first[ idx ];
		check = dlg.check();
	}
	else // when dlg closed clicking the cross 
	{
		version_="";
	}
	return std::pair<QString,bool>(version_,check);
}


//from: 1)config file 2)dlg
//version::
std::pair<QString,bool> get()
{
	QString version_;
	bool check = false;

	version_ = version::read();
	printer->debug("version::read:%s",version_.toStdString().c_str());

	// 2) Found in the config file
	//if( version_ != NOTSET )
	if( !version_.isEmpty() )
	{
		QString path = app::paths::get( version_ );
		if( !path.isEmpty() )
		{
			QString version_url = QUtil::url( path, version_ );
			printer->debug("version_url:%s",version_url.toStdString().c_str());
		}
		else
			version_="ERROR";

		return std::pair<QString,bool>(version_,check);
	}
	else
	{
		// 3) Ask the user to choose one
		QStringList paths = app::paths::get() << app::paths::default_();
		paths.removeDuplicates();

		//QStringList versions = version::list( paths );
		std::pair< std::vector< QString >, std::vector< QString > > versions = version::list_ext( paths );

		// if only one version skipe the dialog
		if( versions.first.size() == 1 )
			return std::pair< QString, bool >( versions.first[ 0 ], false );

		return version::get_GUI( versions );
	}
}

//
// FIXME:
//
// ENVVARS needed, this wouldn't work in an clean environment
// If called from Geomec, session should already have the ENVVARS -> TODO: verify it
// executes geomec_shell --version
// result needs to be parsed to extract version and build
// not used currently 
//
//version::
TPair get_()
{
	// FIXME
	//QString cmd = "C:/aWork/aGeomec/Trunk_0728/x64/ReleaseWithDebug/geomec_shell.exe --version";
	QString cmd = QUtil::url( 
		QString::fromStdString( util::GetAppPath() ),
		"geomec_shell.exe --version" );

	std::string res = app::exec( cmd.toStdString().c_str() );
	/*geomec_shell 5.7.0.9 dev build 19333.1
	supports files up to version 4.1.75
	using interface RGI*/
	QStringList lines = QString::fromStdString(res).split("\n");
	QString version, build;
	if( lines.size() )
	{
		QString line = lines[0];
		QStringList words = line.split(" ");
		for( auto w : words )
		{
			int count = w.count(".");
			if( count == 3 ) //5.7.0.9
				version = w;
			else if( count == 1 ) //19333.1
				build = w;
		}
	}
	return TPair( 
		version.toStdString(), 
		build.toStdString() );
}
// not used currently 
//
//version::
TPair get__()
{
	QString path = QString::fromStdString( util::GetAppPath() );
	QString bin = "geomec_shell.exe";
	QString params = "--version";

	TMap envvars_m = app::envvars( path.toStdString() ); 

	QString result;
	bool res = app::launch( path, bin,	params,	envvars_m, result );
	
	TPair ver = version::parse_( result );
	return ver;

}

// parses result of 'get_' and 'get__' methods
// not used currently
//
//version::
TPair parse_( const QString& ver )
{
	QStringList lines = ver.split("\n");
	QString version, build;
	if( lines.size() )
	{
		QString line = lines[0].trimmed();
		QStringList words = line.split(" ");
		for( auto w : words )
		{
			int count = w.count(".");
			if( count == 3 ) //5.7.0.9
				version = w;
			else if( count == 1 ) //19333.1
				build = w;
		}
	}
	return TPair(
		version.toStdString(), 
		build.toStdString() );
}

}

////////////////////////////////////////////////////////////////////////// 
//
//			3) migrate: registry -> config file
//
//////////////////////////////////////////////////////////////////////////
/*
UserResult:
	Resultd%d
Settings:
	XMLMatLib
	OIVLicenseAttempt
	UNIT
	UnlockGVT
	MAGIC
	DCASINT_MAGIC
	MODEL2D_MAGIC
	UserTempPath
	CalcTempPath
	UserTemp
	Build ??? Not found in code
Paths:
	FvgExeName ???? where?
	[...]
ColorScale
	Local
PointSetDlg
	DefaultValueType
Window size -> AfxGetApp()->GetProfileInt -> easy to migrate
	Rect
	icon
	max
	MainFrameSplitter
	OpenGLSplitViewSplitter
	TreeSplitViewSplitter
		
Recent File List: Implement it
*/
namespace migrate{

//migrate::
void section( 
	const QString& section_, 
	const std::vector< QString >& keys_v  )
{
	bool found = false;

	QSettings settings_dest( version::config_file_url(), QSettings::IniFormat );

	for( int j = 7; j >= 0; j-- )
	{
		QString path = REG_PATH + "\\Geomec 5.%d\\" + section_;
		QSettings settings_reg( path, QSettings::NativeFormat );
		
		QString value;
		for( const auto& key: keys_v )
		{
			value = settings_reg.value( key, "NOT_FOUND" ).toString();
			if( value != "NOT_FOUND" )
			{
				found = true;
				settings_dest.setValue( section_ + '/' + key, value );
			}
		}

		if( found )
			break;
	}
}

//migrate::
void all_but_udr()
{
	migrate::section( migrate::SETTINGS_SECTION,		migrate::SETTINGS_KEYS );
	migrate::section( migrate::PATHS_SECTION,			migrate::PATHS_KEYS );
	migrate::section( migrate::COLOR_SCALE_SECTION,		migrate::COLOR_SCALE_KEYS );
	migrate::section( migrate::POINT_SET_DLG_SECTION,	migrate::POINT_SET_DLG_KEYS );
	migrate::section( migrate::WINDOW_SIZE_SECTION,		migrate::WINDOW_SIZE_KEYS );
}

// get user defined results
//
////migrate::
std::vector< std::string > udr()
{
	QString section = "UserResult";

	std::vector< std::string > results_v;

	for( int j = 7; j >= 0; j-- )
	{
		QString path = REG_PATH + "\\Geomec 5.%d\\" + section;
		QSettings settings( path, QSettings::NativeFormat );

		QString val;
		int i = 0;
		while( i < 100 )
		{
			val = settings.value( QString::asprintf("Result%d", i) ).toString();
			if( val.isEmpty() )
				break;
			results_v.push_back( val.toStdString() );
			i++;
		}
		if( results_v.size() )
			break;
	}

	return results_v;
}

// set user defined results
//
//migrate::
void udr( std::vector< std::string > results_v )
{
	QSettings settings( version::config_file_url(), QSettings::IniFormat );

	QString aDefault;
	QString section = "UserResult";
	
	int i = 0;
	for( auto result : results_v )
	{
		QString  entry = QString::asprintf("Result%d", i);
		settings.setValue( 
			section + '/' + entry, 
			QString::fromStdString( result ) );
		i++;
	}
}

//migrate::
bool need()
{
	QSettings settings( version::config_file_url(), QSettings::IniFormat );

	QString  section = "UserResult";
	QString  entry = QString::asprintf("Result0");
	QString val = settings.value( section + '/' + entry ).toString();

	return val.isEmpty();
}

//migrate::
void do_()
{
	std::string key = "migrate";
	CXml cfg( app::CONFIG_FILE, {}, { key } );
	bool do_migrate = false;
	if( cfg.exist( key ) )
	{
		QString v = QString::fromStdString( cfg.value( key ) ).toUpper();
		if( v.isEmpty() || v == "Y" || v == "Yes"  )
			do_migrate = true;
	}
	if( do_migrate )
	{
		std::vector< std::string > r = migrate::udr();
		migrate::udr( r );

		migrate::all_but_udr();
		CXml::update( app::CONFIG_FILE, key, "N" );
	}
}

} // namespace migrate