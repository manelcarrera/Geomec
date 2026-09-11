/*

Reads version.h and:

1. Adds the version information as Comments to the msi file summary
2. Renames the msi file accordingly
3. Copies result to release directory.



*/
#include <Windows.h>
#include <strsafe.h>
#include "Shlwapi.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "version.h"


void strip_spaces(std::string& str)
{
  str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
}



int main(int argc, char *argv[])
{
  int retval = 0;

  if (argc < 4 || argc > 5)
    retval |= 1 << 0;

  if (argc >  1 && !PathFileExists(argv[1]))
    retval |= 1 << 1;

  if (argc >  2 && !PathFileExists(argv[2]))
    retval |= 1 << 2;

  if (argc >  3 && !PathFileExists(argv[3]))
    retval |= 1 << 3;

  if (retval != 0)
  {
    std::cerr << "Usage: " << argv[0] << " <path to msi file> <path to MsiInfo.exe> <path to release drop folder> [1 if auto-distributing]" << std::endl;
    std::cerr << "   OR: " << argv[0] << " <path to aip file> <path to AdvancedInstaller.com> <path to release drop folder> [1 if auto-distributing]" << std::endl;
    return retval;
  }


  const std::string version = xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX " " xstr(GM_BUILD_STRING);
  std::string version_ = version;
  strip_spaces(version_);

  const bool bDistributing = argc > 4;

  const std::string old_name  = argv[1];
  const std::string extension = old_name.substr(old_name.find_last_of("."));
  const std::string new_name  = "GEOMEC " + version;

  // extracting path from old_name for use in copy command
  std::string src_path;

  int pos = (int)(old_name.length() - 1);
  while (pos >=0 && old_name[pos] != '\\' && old_name[pos] != '/')
    --pos;

  if (pos >= 0)
    src_path = old_name.substr(0, pos + 1);
  std::string src = src_path + new_name;

  std::string dst = argv[3];
#if BUILD_LICENSE_OPTION == -1
  dst.append("\\No License");
#endif


  if (extension.find(".aip") == std::string::npos)
  {

    // no worries about the quotes; it's DOS magic
    std::string msiinfo_cmd = "\"\"" + std::string(argv[2]) + "\" \"" + old_name + "\" /O \"" + version + "\"\"";
    std::string rename_cmd  = "\"ren \"" + old_name + "\" \"" + new_name + extension + "\"\"";
    std::string copy_cmd    = "\"copy \"" + src + extension + "\" \"" + dst + "\"\"";


    if ((retval = system(msiinfo_cmd.c_str())))
    {
      std::cerr << msiinfo_cmd << std::endl;
      std::cerr << argv[2] << " exited with code " << retval << std::endl;
      return retval;
    }

    if ((retval = system(rename_cmd.c_str())))
    {
      std::cerr << rename_cmd << std::endl;
      std::cerr << "File rename exited with code " << retval << std::endl;
      return retval;
    }

    if ((retval = system(copy_cmd.c_str())))
    {
      std::cerr << copy_cmd << std::endl;
      std::cerr << "File copy exited with code " << retval << std::endl;
      return retval;
    }

  }
  else // advanced installer, assume we're on the build server
  {

    src_path = ".\\";

    std::string rename_cmd = "\"copy \"" + old_name + "\" \"" + new_name + extension + "\"\"";
    std::string build_cmd  = "\"\"" + std::string(argv[2]) + "\" /build geomec" + extension + "\"";
    std::string copy_cmd    = "\"copy \"geomec-SetupFiles\\" + new_name + ".msi\" \"" + dst + "\"\"";


    std::ifstream input;
    std::ofstream output;

    std::string cora;
    std::string cora_;

    input.open(old_name, std::ios::in | std::ios::binary);
    if (!input.is_open())
    {
      std::cerr << "Could not open input file '" << old_name << "'" << std::endl;
      return -1;
    }

    output.open("geomec" + extension, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!input.is_open())
    {
      std::cerr << "Could not open output file '" << new_name << extension << "'" << std::endl;
      return -1;
    }

    // simple parser: assume correct syntax
    char c;
    while (input.get(c))
    {
      if (c == '$' && input.peek() == '(')
      {
        input.get(c);

        char name[100];
        size_t i = 0;
        while (input.get(c) && c != ')')
          name[i++] = c;

        name[i] = 0;

        if (!strncmp(name, "version", 7))
        {
          output << (name[7] == '_' ? version_ : version);
        }
        else if (!strncmp(name, "numericVersion", 14))
        {
          output << xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION);
        }
        else if (!strncmp(name, "cora", 4))
        {

          if (cora.empty())
          {

            TCHAR szDir[MAX_PATH];
            WIN32_FIND_DATA ffd;

            StringCchCopy(szDir, MAX_PATH, "..\\src\\third_party\\CORA\\CORA*.msi");
            HANDLE hFind = FindFirstFile(szDir, &ffd);

            if (hFind != INVALID_HANDLE_VALUE) 
            {
              bool notFound = true;
              do
              {
                if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                  if (!strncmp(ffd.cFileName, "CORA", 4))
                  {
                    cora = ffd.cFileName;
                    cora_ = ffd.cFileName;
                    strip_spaces(cora_);
                    notFound = false;
                  }
                }
              }
              while (notFound && FindNextFile(hFind, &ffd) != 0);

              FindClose(hFind);
            }
          }

          output << (name[4] == '_' ? cora_ : cora);
        }
      }
      else
      {
        output << c;
      }
    }

    input.close();
    output.close();

    if ((retval = system(build_cmd.c_str())))
    {
      std::cerr << build_cmd << std::endl;
      std::cerr << argv[2] << " exited with code " << retval << std::endl;
      return retval;
    }

    if ((retval = system(copy_cmd.c_str())))
    {
      std::cerr << copy_cmd << std::endl;
      std::cerr << "File copy exited with code " << retval << std::endl;
      return retval;
    }

  }


  // see if we have a linux file
  std::string linux_src_path, linux_dst_path, linux_version;

  std::ifstream f;
  f.open(src_path + "autobuild.tmp");
  if (f.is_open())
  {
    f >> linux_src_path;
    f.close();
  }

  if (linux_src_path.size())
  {
    linux_src_path = src_path + linux_src_path;
    f.open(linux_src_path + "/VERSION");
    if (f.is_open())
    {
      getline(f, linux_version);
      f.close();
    }

    for (std::size_t i = 0; i < linux_version.size(); ++i)
      if (linux_version[i] == ' ')
        linux_version[i] = '_';

    linux_dst_path = dst + "/" + linux_version;
  }


  if (!linux_version.size()) // no linux files, we're done
    return retval;

  std::string lnx_md_cmd  = "\"mkdir \"" + linux_dst_path + "\"\"";
  std::string lnx_cp0_cmd = "\"copy \"" + linux_src_path + "\\geomec_rgi.tar.bz2\" \"" + linux_dst_path + "\"\"";
  std::string lnx_cp1_cmd = "\"copy \"" + linux_src_path + "\\install\" \"" + linux_dst_path + "\"\"";
  std::string lnx_cp2_cmd = "\"copy \"" + linux_src_path + "\\README.install\" \"" + linux_dst_path + "\"\"";
  std::string lnx_cp3_cmd = "\"copy \"" + linux_src_path + "\\VERSION\" \"" + linux_dst_path + "\"\"";

  if ((retval = system(lnx_md_cmd.c_str())))
  {
    std::cerr << lnx_md_cmd << std::endl;
    std::cerr << "Mkdir exited with code " << retval << std::endl;
    return retval;
  }

  if ((retval = system(lnx_cp0_cmd.c_str())))
  {
    std::cerr << lnx_cp0_cmd << std::endl;
    std::cerr << "File copy exited with code " << retval << std::endl;
    return retval;
  }

  if ((retval = system(lnx_cp1_cmd.c_str())))
  {
    std::cerr << lnx_cp1_cmd << std::endl;
    std::cerr << "File copy exited with code " << retval << std::endl;
    return retval;
  }

  if ((retval = system(lnx_cp2_cmd.c_str())))
  {
    std::cerr << lnx_cp2_cmd << std::endl;
    std::cerr << "File copy exited with code " << retval << std::endl;
    return retval;
  }

  if ((retval = system(lnx_cp3_cmd.c_str())))
  {
    std::cerr << lnx_cp3_cmd << std::endl;
    std::cerr << "File copy exited with code " << retval << std::endl;
    return retval;
  }

  return retval;
}