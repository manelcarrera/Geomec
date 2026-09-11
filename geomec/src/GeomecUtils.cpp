
#include "geomec.h"
#include "stdafx.h"

#include <direct.h>
#include <io.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "GeomecUtils.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

CString EnsureBackslash(const CString &path) {
  if (path.Right(1) != "\\")
    return path + "\\";
  else
    return path;
}

CString EnsureNoBackslash(const CString &path) {
  if (path.Right(1) == "\\")
    return path.Left(path.GetLength() - 1);
  else
    return path;
}

CString RemoveExtension(const CString &sFilename) {
  char DOT = '.';

  // Check for trailing slash:
  int pos = sFilename.ReverseFind(DOT);

  if (pos < 0)
    return CString();

  return sFilename.Left(pos);
}

// recursivly buld up a directory tree
bool CreateDirectory(QString wsPath) {

  char SLASH = '\\';
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
    if (0 < pos) {
      // Create parent dirs:
      result = CreateDirectory(wsPath.left(pos));
    }
    // Create node:
    result = result && ::CreateDirectory(wsPath.toStdString().c_str(), NULL);
  } else if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) { // object already exists, but is not a dir
    SetLastError(ERROR_FILE_EXISTS);
    result = false;
  }
  return result;
}

bool DeleteFilesInDir(const QString &sDir, const QString &sFile) {
  CString sFileName = (EnsureBackslash(sDir) + sFile).toStdString().c_str();

  CFileFind ff;

  bool bWorking = ff.FindFile(sFileName);
  std::vector<CString> vec;

  while (bWorking) {
    bWorking = ff.FindNextFile();
    CString cFile = (EnsureBackslash(sDir) + ff.GetFileName()).toStdString().c_str();
    vec.push_back(cFile);
    // DeleteFile(sFile);
  }

  for (size_t i = 0; i < vec.size(); i++)
    DeleteFile(vec[i]);

  return true;
}

// taken from KB article Q179378
static int CALLBACK BrowseCallbackProc(HWND hwnd, unsigned int uMsg, LPARAM lp, LPARAM pData) {
  TCHAR szDir[MAX_PATH];
  switch (uMsg) {
  case BFFM_INITIALIZED: {
    if (GetCurrentDirectory(sizeof(szDir) / sizeof(TCHAR), szDir)) {
      // WParam is TRUE since you are passing a path.
      // It would be FALSE if you were passing a pidl.
      SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
    }
    break;
  }
  case BFFM_SELCHANGED: {
    // Set the status window to the currently selected path.
    if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir)) {
      SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
    }
    break;
  }
  default:
    break;
  }
  return 0;
}

QString BrowseFolder(HWND hWndOwner, QString sTitle, QString sStartFolder) {
  BROWSEINFO bi;
  TCHAR szDir[MAX_PATH];
  LPITEMIDLIST pidl;
  LPMALLOC pMalloc;

  if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
    // Set initial folder if requested
    if (sStartFolder != "")
      bool bSucces = SetCurrentDirectory(sStartFolder.toStdString().c_str());

    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hWndOwner; // NULL;
    bi.lpszTitle = sTitle.toStdString().c_str();
    bi.pszDisplayName = 0;
    bi.pidlRoot = 0;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
    bi.lpfn = BrowseCallbackProc;
    pidl = SHBrowseForFolder(&bi);
    if (pidl) {
      bool bSucces = SHGetPathFromIDList(pidl, szDir);

      pMalloc->Free(pidl);
      pMalloc->Release();
      /* In C: pMalloc->lpVtbl->Free(pMalloc,pidl);
               pMalloc->lpVtbl->Release(pMalloc);*/
    } else
      return "";
  }
  return szDir;
}

std::vector<CString> TokenizeString(const CString &str, const CString &delim) {
  std::vector<CString> vcRet;
  int curPos = 0;
  CString res = str.Tokenize(delim, curPos);
  while (!res.IsEmpty()) {
    vcRet.push_back(res);
    res = str.Tokenize(delim, curPos);
  }

  return vcRet;
}
