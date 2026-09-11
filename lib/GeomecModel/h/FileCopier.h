#ifndef _FileCopier_h
#define _FileCopier_h

#include <qstring.h>

// Copy a file with a progress dialog and cancel button

class CFileCopier {
public:
  CFileCopier(const QString &strSourcePath, const QString &strTargetPath, const QString &strTitle);
  bool Copy();

private:
  QString m_strSourcePath;
  QString m_strTargetPath;
  QString m_strTitle;
};

#endif // _FileCopier_h
