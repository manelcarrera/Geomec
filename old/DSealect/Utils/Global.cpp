

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <direct.h>
#include "Global.h"
#include <vector>
#include <Userenv.h>
#include <cassert>

#define USE_UNITBRACKETS 1

CString GetAppPath()
{
	TCHAR app_path[_MAX_PATH];
	GetModuleFileName((HMODULE)AfxGetInstanceHandle(), app_path, MAX_PATH);
	CString app_str = app_path;
	app_str = app_str.Left(app_str.ReverseFind('\\')+1);
	return app_str;
}

QString GetHomeDirectory()
{
  TCHAR homeDirectory[MAX_PATH] = {0};
  HANDLE token = 0;
  DWORD bufferSize = MAX_PATH;
  BOOL succeeded = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);

  assert(succeeded);

  succeeded = GetUserProfileDirectory(token, homeDirectory, &bufferSize);

  assert(succeeded);

  CloseHandle(token);

  return (LPCTSTR) EnsureBackslash(homeDirectory);
}

CString RemoveExtension(const CString &sFilename)
{
	char DOT='.'; 

	// Check for trailing slash: 
	int pos = sFilename.ReverseFind(DOT); 

	if(pos<0)
		return CString();

	return sFilename.Left(pos);

}

void SplitPathAndFileName(const CString& sPathAndFileName,CString& sPath,CString& sFileName)
{
	char SLASH='\\'; 
	int pos=0; 

	// Check for trailing slash: 
	pos = sPathAndFileName.ReverseFind(SLASH); 

	if(pos<0)
		return;

	sPath=sPathAndFileName.Left(pos);
	sFileName=sPathAndFileName.Right(sPathAndFileName.GetLength() - pos - 1);

}

BOOL FileExists( CString strPath )
{
	struct _stat buf;
	if( _stat( strPath, &buf ) != 0 ) return FALSE;
	if( buf.st_mode & _S_IFDIR ) return FALSE; // may not be a directory

	return TRUE;
}


CString GetTempEnv()
{
	CString sTemp;
	//Get environment setting for Temp
	char *szDir;		//Resulting Value
	szDir=(char *)malloc(MAX_PATH);
	DWORD dwLength = MAX_PATH;	//Maximum length of value
	long lRet=GetEnvironmentVariable("TEMP",szDir,dwLength);
	if (lRet!=0)
		sTemp=szDir;
	else
		sTemp="";
	
	free(szDir);
	return sTemp;
}

void SetFileReadOnly( CString strPath, BOOL bReadOnly )
{
	struct _stat buf;
	if( _stat( strPath, &buf ) != 0 ) return; // file does not exist
	CFileStatus Status;

	CFile::GetStatus( strPath, Status );

	if( bReadOnly ) Status.m_attribute |= CFile::readOnly; // set read-only flag
	else Status.m_attribute ^= CFile::readOnly; // remove read-only flag

	CFile::SetStatus( strPath, Status );
}





//taken from KB article Q179378
int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH];
	switch(uMsg)
	{
		case BFFM_INITIALIZED:
			{
				if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
				{
					// WParam is TRUE since you are passing a path.
					// It would be FALSE if you were passing a pidl.
					SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
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

CString BrowseFolder(HWND hWndOwner, CString sTitle, CString sStartFolder)
{
	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		//Set initial folder if requested
		if (sStartFolder!="")
			BOOL bSucces=SetCurrentDirectory(sStartFolder);

		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = hWndOwner; //NULL;
		bi.lpszTitle=sTitle;
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;
		pidl = SHBrowseForFolder(&bi);
		if (pidl)
		{
			BOOL bSucces=SHGetPathFromIDList(pidl,szDir);

			pMalloc->Free(pidl);
			pMalloc->Release();
			/* In C: pMalloc->lpVtbl->Free(pMalloc,pidl);
               pMalloc->lpVtbl->Release(pMalloc);*/
		}
		else
			return "";
	}
	return szDir;
}

extern "C" BOOL DirExists( LPCTSTR strPath )
{
	struct _stat buffer;
	if( _stat( strPath, &buffer ) ) return FALSE;

	if( !( buffer.st_mode | _S_IFDIR ) ) return FALSE;

	return TRUE;
}

// This is a recursive function which empty a directory path 
// of its files and call itself if necessary to delete
// subdirectories.
BOOL EmptyDirectory(const CString &sPath)
{
	CFileFind finder;

	CString  sWildCard = sPath + "\\*.*";

	BOOL bFound;
	BOOL bWorking = finder.FindFile(sWildCard);

	bFound = bWorking;

	while (bWorking) 
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots()) continue;

		if (finder.IsDirectory()) 
		{
			CString s = finder.GetFilePath();
			EmptyDirectory(s);
			RemoveDirectory(finder.GetFilePath());
			continue; 
		}
		_unlink( finder.GetFilePath() );

	}

	return bFound;
}


