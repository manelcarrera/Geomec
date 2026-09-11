#include "QUtil.h"

#include "Printer.h"
// qt
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QTextStream>
// std
#include <thread>

namespace util {
namespace concurrency {
namespace threads {
//
//
//
void quit(std::thread *&th, Printer *p_) {
  Printer *p = p_ ? p_ : Printer::instance();

  if (!th) {
    p->debug("thread : quit : No need");
    return;
  }

  p->debug("thread : quit >>");
  if (th) {
    if (th->joinable())
      th->join();
    delete th;
    th = nullptr;
  }
  p->debug("<< thread : quit");
}
} // namespace threads
} // namespace concurrency
} // namespace util

std::string QUtil::create_random_dir() {
  std::string path = QUtil::usr_dir(QUtil::eUsrDir::Temp) + "\\" + std::to_string(std::rand());
  bool res = QDir().mkpath(QString::fromStdString(path));

  return res ? path : "ERROR";
}

bool QUtil::remove_dir(QString strPath) {
  Printer::instance()->info("remove dir : '%s'", strPath.toStdString().c_str());

  // QString strPath = QString::fromStdString( path );

  // assert(!strPath.isEmpty());
  if (strPath.isEmpty())
    return false; // never remove from the root dir !!

  bool result = true;
  QDir dir(strPath);

  if (dir.exists(strPath)) {
    Q_FOREACH (QFileInfo info,
               dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files,
                                 QDir::DirsFirst)) {
      if (info.isDir()) {
        result = remove_dir(info.absoluteFilePath());
      } else {
        result = QFile::remove(info.absoluteFilePath());
      }

      if (!result) {
        return result;
      }
    }

    result = dir.rmdir(strPath);
  }

  return result;
}

// FIXME: doesn't cover all the cases
// Just for Windows
// Better don't use this as it is
std::string QUtil::GetAppPath() {
  const std::string PROGRAMFILES_PATH = getenv("programfiles");

  QUtil::TFoldersV folders_v = QUtil::folders(PROGRAMFILES_PATH, QUtil::FullPath);
  size_t nE = folders_v.size();
  for (size_t i = 0; i < nE; i++) {
    std::string f = folders_v[i];
    for (auto &c : f)
      c = toupper(c);
    if (f.find("GEOMEC") != std::string::npos)
      return f;
  }
  return "ERROR";
}

std::string QUtil::url(eUsrDir type_, std::string filename) {
  return QDir(QString::fromStdString(usr_dir(type_))).absoluteFilePath(QString::fromStdString(filename)).toStdString();
}

std::string QUtil::url(std::string path, std::string filename) {
  return QDir::toNativeSeparators(QString::fromStdString(path + "/" + filename)).toStdString();
}

QString QUtil::url(QString path, QString filename) { return QDir::toNativeSeparators(path + "/" + filename); }

std::string QUtil::usr_dir(eUsrDir type_) {
  QString dir;
#if _WIN32
  dir = getenv("APPDATA");
#else
  dir = getenv("HOME");
#endif

  switch (type_) {
  default:
  case Base: // dir += "/";
    break;
  case Goemec:
    dir += "/Geomec";
    break;
  case Plugins:
    dir += "/Geomec/plugins";
    break;
  // case Changelog:	dir += "/Geomec/changelog";	break;
  case Temp:
    dir += "/Geomec/temp";
    break;
  }
  return QDir::toNativeSeparators(dir).toStdString();
}

std::string QUtil::native_sepatators(std::string s) {
  return QDir::toNativeSeparators(QString::fromStdString(s)).toStdString();
}

bool QUtil::copy(std::string orig, std::string dest) {
  QString orig_qs = QDir::toNativeSeparators(QString::fromStdString(orig));
  QString dest_qs = QDir::toNativeSeparators(QString::fromStdString(dest));

  std::string orig_s = orig_qs.toStdString();
  std::string dest_s = dest_qs.toStdString();

  bool res_c = QFile::copy(orig_qs, dest_qs);

  if (!res_c)
    Printer::instance()->error("copy : '%s' -> '%s'", orig.c_str(), dest.c_str());

  return res_c;
}

