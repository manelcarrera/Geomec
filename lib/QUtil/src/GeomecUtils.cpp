
#ifdef WIN32
#include <Windows.h>
#endif  // WIN32

#include <cassert>
#include <algorithm>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h> // getpid
#endif

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif  // WIN32

#include <QDir>

#include "GeomecUtils.h"
#include "IPlatform.h"
#include "Environment.h"
//#include "lbcx.h"

#include "Printer.h"

#include <QString>

/*
For Linux the implementation is not modified:

http://linux.die.net/man/2/stat64

Underlying kernel interface 

Over time, increases in the size of the stat structure have led to three
successive versions of stat(): sys_stat() (slot __NR_oldstat), sys_newstat()
(slot __NR_stat), and sys_stat64() (new in kernel 2.4; slot __NR_stat64).
The glibc stat() wrapper function hides these details from applications,
invoking the most recent version of the system call provided by the kernel,
and repacking the returned information if required for old binaries.
Similar remarks apply for fstat() and lstat().
*/

#ifndef WIN32
#define _stat stat
#define _getcwd getcwd
#define _chdir chdir
#define _mktemp mktemp
#endif  // WIN32

#ifndef _S_IFDIR
#define _S_IFDIR S_IFDIR
#endif  // _S_IFDIR

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif  // _MAX_PATH

static QString g_temp_path[CTempPath::_TEMP_NR_OF_CATEGORIES];

void SplitPathAndFileName(const QString& sPathAndFileName,QString& sPath,QString& sFileName)
{
	char BSLASH='\\'; 
  char SLASH = '/';
	int pos=0; 

	// Check for trailing slash: 
	pos = sPathAndFileName.lastIndexOf(BSLASH);
  if(pos < 0)
    pos = sPathAndFileName.lastIndexOf(SLASH);

	if(pos<0)
  {
    // no path, just a filename
		sFileName = sPathAndFileName;
    sPath = "";
  }
  else
  {
    sPath=sPathAndFileName.left(pos);
    sFileName=sPathAndFileName.right(sPathAndFileName.length() - pos - 1);
  }
}

bool FileExists(QString strPath)
{
#ifdef _WIN32
  struct _stat64 buffer;

  if (_stat64(strPath.toStdString().c_str(), &buffer) != 0)
#else
  struct _stat buffer;

  if (_stat(strPath.toStdString().c_str(), &buffer) != 0)
#endif
  {
    return false;
  }

  return true;
}

// '\\' or '/' not included, before it was
QString GetAppPath()
{
  char app_path[_MAX_PATH];

#ifdef WIN32
  GetModuleFileName(NULL, app_path, _MAX_PATH);
#else // !WIN32
  char szTmp[32];

  sprintf(szTmp, "/proc/%d/exe", getpid());

  ssize_t len = _MAX_PATH;
  int bytes = std::min(readlink(szTmp, app_path, len), len - 1);

  if (bytes >= 0)
  {
    app_path[bytes] = '\0';
  }
#endif  // WIN32

  QString app_str = app_path;
  // app_str: C:\Program Files\GEOMEC 5.7 - SVS\Geomec.exe

 #ifdef WIN32
  app_str = app_str.left(app_str.lastIndexOf('\\') );
#else
  app_str = app_str.left(app_str.lastIndexOf('/') );
#endif

  return app_str;
}

/*
CString EnsureBackslash(const CString& path)
{
	if(path.Right(1) != "\\")
		return path + "\\";
	else
		return path;
}


CString EnsureNoBackslash(const CString& path)
{
	if(path.Right(1) == "\\")
		return path.Left(path.GetLength()-1);
	else
		return path;
}
*/

QString RemoveExtension(const QString &sFilename, bool bCheckForGM)
{
	int pos = sFilename.lastIndexOf('.'); 

	if(pos < 0)
		return sFilename; // has no extension

  if (bCheckForGM)
  {
    int len = sFilename.length();
    if (sFilename.right(len - pos) == ".gm4" || sFilename.right(len - pos) == ".gm5")
      return sFilename.left(pos);
    else
      return sFilename;
  }
	return sFilename.left(pos);

}


QString ChangeDirectory(const QString &dir)
{
	char curdir[_MAX_PATH];
	_getcwd(curdir, _MAX_PATH);
	QString strCurDir = curdir;

	_chdir(dir.toStdString().c_str());
	return strCurDir;
}

