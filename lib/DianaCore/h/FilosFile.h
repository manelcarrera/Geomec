#pragma once

#include <string>

namespace dia {

namespace ff {
void close();
void open(const char *fname, int mode);
} // namespace ff

namespace util {
char *GetCurrentDir();
}

class CFilosFile {
public:
  CFilosFile(std::string &strFileName, bool bInitFilos);
  ~CFilosFile();

private:
  std::string m_old_FF;
  std::string m_old_FFDIR;
};

} // namespace dia