QUtil::TFoldersV QUtil::folders(std::string path, eFile type_) {
  TFoldersV folders_v;
  QDirIterator it(QString::fromStdString(path), QDir::Dirs | QDir::NoDotAndDotDot);
  while (it.hasNext()) {
    QDir d = QDir(it.next());
    QString d_qs = type_ == Name ? d.dirName() : d.absolutePath();
    folders_v.push_back(d_qs.toStdString());
  }
  return folders_v;
}

QUtil::TFilesV QUtil::files(std::string path, std::string filter, eFile type_) // filter ex.: "*.xml"
{
  TFilesV files_v;
  QDirIterator it(QString::fromStdString(path), QStringList(QString::fromStdString(filter)), QDir::Files);
  while (it.hasNext()) {
    it.next();
    files_v.push_back(type_ == Name ? it.fileName().toStdString() : it.filePath().toStdString());
  }
  return files_v;
}

void QUtil::rename(std::string name, std::string to_replace, std::string replace_by) {
  QString new_extension = QString::fromStdString(replace_by);
  QString new_name = QString::fromStdString(name).replace(QString::fromStdString(to_replace), new_extension);
  QFile::rename(QString::fromStdString(name), new_name);
}

bool QUtil::exists(std::string path, eType type_) {
  QFileInfo info(QString::fromStdString(native_sepatators(path)));
  return (info.exists() && (type_ == File ? info.isFile() : info.isDir()));
}

void QUtil::kill_process(std::string process_) {
  Printer::instance()->debug("kill processe : '%s'", process_.c_str());

#ifdef WIN32
  const std::string DEFAULT_FOLDER = std::string(getenv("APPDATA"));
#else
  const std::string DEFAULT_FOLDER = std::string("~");
#endif

  /*	std::string cmd =  "( \
  echo. && \
  date /T && time /T && \
  taskkill /F /IM diana_app.exe && \
  taskkill /F /IM geomec_launcher.exe && \
  (tasklist | find \"diana_app.exe\") && \
  (tasklist | find \"geomec_launcher.exe\") ) \
  >> " + DEFAULT_FOLDER + "\\kill_processes.txt 2>&1";*/

  /*std::string cmd =  "( \
echo. && \
date /T && time /T && \
taskkill /F /IM diana_app.exe && \
(tasklist | find \"diana_app.exe\") && \
>> " + DEFAULT_FOLDER + "\\kill_processes.txt 2>&1";*/

  QString cmd_s = QString::asprintf("( \
echo. && \
date /T && time /T && \
taskkill /F /IM %s && \
(tasklist | find \"%s\") && \
>> %s\\kill_processes.txt 2>&1",
                                    process_.c_str(), process_.c_str(), DEFAULT_FOLDER.c_str());

  std::string cmd_p = "cmd /c " + cmd_s.toStdString();

  QProcess process;
  process.start(QString::fromStdString(cmd_p));

  bool res = process.waitForFinished(-1); // -1: no timeout, 30s. by default
}

void QUtil::token_to_file(int pid, const std::string &file_) {
  QString filename = QString::fromStdString(Printer::instance()->folder() + "/" + file_);
  QFile file(filename);
  if (file.open(QIODevice::ReadWrite)) {
    QTextStream stream(&file);
    stream << pid << endl;
  }
  Printer::instance()->info("token:%d -> file:%s", pid, file.fileName().toStdString().c_str());
}

int QUtil::token_from_file(const std::string &file_) {
  int pid = 0;

  QString filename = QString::fromStdString(Printer::instance()->folder() + "/" + file_);
  QFile file(filename);

  // Printer::instance()->info( "CDianaStartUp > RunDiana > run_diana_task > pid [file:%s]",
  // file.fileName().toStdString().c_str() );

  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);
    QString line = in.readLine();
    file.close();

    pid = line.toInt();

    // Printer::instance()->info( "CDianaStartUp > RunDiana > run_diana_task > pid [file:%s pid=%d]",
    // file.fileName().toStdString().c_str(), pid );
  } else {
    // Printer::instance()->info( "CDianaStartUp > RunDiana > run_diana_task > pid [file:%s NOT EXIST]",
    // file.fileName().toStdString().c_str() );
  }
  return pid;
}