CString EnsureBackslash(const CString path)
{
	if(path.Right(1) != "\\")
		return path + "\\";
	else
		return path;
}


CString EnsureNoBackslash(const CString path)
{
	if(path.Right(1) == "\\")
		return path.Left(path.GetLength()-1);
	else
		return path;
}

//recursivly buld up a directory tree
bool CreateDirectory(CString wsPath)
{ 

	char SLASH='\\'; 
	DWORD attr; 
	int pos; 
	bool result = true; 

	// Check for trailing slash: 
	pos = wsPath.ReverseFind(SLASH); 

	if (wsPath.GetLength() == pos + 1) // last character is "\" 
	{ 
		wsPath = wsPath.Left(pos); 
	} 

	// Look for existing object: 
	attr = GetFileAttributes(LPCTSTR(wsPath)); 

	if (0xFFFFFFFF == attr) // doesn't exist yet - create it! 
	{	
		pos = wsPath.ReverseFind(SLASH); 
		if (0 < pos) 
		{ 
			// Create parent dirs: 
			result = CreateDirectory(wsPath.Left(pos)); 
		} 
		// Create node: 
		result = result && ::CreateDirectory(LPCTSTR(wsPath), NULL); 
	} 
	else if ((FILE_ATTRIBUTE_DIRECTORY & attr) == 0)
	{ // object already exists, but is not a dir 
		SetLastError(ERROR_FILE_EXISTS); 
		result = false; 
	} 
	return result; 
} 


BOOL DeleteFilesInDir(const CString& sDir,const CString& sFile)
{
	CString sFileName=EnsureBackslash(sDir) + sFile;

	CFileFind ff;

	BOOL bWorking=ff.FindFile(sFileName);
	std::vector<CString> vec;

	while(bWorking)
	{
		bWorking=ff.FindNextFile();
		CString sFile=EnsureBackslash(sDir) + ff.GetFileName();
		vec.push_back(sFile);
		//DeleteFile(sFile);
	}

	for(size_t i=0;i<vec.size();i++)
		DeleteFile(vec[i]);

	return TRUE;
}

/*
FileDeleter(CFileFinder* ff = 0, const CString& FileSpec ="")
{
	if(ff)
	{
		// We Have a FileFinder
		if(ff->GetNextFile())
		{
			CString sDelName = ff->GetFileName();
			FileDeleter(ff);
			DeleteFile(sDelName);
		}
	} else {
		// We Need to create the FileFinder 
		CFileFind ff;
		if(ff.FindFile(FileSpec))
		{
			if(ff.GetNextFile())
			{
				CString sDelName = ff.GetFileName();
				FileDeleter(&ff);
				DeleteFile(sDelName);
			}
		}
	}
}
*/

BOOL IsDirEmpty(const CString& sDir)
{
	CString sFileName=EnsureBackslash(sDir) + "*.*";

	CFileFind ff;
	if(ff.FindFile(sFileName))
	{
		while(ff.FindNextFile())
		{
			if(!ff.IsDots())
				return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	

}