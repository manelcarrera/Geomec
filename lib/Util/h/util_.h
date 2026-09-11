#pragma once

#include <string>

namespace util
{

bool dir_exists(const std::string& dirName_in);
bool create_dir(const std::string& dir);

bool file_exists(const std::string& file_name);

std::pair< std::string, std::string > split_url( const std::string& str );
//std::string path_from_file_name( std::string file_name );

std::string GetLastErrorAsString();

long long GetFileSize(std::string const &path);

bool number( std::string val );

std::string format(const std::string fmt, ...);

std::string GetAppPath();
std::string GetAppFileName();

int get_last_line( std::string filename, std::string& last_line );

bool is_foreground_process(long long pid);

}