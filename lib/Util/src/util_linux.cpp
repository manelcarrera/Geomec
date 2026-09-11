//#include "util_linux.h"
#include "util_.h"

#ifndef WIN32

#define _MAX_PATH 260

#include <string>
#include <unistd.h> // getpid

namespace util
{
  bool create_dir(const std::string& dir){		return false; }
  bool dir_exists(const std::string& dirName_in){ return false; }
  bool file_exists(const std::string& file_name){	return false; }
  //std::string path_from_file_name( std::string file_name ){ return std::string("TO BE IMPLEMENTED"); };
  std::string GetLastErrorAsString(){				std::string message; return message; }
  long long GetFileSize(std::string const &path){ return -1; }
  bool number( std::string val ){					return false; }
  //std::string format(const std::string fmt, ...){ return std::string("TO BE IMPLEMENTED"); };

  std::string GetAppPath()
  {	
    char app_path[_MAX_PATH];

    char szTmp[32];

    sprintf(szTmp, "/proc/%d/exe", getpid());

    ssize_t len = _MAX_PATH;
    int bytes = std::min(readlink(szTmp, app_path, len), len - 1);

    if (bytes >= 0)
    {
    app_path[bytes] = '\0';
    }

    std::pair< std::string, std::string > items = split_url( app_path );
    return items.first;
  }

  std::string GetAppFileName()
  {	
    char app_path[_MAX_PATH];

    char szTmp[32];

    sprintf(szTmp, "/proc/%d/exe", getpid());

    ssize_t len = _MAX_PATH;
    int bytes = std::min(readlink(szTmp, app_path, len), len - 1);

    if (bytes >= 0)
    {
    app_path[bytes] = '\0';
    }

    std::string url_s = app_path;
    std::string filename = url_s.substr( url_s.find_last_of("/\\") + 1 );

    return filename;
  }

  bool is_foreground_process(long long pid){ return false; }
}

#endif // ! WIN32
