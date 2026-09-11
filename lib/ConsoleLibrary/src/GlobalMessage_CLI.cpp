#include "GlobalMessage_CLI.h"
#include "GeomecStringTable.h"
#include <fstream>

int GlobalMessage_CLI::msg(const char *message, unsigned int style, unsigned int contextID) {
  outstream() << std::endl << message << std::endl;
  return MB_OK;
}

std::string GlobalMessage_CLI::constructMessage(unsigned int message) {
  QString stringTableEntry = getStringTableEntry(message);
  return stringTableEntry.toStdString();
}

// status

void GlobalMessage_CLI::status(const QString &message) {
  outstream() << std::endl << message.toStdString() << std::endl;
}

// outstream

std::ostream &GlobalMessage_CLI::outstream() {
  if (out_stream.is_open())
    return out_stream;

  return std::cout;
}

bool GlobalMessage_CLI::openlogfile(const std::string &path) {
  if (out_stream.is_open())
    closelogfile();

  out_stream.open(path.c_str(), std::fstream::out);
  return (out_stream.is_open());
}

bool GlobalMessage_CLI::closelogfile() {
  if (!out_stream.is_open())
    return false;

  out_stream.close();
  return (!out_stream.is_open());
}