#ifndef _ARGUMENT_PARSER_H_
#define _ARGUMENT_PARSER_H_

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace gm_shell {

class CArgumentParser {
public:
  typedef std::pair<std::string, std::string> TKeyValuePair;

  CArgumentParser(int argc, char *argv[]);

  void GetCommand(std::string &command);
  void GetInterface(std::string &iFace);
  void GetInterface(std::string &iFace, int &major, int &minor);
  void GetArgument(size_t index, std::string &key, std::string &value);
  size_t ArgumentSize();

  // deprecated
  char **_argv(bool upperCase = false);

private:
  std::string m_command;

  std::string m_interface;
  int m_interfaceVersionMajor;
  int m_interfaceVersionMinor;

  std::vector<TKeyValuePair> m_arguments;
};

} // namespace gm_shell

#endif