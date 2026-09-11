#include "util_.h"

#include <stdarg.h>  // For va_start, etc.
#include <string>

#include <stdio.h> // -> vsnprintf

#include <fstream>
#include <iostream>

namespace util
{

std::pair< std::string, std::string > split_url( const std::string& str )
{
  size_t pos = str.find_last_of( "/\\" );
  bool found = pos != std::string::npos;

  if( found )
	  return std::pair< std::string, std::string >(
		  str.substr( 0, pos ), 
		  str.substr( pos + 1 ) );
  else
	  return std::pair< std::string, std::string >("",""); // ERROR
}
// wrong
// gives filename instead
/*std::string path_from_file_name( const std::string file_name )
{
	std::string filename = file_name;

	const size_t last_slash_idx = filename.find_last_of("\\/");

	if (std::string::npos != last_slash_idx)
		filename.erase(0, last_slash_idx + 1);
	//maybe faster better with substr() ?
	return filename;
}*/

std::string format(const std::string fmt, ...)
{
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

int get_last_line( std::string filename, std::string& last_line ) 
{
    std::string line;

    std::ifstream myfile( filename );
    if(myfile.is_open())
    {
        while( std::getline(myfile, line) ) 
		{
            bool is_empty = true;
            for (int i = 0; i < line.size(); i++) 
			{
                char ch = line[i];
                is_empty = is_empty && isspace(ch);
            }
            if( !is_empty )
                last_line = line;
        }
        myfile.close();
        std::cout << "Last line: " << last_line << std::endl;
    }
    else 
	{
        std::cout << "Unable to open file";
    }   

    return 0;
}

} //namespace util
