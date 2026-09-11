#include <Windows.h>
#include <string>

// 'QUtil.lib'
#include "System.h" //unpack
#include "QUtil.h"
#include "Printer.h"

// 'Util.lib'
#include "util_.h"

//Qt
#include <qstring>
#include <qdir>
#include <qfileinfo>

static const std::string PROGRAMFILES_PATH	= getenv("programfiles");

static const std::string UNINSTALL_PARAM = "uninstall";

static const std::string LOG_FILE = QUtil::native_sepatators( QUtil::usr_dir( QUtil::eUsrDir::Plugins ) + "/plugins.log");

std::string GetAppPath_() // FIXME: doesn't cover all the cases
{
	QUtil::TFoldersV folders_v = QUtil::folders( PROGRAMFILES_PATH, QUtil::FullPath );
	size_t nE = folders_v.size();
	for( size_t i=0; i < nE; i++ )
	{
		std::string& f = folders_v[ i ];
		if( f.find( "GEOMEC" ) != std::string::npos )
		{
			//bool is_new_installation = !QUtil::exists( QUtil::native_sepatators( f + "/changelog" ), QUtil::File );
			//if( is_new_installation )
				return f;  
		}
	}
	return "ERROR";
}

static const std::string APPPATH			= QUtil::native_sepatators( GetAppPath_() );
static const std::string CHANGELOG_PACKAGE	= QUtil::native_sepatators( APPPATH + "/packages/changelog.zip" );
static const std::string CHANGELOG_DIR		= QUtil::native_sepatators( APPPATH + "/changelog");
static const std::string UNZIP				= QUtil::native_sepatators( APPPATH + "/7za.exe");

static const std::string CSS_FILE = "changelog.css"; //FIXME

class CSystemInstall : public CSystem
{
public:
	virtual std::string get_unzip(){ return UNZIP; };
};

class Changelog
{

public:
	static bool install();
	static bool uninstall();
};

bool Changelog::install()
{
	CSystemInstall sys;

	bool retval = false;

	if( QUtil::exists( CHANGELOG_PACKAGE, QUtil::File ) )
	{
		Printer::instance()->info( "changelog : install : %s", CHANGELOG_PACKAGE.c_str() );
	
		std::string tmp_dir_path_s = QUtil::create_random_dir();
		QString tmp_dir_path = QString::fromStdString( tmp_dir_path_s );

		Printer::instance()->info( "tmp dir : %s", tmp_dir_path_s.c_str() );

		if( QFileInfo( tmp_dir_path ).isDir() )
		{
			bool res_u  = sys.unpack(CHANGELOG_PACKAGE, tmp_dir_path_s );
			if( res_u )
			{
				if( !QUtil::exists( CHANGELOG_DIR, QUtil::Dir ) )
					QDir().mkpath( QString::fromStdString( CHANGELOG_DIR ) );

				QUtil::TFilesV changelog_v = QUtil::files( tmp_dir_path_s, "*.zip", QUtil::eFile::FullPath );

				bool res;
				for( int i=0; i < changelog_v.size(); i++ )
				{
					res = sys.unpack( changelog_v[ i ], CHANGELOG_DIR );
					if( !res )
						break;
				}
				if( res )
				{
					res  = sys.unpack( CHANGELOG_PACKAGE, CHANGELOG_DIR, CSS_FILE );
					if( res )
						retval = true;
				}
			}
			QUtil::remove_dir( tmp_dir_path );
		}
	}
	else
	{
		Printer::instance()->error( "'%s': not found", CHANGELOG_PACKAGE.c_str() );
	}
	return retval;
}

bool Changelog::uninstall()
{
	Printer::instance()->info( "changelog : UNINSTALL : remove:'%s'", CHANGELOG_DIR.c_str() );
	return QUtil::remove_dir( QString::fromStdString( CHANGELOG_DIR ) );
}

int main(int argc, char *argv[])
{
	Printer::instance()->file( LOG_FILE );

	if( argc > 1 ) // uninstall
	{
		std::string param = std::string( argv[ 1 ] );
		Printer::instance()->info( "changelog : uninstall : param:'%s'", param.c_str() );
		if( param == UNINSTALL_PARAM )
		{
			Changelog::uninstall();
		}
		else
		{
			Printer::instance()->error( "param:'%s' not recognised", param.c_str() );
		}
	}
	else
	{
		Printer::instance()->info( "changelog : install" );
		Changelog::install();
	}
	return 0; // -> always, otheriwse installation is aborted
}