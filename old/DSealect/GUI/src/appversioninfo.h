#ifndef VERSIONINFO_H
#define VERSIONINFO_H 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAppVersionInfo
{
public:
	CString m_strVersionInfo;
	CString m_strFixedFileVersion;
	CString m_strFixedProductVersion;
	WORD m_major;
	WORD m_minor;
	CAppVersionInfo ();
	~CAppVersionInfo ();
	CAppVersionInfo (HMODULE hModule, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
	CAppVersionInfo (LPTSTR szFilename, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
	void GetVersionInfo (LPTSTR szFilename, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
	void GetVersionInfo (HMODULE hModule, LPCTSTR strLangID = NULL, LPCTSTR strInfoType = NULL);
}; 

#endif // VERSIONINFO_H
