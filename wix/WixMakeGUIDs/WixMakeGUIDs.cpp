#include "WixMakeGUIDs.h"

#include <fstream>
extern "C" {
#ifdef WIN32
#include <Lmcons.h>
#include <Rpc.h>
#else
#include <unistd.h>
#include <uuid/uuid.h>
#endif
}

Template::Template(std::string path, std::string templateFile) : m_Path(path) {
  if (path.back() != '/' && path.back() != '\\')
    path += "/";

  std::ifstream fs;

  fs.open((path + templateFile).c_str(), std::fstream::in);
  if (fs.is_open()) {
    fs.seekg(0, fs.end);
    std::size_t sz = fs.tellg();

    if (sz > 0) {
      char *buffer = new char[sz + 1]();

      fs.seekg(0, fs.beg);
      fs.read(buffer, sz);

      buffer[sz] = 0;

      std::size_t prev = 0, cur = 0;
      while (cur < sz) {
        if (!strncmp(buffer + cur, "AUTOGUID", 8)) {
          if (prev < cur) {
            buffer[cur] = 0;
            m_List.push_back(new TemplatePartString(buffer + prev));
          }
          m_List.push_back(new TemplatePartGUID());

          cur += 8;
          prev = cur;
        } else {
          ++cur;
        }
      }
      if (prev < cur) {
        m_List.push_back(new TemplatePartString(buffer + prev));
      }

      delete[] buffer;
    }

    fs.close();
  }
}

void Template::write(std::string outputFile) {
  std::ofstream fs;
  fs.open(outputFile, std::fstream::out | std::fstream::trunc);
  if (fs.is_open()) {
    for (std::list<TemplatePart *>::iterator it = m_List.begin(); it != m_List.end(); ++it) {
      (*it)->write(fs);
    }
    fs.close();
  }
}

std::string Template::getGUID() {
#ifdef WIN32
  UUID uuid;
  UuidCreate(&uuid);

  unsigned char *str;
  UuidToStringA(&uuid, &str);

  std::string s((char *)str);

  RpcStringFreeA(&str);
#else
  uuid_t uuid;
  uuid_generate_random(uuid);
  char s[37];
  uuid_unparse(uuid, s);
#endif
  return s;
}