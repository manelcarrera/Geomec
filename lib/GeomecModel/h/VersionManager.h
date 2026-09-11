#ifndef _VersionManager_h_
#define _VersionManager_h_

#include <qstring.h>

#include "StreamVersion.h"

class CVersionManager {
public:
  CVersionManager(const QString &applicationVersion);
  CVersionManager(const CVersionManager &rhs);
  CVersionManager &operator=(const CVersionManager &rhs);

  QString &AppVersion();
  const QString &AppVersion() const;

  void AppVersion(const QString &applicationVersion);

  CStreamVersion &FileVersion();
  const CStreamVersion &FileVersion() const;

  void FileVersion(const CStreamVersion &file_version);

  CStreamVersion CurrentVersion() const;

  bool CheckVersion(const CStreamVersion &version) const;

private:
  CStreamVersion m_file_version;
  QString m_applicationVersion;
};

#endif // _VersionManager_h_
