// QT += widgets
#include <QApplication>
#include <QTextStream>
#include <QObject>
#include <QProcess>
#include <QtWidgets\QMessageBox>
#include <QFile> // copy files
#include <QDir> // current path

// std
#include <iostream>
#include <string>

// own
#include "QUtil.h"
#include "Printer.h"
#include "SelectorHelper.h"
#include "test.h"
#include "util_.h"

namespace
{
	static const std::string LOGS_FILE = "gm_selector.log";

	static const QString TITLE = "Geomec selector"; 
	static const QString VERSION_NOT_FOUND = "Version specified in 'selector.xml' not found";

	static const  QString DEFAULT_INST_START = getenv("PROGRAMFILES");

	static const QString PACKAGES_PATH	= QUtil::url( QString::fromStdString( util::GetAppPath() ), "packages");

	static const QString SEP_W	= "\\";

	static const QString BLADE_INST_START = "c:\\Apps\\PetroSigns";

	Printer* printer = Printer::instance(Printer::Sel);
}

///////////////////////////////////////////////////////
//
//				NOTES 
//
///////////////////////////////////////////////////////

/*
-->
QProcess Start param
--------------------
p2->start( exe );
exe have to be the file not the fullpath

-->
File as an argument
-------------------
It works
VS command arguments:
C:\aWork\aProjects\2019\06\DSA\test1_tetra.gm5

-->
Working dir:
-----------
4a) Doesn't work:
	p1.setWorkingDirectory( gm_path_version ); 
4b) Works:
	QDir::setCurrent( gm_path_version );

-->
Detached process:
----------------
Process launched detached... so no need to keep selector running

Order counts:
// 1. -> KO
//p1.startDetached( GEOMEC_BIN );

// 2. -> OK
p1.setProgram( GEOMEC_BIN );
p1.startDetached();

--> 
Version folder naming:
---------------------
Folder name
Can be changed but version, build and 'build' word needs to remain 


*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//									-- main --
//
// Debugging > Command Arguments > "C:\aWork\aProjects\2019\12\Test export\test1_tetra.gm5"
//
// Launch cases:
// -Double click on gm4,gm5 file -> if default version then open default version else open dlg
// -Click on Start menu -> Same as above
// -Click unzipped version: open unzipped version regardless of the default version
//
// Ways to change default version:
// -From the selector when dlg is shown
// -From Geomec > Settings > Last menu item: 'Mark as default' / 'Un-mark as default' (only current version can be set/unset)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main_( int argc, char *argv[] )
{
	QApplication a(argc, argv);
	TEST_06();
	return 0;
}

int main( int argc, char *argv[] )
{
	QApplication a(argc, argv);

	printer->file( LOGS_FILE );

	// zip versions not allowed !!!
#ifdef KK
	QString _path = QString::fromStdString( util::GetAppPath() ); // current path
	if( !_path.startsWith( DEFAULT_INST_START ) && 
		!_path.toUpper().startsWith( BLADE_INST_START.toUpper() ) )
	{
		printer->info( "path: %s", _path.toStdString().c_str() );
		printer->info( "Installation location not allowed -> EXIT" );
		/*QMessageBox::warning( 
			nullptr, 
			"Launch Geomec", 
			"ZIP versions not allowed !!!" );*/
		return 1;
	}
