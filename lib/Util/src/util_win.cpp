#include "util_.h"

#include <fstream>

#ifdef WIN32

#include "Shlobj.h"		// create dir
#include "Shlwapi.h"	// extract directory from filename -> 'Shlwapi.lib'

namespace util
{
	std::string path_from_file_name_win( std::string file_name )
	{
		char path_cp[256];
		strcpy( path_cp, file_name.c_str() );
		PathRemoveFileSpec( path_cp );			// -> 'Shlwapi.lib' / 'Shlwapi.h'

		return std::string( path_cp );
	}

	bool create_dir(const std::string& dir)
	{
		//SHCreateDirectoryEx(NULL, dir.c_str(), NULL);
		return (SHCreateDirectoryEx(NULL, dir.c_str(), NULL) ||	ERROR_ALREADY_EXISTS == GetLastError() );	// -> 'Shell32.lib' / 'Shlobj.h'
	}

	bool dir_exists(const std::string& dirName_in)
	{
	  DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	  if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	  return false;    // this is not a directory!
	}

	bool file_exists(const std::string& file_name)
	{
		std::ifstream file;
		file.open(file_name.c_str());
		if( file ) 
			return true;
		else
			return false;
	}

	std::string GetLastErrorAsString()
	{
		//Get the error message, if any.
		DWORD errorMessageID = ::GetLastError();
		if(errorMessageID == 0)
			return std::string(); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}

	long long GetFileSize(std::string const &path) 
	{ 
		std::wstring path_ws(path.length(),L' ');
		std::copy(path.begin(), path.end(), path_ws.begin());

		WIN32_FIND_DATAW data;
		HANDLE h = FindFirstFileW(path_ws.c_str(), &data);
		if (h == INVALID_HANDLE_VALUE)
			return -1;

		FindClose(h);

		return data.nFileSizeLow | (long long)data.nFileSizeHigh << 32;
	}

	bool number( std::string val )
	{
		char* p;
		std::strtod(val.c_str(), &p);
		return *p == 0;
	}

	std::string GetAppPath()
	{
	  char app_path[_MAX_PATH];
	  GetModuleFileName(NULL, app_path, _MAX_PATH);		// -> 'Kernel32.lib'
	  std::string s2 = path_from_file_name_win( app_path );
	  return s2;
	}

	std::string GetAppFileName()
	{
	  char url[_MAX_PATH];
	  GetModuleFileName(NULL, url, _MAX_PATH);		// -> 'Kernel32.lib'

	  std::string url_s = url;
	  std::string filename = url_s.substr( url_s.find_last_of("/\\") + 1 );

	  return filename;
	}

	bool is_foreground_process(long long pid)
	{
	   HWND hwnd = GetForegroundWindow();
	   if (hwnd == NULL) 
		   return false;

	   DWORD foregroundPid;
	   if (GetWindowThreadProcessId(hwnd, &foregroundPid) == 0) 
		   return false;

	   return (foregroundPid == pid);
	}
} //namespace util


#endif