void ForceGeomecTempPathExt(CTempPath::Category category, const QString& strPath)
{
  g_temp_path[category] = strPath;
}

//
// private funcion
//
QString GetGeomecTempPathExt_GLOBAL_VAR(CTempPath::Category category)
{
  // forced temp path?
  if(	!g_temp_path[category].isEmpty() && 
		DirExists(g_temp_path[category]))
  {
    return g_temp_path[category];
  }

  if(	category == CTempPath::TEMP_CALCULATION && 
		!g_temp_path[CTempPath::TEMP_GENERAL].isEmpty() && 
		DirExists(g_temp_path[CTempPath::TEMP_GENERAL]))
  {
    return g_temp_path[CTempPath::TEMP_GENERAL];
  }
  return QString();
}

//
// A) OLD implementation
//
// Try registry
// Then try ENVVAR
// In ENVVAR then ENVVAR
// Otrherwise registry
//
#ifdef KK
QString GetGeomecTempPathExt_ENV_THEN_REG( CTempPath::Category category )
{
	CTempPath tempPath;
  tempPath.ReadConfig();

  CEnvironment* env = CEnvironment::instance();

	QString strTempDir = tempPath.Path(category);

   // try some alternatives
  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = DiGetenv("TEMP");

  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = DiGetenv("TMP");

  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = DiGetenv("HOME");

#ifdef WIN32
  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = QString(DiGetenv("HOMEDRIVE")) + DiGetenv("HOMEPATH");
#else
  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = "/tmp";
#endif

  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = QString();

	return strTempDir;
}
#endif

//
// No more windows registry with a config file: 'geomec.ini'
//
QString GetGeomecTempPathExt_INTERMEDIATE( CTempPath::Category category )
{
	  // 1) Regsitry -> Now config file
  CTempPath tempPath;
  tempPath.ReadConfig(); // destructor: writes to the file values read from the file

	QString strTempDir = tempPath.Path(category);

  // 2) Priority : 1. ENVVAR | 2. Config file | 3. Dafault
  //
  // With this, the step before is not needed

  CEnvironment* env = CEnvironment::instance();

   // try some alternatives
  if(!DirExists(strTempDir.toStdString().c_str()))
	strTempDir = env->get("TEMP");

  if(!DirExists(strTempDir.toStdString().c_str()))
	strTempDir = env->get("TMP");

  if(!DirExists(strTempDir.toStdString().c_str()))
	strTempDir = env->get("HOME");

#ifdef WIN32
  if(!DirExists(strTempDir.toStdString().c_str()))
	strTempDir = env->get("HOMEDRIVE") + env->get("HOMEPATH");
#else
  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = "/tmp";
#endif

  if(!DirExists(strTempDir.toStdString().c_str()))
    strTempDir = QString();

	return strTempDir;
}

#ifdef KK
//
// Order:
// 1) global var
// 2) CTempPath obj 
// 3) CEnvironment obj
//
QString GetGeomecTempPathExt(CTempPath::Category category)
{
	QString dir = GetGeomecTempPathExt_GLOBAL_VAR( category );
	if( !dir.isEmpty() )
		return dir;
  
  //
  // Continue if GEN and CALC both not in global var: 'g_temp_path'
  //

  //
  // mcr 2020-02-26
  // this currently does not was it was intended for
  // just sets both paths but from the config file that is empty
  // 

  //
  // A) OLD implementation
  //
  // Try registry
  // Then try ENVVAR
  // In ENVVAR then ENVVAR
  // Otrherwise registry
  //
  // return GetGeomecTempPathExt_ENV_THEN_REG( category );
  //

  //
  // B) Intermediate implementation (START)
  //
  // return GetGeomecTempPathExt_INTERMEDIATE( category );
  // 

  //
  // C) New implementation
  // mcr 2020-02-26
  // Environment looks at ENVAR, file and proiritizes... and if no value it takes a default one (nothing defined for TMP)

	//
	// vars:
	// if user defined:
	// -'UserTempPath'
	// -'CalcTempPath'
	// else
	// -'TEMP' ENVVAR
	//
	CTempPath path;
	path.ReadConfig(); // destructor: writes to the file values read from the file
	dir = path.Path( category );
	if( DirExists( dir ) )
		return dir;

	CEnvironment* env = CEnvironment::instance();
	for( const auto& key : {"TEMP","TMP","TMPDIR",	// TMPDIR sometimes defined in Linux but not always
							"APPDATA","HOME"} )		// These two are always derfined, so in the worse case it will take these ones 
													// -'APPDATA': win 
													// -'HOME': Linux
	{
		dir = env->get( key );
		if( DirExists( dir ) )
			return dir;
	}
#ifndef WIN32
    return "/tmp";
#endif

	return QString();
}
#endif

