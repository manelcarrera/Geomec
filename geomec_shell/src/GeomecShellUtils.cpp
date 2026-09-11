#include "GeomecShellUtils.h"

#include <cstring>
#include <fstream>
#include <stdexcept>

#include <QDir>
#include <QString>

namespace gm_shell {

const char *ProgName(const char *argv0) {
  const char *p = strrchr(argv0, '/');
  if (!p)
    p = strrchr(argv0, '\\');

  if (p)
    ++p;

  if (p && *p)
    return p;

  return argv0;
}

void MakeAbsolutePath(QString &strPath) {
  if (QDir::isRelativePath(strPath)) {
    strPath = QDir::current().absoluteFilePath(strPath);
    strPath = QDir::current().cleanPath(strPath);
    strPath = QDir::toNativeSeparators(strPath);
  }
}

void copyFile(const QString &destination, const QString &source) {
  if (source == destination) {
    return;
  }

  std::ifstream input(source.toStdString().c_str(), std::ios::in | std::ios::binary);
  std::ofstream output(destination.toStdString().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

  if (input.fail()) {
    throw std::runtime_error((source + " could not be opened for reading").toStdString().c_str());
  }

  if (output.fail()) {
    throw std::runtime_error((destination + " could not be opened for writing").toStdString().c_str());
  }

  output << input.rdbuf();

  if (output.fail()) {
    throw std::runtime_error((destination + " could not be written to").toStdString().c_str());
  }

  // there *may* be an issue on windows with automatic flush/close on destruction of ofstream object,
  // so we do it explicitly. It can't hurt. (Bug 83505.)
  output.flush();
  output.close();
}

void moveFile(const QString &destination, const QString &source) {
  if (source == destination) {
    return;
  }

  copyFile(destination, source);

  if (remove(source.toStdString().c_str()) != 0) {
    throw std::runtime_error((source + " could not be removed").toStdString().c_str());
  }
}

} // namespace gm_shell