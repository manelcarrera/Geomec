#include "SettingsIni.h"
#include "SettingsFile.h"

#include "QUtil.h"
#include "Printer.h"

#include "version.h"
#include "GeomecUtils.h"

#include <QDir>

#include <QTextStream>

namespace
{
  //
  // can't be here as print_ is called before... I don't know why
  //
  //Printer* printer = Printer::instance(Printer::App);

  enum eConfigFileType{ User,	System };
  enum eConfigFile { Default, Version, ToBeInstalled, Merged };

#ifdef WIN32
  static const char* SETTINGS_PATH_USER = "APPDATA"; //"LOCALAPPDATA"; //FIXME
  static const char* SETTINGS_PATH_SYS = "APPDATA"; //"ProgramData"; // FIXME
#else
  static const char* SETTINGS_PATH_USER = "HOME"; //"LOCALAPPDATA"; //FIXME
  static const char* SETTINGS_PATH_SYS = "HOME"; //"ProgramData"; // FIXME
#endif

  static const char* FILENAME_USER = "geomec.ini"; // "geomec.usr.ini"
  static const char* FILENAME_SYS = "geomec.ini"; // "geomec.sys.ini"
  
  static const char* SEP = "/";

  static const char* GEOMEC_INI_ENVVAR_KEY = "GEOMEC_INI";

  //static const bool MULTIPLE_INSTANCES = true;

  //
  // Order of importance:
  //
  // -geomec.%version%.ini
  // -geomec.ini in APPDATA
  // -copy geomec.ini from INSTALLATION folder to APPDATA and use it
  //

  //
  // Installed:		in '%APPDATA%\Roaming\Geomec'
  //
  //		file:		geomec_5.8.0.0_20105.1.ini
  //		file_DEF:	geomec.ini
  //
  // To be installed:	in '%PROGRAMFILES%\Geomec - SVS\5.7.1.1_build_20133.1' (example)
  //
  //		file_INST:	geomec.ini in 
  //

  //
  // imput: usr / system
  // output: paths vector with VER, DEF, INST and MERGED urls (files fullpath)
  //
  //		VER:	geomec_5.8.0.0_20105.1.ini
  //		DEF:	geomec.ini
  //		MERGED: geomec_5.8.0.0_20105.1_MERGED.ini		
  //		INST:	geomec.ini in installation 'pachages' folder
  //
  std::vector<QString> url_v(eConfigFileType type_)
  {
    QString path_ = getenv(type_ == eConfigFileType::System ? SETTINGS_PATH_SYS : SETTINGS_PATH_USER);

    QString path = QUtil::url(path_, "Geomec");

    QString filename = type_ == eConfigFileType::System ? FILENAME_SYS : FILENAME_USER;
    //
    // 1) file_VER
    //
    QString file_VER_ = filename;
    file_VER_.replace(".ini", "_" + CSettingsIni::instance()->version_str() + ".ini"); // FIXME:
    QString file_VER = QUtil::url(path, file_VER_);
    //
    // 2) file_MERGED 
    //
    // the one to be used
    //
    QString filename_merged = file_VER_;
    filename_merged.replace(".ini", "_MERGED.ini");
    QString file_MERGED = QUtil::url(path, filename_merged);
    //
    // 3) file_DEF
    //
    QString file_DEF = QUtil::url(path, filename);
    //
    // 4) file_INST
    //
    // if geomec.ini not present in %APPDATA%
    //
    QString file_INST = QStringList({ GetAppPath(), "packages", filename }).join(SEP);

    return std::vector<QString>{
      file_DEF, 
      file_VER, 
      file_INST, 
      file_MERGED
    };
  }

  //
  // input: file type: VER, DEF, INST and MERGED
  // output: file type url
  //
  QString settings_url( eConfigFileType type_ )
  {
    QString file;
    const QString GEOMEC_INI = getenv( GEOMEC_INI_ENVVAR_KEY );
    if( !GEOMEC_INI.isEmpty() && QFile::exists( GEOMEC_INI ) )
    {
      return GEOMEC_INI;
    }
    else
    {
      //
      // not optimal
      //
      std::vector<QString> files_v = url_v(type_);

      QString file_VER	= files_v[Version];
      QString file_DEF	= files_v[Default];
      QString file_INST	= files_v[ToBeInstalled];
      QString file_MERGED = files_v[Merged];

      if( QFile::exists( file_VER ) )
        return file_VER;
      else // default file
      {
        if( !QFile::exists( file_DEF ) )
          QFile::copy( file_INST, file_DEF );

        return file_DEF;
      }
    }
  }

