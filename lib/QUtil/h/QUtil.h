#pragma once

#include <string>
#include <vector>
#include <QString>

namespace std{
  class thread;
}

class Printer;

namespace util{
  namespace concurrency{
    namespace threads{
      void quit( std::thread*& m_thread_run, Printer* p_ = nullptr );
    } // namespace threads
  } // namespace concurrency
} //namespace util


//
// FIXME do them functions instead
//
class QUtil
{
public:

  typedef std::vector<std::string> TFoldersV;
  typedef std::vector<std::string> TFilesV;

  enum eFile{ Name, FullPath };
  enum eUsrDir{ Base,	Goemec,	Plugins, /*Changelog,*/ Temp };
  enum eType{ File, Dir };

public:

  QUtil(){}

  static TFoldersV folders( std::string path, eFile type_ );
  static TFilesV files( std::string path, std::string filter, eFile type_ );	// file full path
  static bool exists( std::string path, eType type_ );

  static std::string GetAppPath(); // FIXME: doesn't cover all the cases

  static std::string url( std::string path, std::string filename );
  static std::string url( eUsrDir type_, std::string filename );
  static QString url( QString path, QString filename );
  static std::string usr_dir( eUsrDir );
  static std::string native_sepatators( std::string s );
  static bool copy( std::string orig, std::string dest );

  static void rename( std::string file_name, std::string to_replace, std::string replace_by );	// change file extension

  static bool remove_dir( QString strPath );
  static std::string create_random_dir();

  static void kill_process( std::string process );

  static void token_to_file( int pid, const std::string& file_ );
  static int token_from_file( const std::string& file_ );

};