#endif

	bool is_zip = true; 
	QString zip_version;
	QString version_;
	bool checked = false;

	/////////////////////////////////////////////////////////// 0-a) Config files: selector + Geomec

	// if config file doesn't exist then copy it
	QString file = QString::fromStdString( app::CONFIG_FILE );
	if( !QFile::exists( file ) )
	{
		QFile::copy( 
			QUtil::url( PACKAGES_PATH, "selector.xml" ) , 
			file );
		QFile::setPermissions( 
			file, 
			QFile::ReadOther | QFile::WriteOther);

	}

	// what if 'geomec.ini' doesn't exist? is it created?
	// TODO: verify if needed
	// needed to migrate the registry
	file = version::config_file_url();
	if( !QFile::exists( file ) )
	{
		QFile::copy( 
			QUtil::url( PACKAGES_PATH, "geomec.ini" ), 
			file );
		QFile::setPermissions( 
			file, 
			QFile::ReadOther | QFile::WriteOther);
	}

	/////////////////////////////////////////////////////////// 0-b) Migrate
	migrate::do_();

	/////////////////////////////////////////////////////////// 0-c) Logs
	printer->info("arg[0]:%s", argv[0]);
	printer->info("");
	printer->info("geomec_selector > main");
	//qInstallMessageHandler( qdebug_msg_handler );

	/////////////////////////////////////////////////////////// 1) get version to launch

	// Just for testing...
	// version::unset();

	// if not set then usr chooses it from a dlg

	// no arguments...
	// so not when double click on a model file (gm4,gm5)
	// we decide if its a zip version
	// if so we add current path to locations in config file
	bool b_model = argc == 2;
	if( !b_model )
	{
		QString p = QDir::toNativeSeparators( QString::fromStdString( util::GetAppPath() ) );
		QString path = p.left( p.lastIndexOf( SEP_W ) );
		QString ver = p.right( p.length() - p.lastIndexOf( SEP_W ) -1 );

		printer->debug("no model");
		printer->debug("url:%s", p.toStdString().c_str());
		printer->debug("path:%s", path.toStdString().c_str());
		printer->debug("ver:%s", ver.toStdString().c_str());
		
		// installed version
		if( path.startsWith( DEFAULT_INST_START ) )
			is_zip = false;
		// zip version
		else
			zip_version = ver;

		app::paths::add( path ); // add current instance to the locations list
		app::paths::tidy(); //remove locations with no installations
	}

	// Either:
	// -call from the start menu
	// -call with model as a parameter 
	// In both cases:
	// 1. Default version in config file
	// 2. Otherwise: dialog
	if( zip_version.isEmpty() ) 
	{
		printer->debug("zip version: no");
		std::tie( version_, checked ) = version::get(); // from: 1) config file 2) usr dlg
	}
	// Click on 'geomec_selector.exe' in the unzipped version
	else 
	{
		printer->debug("zip version: yes");
		version_ = zip_version;
	}

	// close dlg clicking on the cross
	if( version_.isEmpty() )
	{
		printer->debug("version empty 1 -> close dlg with cross -> EXIT" );
		return 1;
	}

	if(	version_ == "ERROR" ) //folder doesn't exist
	{
		printer->debug("version_:%s -> ERROR -> show dlg",version_.toStdString().c_str());

		// version in 'selecrtor.xml' no found
		QMessageBox::warning( nullptr, TITLE, VERSION_NOT_FOUND );

		QStringList paths = app::paths::get() << app::paths::default_();
		paths.removeDuplicates();

		std::pair< std::vector< QString >, std::vector< QString > > versions = version::list_ext( paths );

		// if only one version skipe the dialog
		if( versions.first.size() == 1 )
		{
			version_ = versions.first[ 0 ];
			checked = false;
		}
		else
			std::tie( version_, checked ) = version::get_GUI( versions );

		// user closes dlg without chosing any version
		if( version_.isEmpty() )
		{
			printer->debug("version empty 2 (after error) -> close dlg with cross -> EXIT" );
			return 1;
		}
	}

	// here version can't be worng
	if( checked )
		version::set( version_ );

	/////////////////////////////////////////////////////////// 2) launch (selected) version
	for( int i = 0; i < argc; ++i ) 
		printer->debug("arg: %s", argv[ i ] );
	
	file = argc > 1 ? argv[ 1 ] : ""; // file to open

	QString path = QUtil::url( app::paths::get( version_ ), version_ );
	QString bin = "Geomec.exe";
	QString params = file;
	TMap envvars_m = app::envvars( path.toStdString() ); 

	app::launch( path, bin,	params,	envvars_m );
	
	// process launched detached so no need to keep selector running
	// return a.exec();

	return 0;
}