  //
  // input: filename: source file
  // output: filename + ".TMP"
 	//
  // replaces '\\' and '\' by '/'
  //
  // given combinations of '\' + something ('d', for example) are deleted when parsed by any Qt class
  //
  void create_tmp_file(const QString& filename)
  {
    // modify file in memory
    QFile f(filename);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&f);
    QString s = in.readAll();
    // yes, do it twice
    s.replace("\\\\","/");	// case: C:\\folder_name
    s.replace("\\","/");	// case: C:\folder_name
    //dump to a new file
    QFile modified_file(filename+".TMP");
    if(modified_file.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&modified_file);
      out << s;
    }
    modified_file.close();
  }

} // namespace


namespace settings
{
namespace test
{
  void update()
  {
    std::vector<QString> files_v = url_v(eConfigFileType::User);
    QSettings s_m(files_v[Merged], QSettings::IniFormat);

    // new key
    s_m.setValue("Debug/TEST_NEW_KEY","TEST_NEW_VALUE");

    // update key
    int v_m = s_m.value("MeshRefinement/Type").toInt();
    s_m.setValue("MeshRefinement/Type",v_m==0?1:0);
  }
}
}

//////////////////////////////////////////////////////////
//
//					CSettingsIni
//
//////////////////////////////////////////////////////////

//
// Involved files:
//
// Default:			%APPDATA%\Geomec\geomec.ini
// Version:			%APPDATA%\Geomec\geomec_5.8.0.0_20335.1.ini
// ToBeInstalled:	path_to_packages\geomec.ini
// Merged:			%APPDATA%\Geomec\geomec_5.8.0.0_20335.1_MERGED.ini
//
CSettingsIni::CSettingsIni()
{
  m_env_DIAPATH = getenv( "DIAPATH" );
}

CSettingsIni::~CSettingsIni()
{
}

//
// new values are set to default config file (not to version one)
//
// not implemented for the non-user case as non-user can't update or add any value
//
void CSettingsIni::update()
{
  //
  // do comment this line if not testing 
  //
  //settings::test::update();
  //

  std::vector<QString> files_v = url_v(eConfigFileType::User);

  //
  // xxx_m: merged
  // xxx_d: default
  // xxx_v: version
  //
  // s_xxx:	settings obj
  // v_xxx:	values
  //
  QSettings s_m(files_v[Merged], QSettings::IniFormat);
  QSettings s_d(files_v[Default], QSettings::IniFormat);
  QSettings s_v(files_v[Version], QSettings::IniFormat);

  for( auto const& k : s_m.allKeys() )
  {
    QString v_m = s_m.value(k).toString();
    //
    // key present in default config file
    //
    if( s_d.contains(k) )
    {
      QString v_d = s_d.value(k).toString();	
      if( v_m!=v_d)
        s_d.setValue(k,v_m);
    }
    //
    // key present in version file
    //
    else if( s_v.contains(k) )
    {
      QString v_v = s_d.value(k).toString();
      if( v_m!=v_v)
        s_v.setValue(k,v_m);
    }
    //
    // new key : add it to default config file
    //
    else
    {
      s_d.setValue(k,v_m);
    }
  }
}


CSettingsIni* CSettingsIni::instance()
{
  static CSettingsIni* _instance = new CSettingsIni();
  return _instance;

}

