#if !defined(GEOMEC_UTILS__INCLUDED_)
#define GEOMEC_UTILS__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include <qstring.h>
#include <qwindowdefs.h>
#include <qwindowdefs_win.h>

#ifdef Q_WS_X11
typedef WId HWND;
#endif  // Q_WS_X11


#include "TempPath.h"



void SplitPathAndFileName(const QString& sPathAndFileName,QString& sPath,QString& sFileName);
bool FileExists(QString strPath);
QString GetAppPath();
QString RemoveExtension(const QString &sFilename, bool bCheckForGM = false);
bool RemoveDir(QString strPath);
QString EnsureBackslash(const QString& path);
QString EnsureNoBackslash(const QString& path);
bool DirExists(const QString& strPath );
bool DeleteFilesInDir(const QString& sDir,const QString& sFile);
bool CreateDirectory(QString wsPath);
void ForceGeomecTempPathExt(CTempPath::Category category, const QString& strPath);
QString GetGeomecTempPathExt(CTempPath::Category category);
QString CreateTempDirExt(CTempPath::Category category);
QString ChangeDirectory(const QString &dir);
QString BrowseFolder(HWND hWndOwner, QString sTitle, QString sStartFolder = "");
std::vector<QString> TokenizeString(const QString& str, const QString& delim);

void Glob(const QString& path, const QString& filter, std::vector<QString>& files);

#endif