QString GetGeomecTempPathExt(CTempPath::Category category)
{
	enum eOrigin{ Global, TempPath, Environment, NotFound } origin;
	char* origin_cp[]={ "Global", "TempPath", "Environment", "NotFound" };

	//
	//
	//
	origin=Global;
	QString dir = GetGeomecTempPathExt_GLOBAL_VAR( category );
	if( dir.isEmpty() || !DirExists( dir ) )
	{
		//
		//
		//
		origin=TempPath;
		CTempPath path;
		path.ReadConfig(); // destructor: writes to the file values read from the file
		dir = path.Path( category );
		if( dir.isEmpty() || !DirExists( dir ) )
		{
			//
			//
			//
			origin=Environment;
			CEnvironment* env = CEnvironment::instance();
			for( const auto& key : {"TEMP","TMP","TMPDIR",	// TMPDIR sometimes defined in Linux but not always
									"APPDATA","HOME"} )		// These two are always derfined, so in the worse case it will take these ones 
															// -'APPDATA': win 
															// -'HOME': Linux
			{
				dir = env->get( key );
				if( !dir.isEmpty() && DirExists( dir ) )
					break;
			}
			if( dir.isEmpty() || !DirExists( dir ) )
			{
				origin=NotFound;
#ifndef WIN32
				dir = "/tmp";
#else
				dir = QString();
#endif
			}
		}
	}
	Printer::instance(Printer::Gen)->debug( "get_gm_tmp_path_ext : %s -> %s", origin_cp[origin], dir.toStdString().c_str());
	return dir;
}


QString CreateTempDirExt(CTempPath::Category category)
{
	QString strDirName;

	char lpszTempPath[_MAX_PATH];
	QString strTempDir = GetGeomecTempPathExt(category);
#ifdef _WIN32
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  QString strTempPath = 
	  strTempDir + 
	  QString("\\geomec%1XXXXXX").arg((long long)ft.dwLowDateTime + 
	  ((long long)ft.dwHighDateTime << 32LL));
#else
	QString strTempPath = strTempDir + "/geomecXXXXXX";
#endif
	strcpy(lpszTempPath, strTempPath.toStdString().c_str());
	_mktemp(lpszTempPath);
#ifdef WIN32
	_mkdir(lpszTempPath);
#else // !WIN32
	mkdir(lpszTempPath, S_IFDIR | S_IRWXU);
#endif  // WIN32

	IPlatform* platform = IPlatform::instance();
	QString traceString =
		QString("Created temp directory %1\n").arg(lpszTempPath);

	platform->trace(traceString);
	strDirName = lpszTempPath;

	return strDirName;
}



// RemoveDir removes a directory and all its contents, including subdirectories
// It behaves like a rm -rf in unix
// Return value is true if everything is deleted
// Return value is false if one or more files/directories could not be deleted
bool RemoveDir(QString strPath)
{
	assert(!strPath.isEmpty());
	if(strPath.isEmpty()) return false; // never remove from the root dir !!

  bool result = true;
  QDir dir(strPath);

  if (dir.exists(strPath))
  {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
      QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files,
      QDir::DirsFirst))
    {
      if (info.isDir())
      {
        result = RemoveDir(info.absoluteFilePath());
      }
      else
      {
        result = QFile::remove(info.absoluteFilePath());
      }

      if (!result)
      {
        return result;
      }
    }

    result = dir.rmdir(strPath);
  }

  return result;
}

bool DirExists(const QString& strPath)
{
#ifdef _WIN32
  struct _stat64 buffer;
  int iRetTemp = 0;

  memset((void*) &buffer, 0, sizeof(buffer));

  iRetTemp = _stat64(strPath.toStdString().c_str(), &buffer);
#else
  struct _stat buffer;
  int iRetTemp = 0;

  memset((void*) &buffer, 0, sizeof(buffer));

  iRetTemp = _stat(strPath.toStdString().c_str(), &buffer);
#endif

  if (iRetTemp == 0)
  {
    if (buffer.st_mode & _S_IFDIR)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}


void Glob(const QString& path, const std::vector<QRegExp>& filter, std::vector<QString>& files)
{
  QDir dir(path);

  if (dir.exists())
  {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
      if (info.isDir())
        Glob(info.absoluteFilePath(), filter, files);
      else
      {
        for (std::vector<QRegExp>::const_iterator it = filter.begin(); it != filter.end(); ++it)
        {
          if (it->indexIn(info.absoluteFilePath()) >= 0)
          {
            files.push_back(info.absoluteFilePath());
            break;
          }
        }
      }
    }
  }
}

