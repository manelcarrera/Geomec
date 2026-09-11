#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

namespace {

const char HASP_INI_PATH[] = "C:/Program Files (x86)/Common Files/Aladdin Shared/HASP/hasplm.ini";
const char DETACHENABLED_HD[] = "[SERVER]";
const char DETACHENABLED_NM[] = "enabledetach = ";
const char DETACHENABLED_VL[] = "1";
const char SERVERADDR_HD[] = "[REMOTE]";
const char SERVERADDR_LB[] = "serveraddr = ";
const char OLDSERVERADDR_IP[] = "10.127.157.48";
const char NEWSERVERADDR_IP[] =
    "10.127.242.94"; // change to .48 at the end for the new server (verify that that is still the right ip)

inline bool file_exists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

inline bool get_line(std::istream &is, std::string &line) {
  line.clear();

  if (is.eof())
    return false;

  std::istream::sentry guard(is, true);
  std::streambuf *buffer = is.rdbuf();

  while (true) {
    char c = static_cast<char>(buffer->sbumpc());
    switch (c) {
    case '\r':
      if (buffer->sgetc() == '\n')
        buffer->sbumpc();
    case '\n':
      return true;
    case EOF:
      if (line.empty())
        is.setstate(std::ios::eofbit);
      return true;
    default:
      line += c;
    }
  }
}

} // namespace

int main(int argc, char *argv[]) {
  try {
    std::stringstream contents;

    bool fileExists = file_exists(HASP_INI_PATH);
    bool foundDetachable = false;
    bool foundNewAddress = false;

    if (strcmp(OLDSERVERADDR_IP, NEWSERVERADDR_IP) != 0 && fileExists) {
      std::ifstream fi;
      fi.open(HASP_INI_PATH, std::ios::in);

      bool inServerSection = false;
      bool isDetachable = false;
      bool inRemoteSection = false;
      bool hasNewAddress = false;

      if (fi.is_open()) {
        std::string line;

        while (get_line(fi, line)) {
          bool skip = false;
          if (line.size() > 0 && line.at(0) == '[') {
            inServerSection = line.find(DETACHENABLED_HD) != std::string::npos;
            inRemoteSection = line.find(SERVERADDR_HD) != std::string::npos;
          } else if (inServerSection) {
            if (line.find(DETACHENABLED_NM) != std::string::npos) {
              if (line.find(DETACHENABLED_VL) != std::string::npos) {
                foundDetachable = true;
              } else {
                contents << DETACHENABLED_NM << DETACHENABLED_VL << std::endl;
                skip = true;
              }
              isDetachable = true;
            }
          } else if (inRemoteSection) {
            if (line.find(SERVERADDR_LB) != std::string::npos) {
              if (line.find(OLDSERVERADDR_IP) != std::string::npos) {
                contents << SERVERADDR_LB << NEWSERVERADDR_IP << std::endl;
                hasNewAddress = true;
                skip = true;
              } else if (line.find(NEWSERVERADDR_IP) != std::string::npos) {
                foundNewAddress = true;
              }
            }
          }
          if (!skip)
            contents << line << std::endl;
        }
      }

      fi.close();

      if (!isDetachable) {
        if (!inServerSection)
          contents << DETACHENABLED_HD << std::endl;
        contents << DETACHENABLED_NM << DETACHENABLED_VL << std::endl;
      }

      if (!hasNewAddress) {
        if (!inRemoteSection)
          contents << SERVERADDR_HD << std::endl;
        contents << SERVERADDR_LB << NEWSERVERADDR_IP << std::endl;
      }

    } else if (!fileExists) {
      contents << DETACHENABLED_HD << std::endl;
      contents << DETACHENABLED_NM << DETACHENABLED_VL << std::endl;
      contents << std::endl;
      contents << SERVERADDR_HD << std::endl;
      contents << SERVERADDR_LB << NEWSERVERADDR_IP << std::endl;
    }

    if ((!foundDetachable || !foundNewAddress) && contents.str().size() > 0) {
      std::ofstream fo;
      fo.open(HASP_INI_PATH, std::ios::out | std::ios::trunc | std::ios::binary);

      if (fo.is_open()) {
        fo << contents.str();
        fo.close();
      }
    }
  } catch (...) {
  }

  return 0;
}