void CSettingsIni::init()
{
  m_usr_url = settings_url( eConfigFileType::User );
  m_sys_url = settings_url( eConfigFileType::System );
  //
  //
  //
  const QString GEOMEC_INI = getenv( GEOMEC_INI_ENVVAR_KEY );
  QString file;
  if( !GEOMEC_INI.isEmpty() && QFile::exists( GEOMEC_INI ) )
    file=GEOMEC_INI;
  else
    file = create_merged(); // -> %APPDATA%\Geomec\geomec_5.8.0.0_20335.1_MERGED.ini -> create and set
  //
  Printer::instance(Printer::App)->info("--config file--: usr = sys : %s", file.toStdString().c_str());
  //
  CSettingsFile::init(file, file);
}

/*
Issue with '\d' in qt:
  if(val.contains(":")) // is a path
  {
    //
    // FIXME:
    //
    // qt looks like having a problem with '\d' chars sequence
    //
    // it interprets it as an special character and then it removes it
    //
    // any path with a dir starting by 'd' results in a problem
    //
    //val=QDir::toNativeSeparators(val);			// it doesn't work
    //val=QDir::fromNativeSeparators(val);			// it doesn't work either
    //val.replace(QChar('\a'),"\\");				// this works but it's not needed
    //val.replace(QChar('\d'),"\\");				// this doesn't compile, warning interpreted as an error
                            // warning C4129: 'd': unrecognized character escape sequence
    //QByteArray ba = s0.value(k).toByteArray();	// it does't work either
    //QUrl url = s0.value(k).toUrl();				// it does't work either
    //
        

    //
    // char already eaten at this point
    //

    //std::string s = val.toStdString(); 
    //for(char& c : s) 
    //{
    //	int a=0;
    //}
  }
*/
QString CSettingsIni::create_merged()
{
  std::vector<QString> files_v = url_v(eConfigFileType::User);
  //
  QString file = files_v[Merged];;
  //
  // 1) if version config file does not exist create it and let it empty
  //
  // this avoids human error if client needs to create the file
  //
  if (!QFile::exists(files_v[Version]))
  {
    QFile file_(files_v[Version]);
    file_.open(QIODevice::ReadWrite);
    file_.close();
  }

  //
  // 2) create filename + ".TMP" (from filename)
  //
  for(const auto& i : { Version, Default } )
  {
    const QString& url_ = files_v[ i ];
    const QString& url_tmp_ = url_ + ".TMP";
    //
    if( QFile::exists( url_tmp_ ) )
      QFile::remove( url_tmp_ );
    //
    create_tmp_file( url_ ); 
  }
  //
  // 3) merged map with default and version values
  //
  auto read_settings=[]( std::map<QString,QString>& m, const QString url_ )
  {
    QSettings s(url_, QSettings::IniFormat);
    QStringList kk = s.allKeys();
    for( auto const& k : kk )
      m[k]=s.value(k).toString();
  };
  std::map<QString,QString> m;
  for(const auto& i : { Default, Version } )		// order count: read Dafault and then override values with Version
    read_settings( m, files_v[ i ]+".TMP" );
  //
  // 4) _MERGED.ini file
  //
  if (QFile::exists(files_v[Merged]))
    QFile::remove(files_v[Merged]);
  //
  // no need to create the file in advance as far as 'sync' is called at the end
  //
  // furthermore 'sync' needs to be called
  //
  QSettings s2(files_v[Merged], QSettings::IniFormat);
  for (auto const& k : m)
    s2.setValue(k.first, k.second);
  //
  // this needs to be done !!!
  //
  // TODO: is it done in the rest of the code? I dont' thnk so... verify if it's working out
  //
  s2.sync(); 
  //
  for(const auto& i : { Version, Default } )
    QFile::remove(files_v[i]+".TMP");
  //
  return file;
  //
  //
  //
  //CSettingsFile::init(file, file);
}

void CSettingsIni::print_()
{
  Printer* printer = Printer::instance(Printer::App);

  printer->info("--settings-- : usr : %s", m_usr_url.toStdString().c_str());
  printer->info("--settings-- : sys : %s", m_sys_url.toStdString().c_str() );
}

QString CSettingsIni::version_str()
{
  return QString::asprintf(
    "%d.%d.%d.%d_%s", 
      GM_MAJOR, 
      GM_MINOR, 
      GM_BUILD, 
      GM_REVISION, 
      QString::number( GM_BUILD_STRING ).toStdString().c_str() );
}

