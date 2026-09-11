#include "System.h"

#include "Printer.h"
#include "util_.h"

#include <QString> 
#include <QProcess>
#include <QFile>
#include <QDir>
//#include "GeomecUtils.h"

const std::string CSystem::ERROR_ = "ERROR";
const std::string CSystem::UNPACK_OK = "Ok";

std::string CSystem::get_unzip(){ return QDir::toNativeSeparators( QString::fromStdString( util::GetAppPath() ) + "/7za.exe").toStdString(); } //FIXME

std::string CSystem::exe( const std::string cmd )
{
  Printer::instance()->info( "system : exe : cmd: %s", cmd.c_str() );

  QProcess process;
  process.start( QString::fromStdString( cmd ) );

  if( process.waitForStarted() )
  {
    process.closeWriteChannel();
    if (process.waitForFinished() )
    {
      QByteArray result = process.readAll();
      return std::string( result.constData(), result.length() );
    }
  }
  return ERROR_;
}

bool CSystem::unpack(const std::string& package, const std::string& dst_dir, const std::string& files)
{
  QString unzip = QString::asprintf("\"%s\" x -y -o\"%s\" \"%s\" %s", get_unzip().c_str(), dst_dir.c_str(), package.c_str(), files.c_str());

  std::string response = exe( unzip.toStdString() );
  bool res = false;
  if( response != ERROR_ )
  {
    if( response.find( UNPACK_OK ) != std::string::npos )
      res = true;
  }
  if( !res )
    Printer::instance()->error( "unzip cmd: %s", unzip.toStdString().c_str() );
  return res;
}