void Glob(const QString& path, const QString& filter, std::vector<QString>& files)
{
  QStringList l = filter.split('|', QString::SkipEmptyParts);
  std::vector<QRegExp> reList;
  reList.reserve(l.length());

  for (QStringList::const_iterator it = l.begin(); it != l.end(); ++it)
    reList.push_back(QRegExp(*it));

  Glob(path, reList, files);
}

/*
//recursivly buld up a directory tree
bool CreateDirectory(QString wsPath)
{ 

	char SLASH='\\'; 
	DWORD attr; 
	int pos; 
	bool result = true; 

	// Check for trailing slash: 
	pos = wsPath.lastIndexOf(SLASH); 

	if (wsPath.length() == pos + 1) // last character is "\" 
	{ 
		wsPath = wsPath.left(pos); 
	} 

	// Look for existing object: 
	attr = GetFileAttributes(wsPath.toStdString().c_str()); 

	if (0xFFFFFFFF == attr) // doesn't exist yet - create it! 
	{	
		pos = wsPath.lastIndexOf(SLASH); 
		if (0 < pos) 
		{ 
			// Create parent dirs: 
			result = CreateDirectory(wsPath.left(pos)); 
		} 
		// Create node: 
		result = result && ::CreateDirectory(wsPath.toStdString().c_str(), NULL); 
	} 
	else if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
	{ // object already exists, but is not a dir 
		SetLastError(ERROR_FILE_EXISTS); 
		result = false; 
	} 
	return result; 
} 


bool DeleteFilesInDir(const QString& sDir,const QString& sFile)
{
	CString sFileName=(EnsureBackslash(sDir) + sFile).toStdString().c_str();

	CFileFind ff;

	bool bWorking=ff.FindFile(sFileName);
	std::vector<CString> vec;

	while(bWorking)
	{
		bWorking=ff.FindNextFile();
		CString sFile=(EnsureBackslash(sDir) + ff.GetFileName()).toStdString().c_str();
		vec.push_back(sFile);
		//DeleteFile(sFile);
	}

	for(size_t i=0;i<vec.size();i++)
		DeleteFile(vec[i]);

	return true;
}

//taken from KB article Q179378
static int CALLBACK BrowseCallbackProc(HWND hwnd,unsigned int uMsg,LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH];
	switch(uMsg)
	{
		case BFFM_INITIALIZED:
			{
				if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
				{
					// WParam is true since you are passing a path.
					// It would be false if you were passing a pidl.
					SendMessage(hwnd,BFFM_SETSELECTION,true,(LPARAM)szDir);
				}
				break;
            }
		case BFFM_SELCHANGED:
			{
				// Set the status window to the currently selected path.
				if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
				{
					SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
				}
				break;
            }
		default:
			break;
	}
	return 0;
}

QString BrowseFolder(HWND hWndOwner, QString sTitle, QString sStartFolder)
{
	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		//Set initial folder if requested
		if (sStartFolder!="")
			bool bSucces=SetCurrentDirectory(sStartFolder.toStdString().c_str());

		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = hWndOwner; //NULL;
		bi.lpszTitle=sTitle.toStdString().c_str();
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;
		pidl = SHBrowseForFolder(&bi);
		if (pidl)
		{
			bool bSucces=SHGetPathFromIDList(pidl,szDir);

			pMalloc->Free(pidl);
			pMalloc->Release();
			// In C: pMalloc->lpVtbl->Free(pMalloc,pidl);
			//       pMalloc->lpVtbl->Release(pMalloc);
		}
		else
			return "";
	}
	return szDir;
}

std::vector<CString> TokenizeString(const CString& str, const CString& delim)
{
  std::vector<CString> vcRet;
  int curPos = 0;
  CString res = str.Tokenize(delim, curPos);
  while(!res.IsEmpty())
  {
    vcRet.push_back(res);
    res = str.Tokenize(delim, curPos);
  }

  return vcRet;
}
*/
