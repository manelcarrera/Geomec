#include "FilosFile.h"

//own
#include "Printer.h"
//diana
#include "lbfl.h"


namespace{ Printer* printer = Printer::instance(Printer::RunR); }

namespace dia{

namespace ff{

  void close()
  {
    //if(fcisop_() != 0)
    {
      printer->debug("ff <<");
      closfc_();
    }
  }

  void open(const char* fname, int mode)
  {
    printer->debug("ff >>");
    if(fcisop_())
      printer->error("ff already open");
    OpenFilos(fname, mode);
  }

} //namespace ff

namespace util{

  //
  // FIXME: this method shouldn't be needed, just use qt one
  //
  #define PATHLEN 1024
  char *GetCurrentDir()
  {
    char *cwd;
    char *ret;
    int dirsize = PATHLEN;
    cwd = (char *) DiMalloc(dirsize * sizeof (char), "IDianaRunner::CreateSubdir");

#ifdef WIN32
    ret = _getcwd(cwd, dirsize);
#else
    ret = getcwd(cwd, dirsize);
#endif

    while(!ret && errno == ERANGE)
    {
      dirsize *= 2;
      cwd = (char *) DiRealloc(cwd, 0, dirsize * sizeof (char), "IDianaRunner::CreateSubdir");

#ifdef WIN32
      ret = _getcwd(cwd, dirsize);
#else
      ret = getcwd(cwd, dirsize);
#endif
    }

    return ret;
  }
} //util

///////////////////////////////////////////////////////////////////////////
//
//
//			CFilosFile
//
//
///////////////////////////////////////////////////////////////////////////


//
// the original one
//
CFilosFile::CFilosFile(std::string &strFileName, bool bInitFilos)
{
  printer->debug("ff_obj: new >>");

  //remember FF symbols

  // This also crashes: 'memchr'

  //int char_read_length = 256;

  //bool is_it_a_string = memchr( DiGetenv("FF"), 0, char_read_length );
  //if( is_it_a_string )
  char* cp = DiGetenv("FF");
  m_old_FF = cp ? cp : std::string();
  //else
  //	printer->debug("FF: ERROR");

  //is_it_a_string = memchr( DiGetenv("FFDIR"), 0, char_read_length );
  //if( is_it_a_string )
  cp = DiGetenv("FFDIR");
  m_old_FFDIR = cp ? cp : std::string();
  //else
  //	printer->debug("FFDIR: ERROR");

  char* cwd = util::GetCurrentDir();

  char* ffdir_env = vDiStrsave("FFDIR=", cwd, NULL);
  vDiSetenv(ffdir_env);

  char* fname;
  if (strFileName.empty())
  {
    fname = DiStrsave("ffXXXXXX");
    mktemp(fname);
    fname = vDiStrsave(fname, ".ff", 0);
  }
  else
    fname = DiStrsave(strFileName.c_str());

  //printer->info("--FF--: %s", fname);

  char *env = vDiStrsave("FF=", fname, NULL);
  vDiSetenv(env);

  if (bInitFilos)
  {
    ftn_int_t FilosDefs[20];
    gtfdef_(FilosDefs);
    InitFilosFile(fname, FilosDefs, 0);
  }

  //OpenFilos(fname, O_RDWR);
  ff::open(fname, O_RDWR);

  strFileName = fname;

  DiFree(env, "IDianaRunner::CFilosFile::CFilosFile");
  DiFree(fname, "IDianaRunner::CFilosFile::CFilosFile");
  DiFree(ffdir_env, "IDianaRunner::CFilosFile::CFilosFile");

}
CFilosFile::~CFilosFile()
{
  printer->debug("ff_obj: delete <<");

  if (fcisop_() != 0)
    ff::close();
}

} //namespace dia