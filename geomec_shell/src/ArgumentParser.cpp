#include <cstdlib>
#include <cstring>

#include "ArgumentParser.h"
#include "GeomecUtils.h"


#if !defined(WIN32) && !defined(WIN64) && !defined(_WINDOWS)
#define _strdup strdup
#endif  // !defined(WIN32) && !defined(WIN64)


namespace gm_shell
{

CArgumentParser::CArgumentParser(int argc, char *argv[]) : m_interfaceVersionMajor(0), m_interfaceVersionMinor(0)
{
  std::vector<std::string> args(argv + 1, argv + argc);

  m_command   = argv[0];
  m_interface = "auto";

  for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
  {
  std::string arg = *it;

  bool isKey = false;

  size_t keyStart = 1;

  if (arg.at(0) == '/')
  {
      // quick hack to keep paths on linux
      QString path, file;
      SplitPathAndFileName(arg.c_str(), path, file);

      isKey = !FileExists(path);
  }
  else if (arg.at(0) == '-')
  {
      isKey = true;
      if (arg.size() > 1 && arg.at(1) == '-')
    ++keyStart;
  }

  if (isKey)
  {
      size_t keyEnd = keyStart;
      for (size_t i = keyStart; i < arg.size(); ++i)
      {
    if (arg.at(i) == '=' || arg.at(i) == ':')
          break;
    arg.at(i) = tolower(arg.at(i));
    ++keyEnd;
      }

      std::string key = arg.substr(keyStart, keyEnd - 1);

      if (!key.compare("interface"))
      {
    ++it;
    if (it != args.end())
    {
          m_interface = *it;

          for (size_t i = 0; i < m_interface.size(); ++i)
      m_interface.at(i) = tolower(m_interface.at(i));

          if (it + 1 != args.end()) // check for optional #.# version number
          {
      char c = (it + 1)->at(0);
      if (c >= '0' && c <= '9')
      {
              ++it;
              arg = *it;

              m_interfaceVersionMajor = c - '0';

              int *version = &m_interfaceVersionMajor;

              for (size_t i = 1; i < arg.size(); ++i)
              {
        bool parseError = false;

        c = arg.at(i);

        if (c == '.')
        {
                  if (version == &m_interfaceVersionMinor)
          parseError = true; // second time we encounter a period

                  version = &m_interfaceVersionMinor;
                  if (i >= arg.size() - 1)
          parseError = true; // period at end
        }
        else if (c >= '0' && c <= '9')
        {
                  *version = *version * 10 + c - '0';
        }
        else
                  parseError = true; // not a valid character

        if (parseError) // assume this is just the next argument, and parse it below
        {
                  m_interfaceVersionMajor = 0;
                  m_interfaceVersionMinor = 0;
                  --it;
                  break;
        }
              }
      }
          }
    }
      }
      else
      {
    std::string value;
    
    if (keyEnd < arg.size() - 1)
          value = arg.substr(keyEnd + 1);

    m_arguments.push_back(std::make_pair(key, value));
      }

  }
  else
  {
      m_arguments.push_back(std::make_pair("", arg));
  }
  }
}


void CArgumentParser::GetCommand(std::string& command)
{
  command = m_command;
}

void CArgumentParser::GetInterface(std::string& iFace)
{
  iFace = m_interface;
}

void CArgumentParser::GetInterface(std::string& iFace, int& major, int& minor)
{
  iFace = m_interface;
  major = m_interfaceVersionMajor;
  minor = m_interfaceVersionMinor;
}

void CArgumentParser::GetArgument(size_t index, std::string& key, std::string& value)
{
  if (index < m_arguments.size())
  {
  key   = m_arguments[index].first;
  value = m_arguments[index].second;
  }
}

size_t CArgumentParser::ArgumentSize()
{
  return m_arguments.size();
}


char **CArgumentParser::_argv(bool upperCase)
{
  char **a = (char **)calloc(sizeof(char *), m_arguments.size() + 1);

  a[0] = _strdup(m_command.c_str());

  for (size_t i = 0; i < m_arguments.size(); ++i)
  {
  std::string key   = m_arguments[i].first;
  std::string value = m_arguments[i].second;

  if (key.size() > 0 && upperCase)
  {
      for (size_t j = 0; j < key.size(); ++j)
    key.at(j) = toupper(key.at(j));
  }

  size_t total = key.size() + value.size() + 1;
  if (key.size() > 0)
  {
      ++total;
      if (value.size() > 0)
    ++total;
  }

  char *s = (char *)malloc(sizeof(char) * total);

  int k = 0;

  if (key.size())
  {
      s[k++] = '/';
      for (size_t j = 0; j < key.size(); ++j)
    s[k++] = key.at(j);
  }
  if (value.size())
  {
      if (k)
    s[k++] = ':';
      for (size_t j = 0; j < value.size(); ++j)
    s[k++] = value.at(j);
  }
  s[k++] = 0;

  a[1 + i] = s;
  }

  return a;

}


}
