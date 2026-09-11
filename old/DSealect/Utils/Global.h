

#if !defined(DS_GLOBAL__INCLUDED_)
#define DS_GLOBAL__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
	#include <QString>

	CString GetAppPath();
	QString GetHomeDirectory();
	BOOL FileExists( CString strPath );
	CString GetTempEnv();
	void SetFileReadOnly( CString strPath, BOOL bReadOnly );
	extern "C" BOOL DirExists( LPCTSTR strPath );
	CString BrowseFolder(HWND hWndOwner, CString sTitle, CString sStartFolder);
	BOOL EmptyDirectory(const CString &sPath);
	bool CreateDirectory(CString wsPath);
	CString EnsureBackslash(const CString path);
	CString EnsureNoBackslash(const CString path);
	void SplitPathAndFileName(const CString& sPathAndFileName,CString& sPath,CString& sFileName);
	CString RemoveExtension(const CString &sFilename);
	BOOL DeleteFilesInDir(const CString& sDir,const CString& sFile);
	BOOL IsDirEmpty(const CString& sDir);
#endif
