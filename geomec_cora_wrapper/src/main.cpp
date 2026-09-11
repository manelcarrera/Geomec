#include <cstdlib> // system
#include <string>
#include <iostream> // std::cout
#include <algorithm> // remove

#include <fstream> // write_to_file

namespace {
  void log( std::string data ) 
  {
    static const std::string FILE = std::string( getenv("APPDATA") ) + "/Geomec/gm_cora_wrapper.log";

    std::ofstream myfile;
    myfile.open( FILE, std::ofstream::out | std::ofstream::app );

    myfile << data << std::endl;

    myfile.close();
  }
}

int main(int argc, char *argv[])
{
#ifdef WIN32
  std::string command = "geomec_shell.exe";
#else
  std::string command = "geomec_shell";
#endif

  for (int i = 1; i < argc; ++i)
  {
    std::string p = argv[ i ];

    bool b_blank = p.find(' ') != std::string::npos;
    if(b_blank)
    {
      char& c = p.front();
      if( p.front() != '"') // weak control but enough
        p = "\"" + p + "\"";
    }

    command += ( " " + p );
  }

  //log( "command: " + command );

  //
  // a)
  //

  return system( command.c_str() );

  //
  // b)
  //

/*#ifdef _WIN32
  FILE* pipe = _popen( command.c_str(), "r");
#else
  FILE* pipe = popen(exec_string, "r");
#endif

  char buffer[256];
  while (!feof(pipe))
  {
    if (fgets(buffer, 256, pipe))
      std::cout << buffer;
  }
#ifdef _WIN32
  _pclose(pipe);
#else
  pclose(pipe);
#endif*/
}
