#include "StdAfx.h"

#include "TnoWinApp.h"
#include "ISettings.h"
#include "Environment.h"

CTnoWinApp::CRegKey::CRegKey(HKEY hKey)
: m_hKey(hKey)
{
}

CTnoWinApp::CRegKey::~CRegKey()
{
  if(m_hKey)
    RegCloseKey(m_hKey);
}

HKEY CTnoWinApp::CRegKey::Key() const
{
  return m_hKey;
}


BEGIN_MESSAGE_MAP(CTnoWinApp, CWinApp)
	//{{AFX_MSG_MAP(CTnoWinApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTnoWinApp::CTnoWinApp()
: m_bExiting( false )
{
}

CTnoWinApp::~CTnoWinApp()
{
}

// returns key for HKEY_LOCAL_MACHINE\"SOFTWARE"\RegistryKey\ProfileName, read-only
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CTnoWinApp::GetAppSystemRegistryKey()
{
	assert(m_pszRegistryKey != NULL);
	assert(m_pszProfileName != NULL);

	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE"), 0, KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, m_pszRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, m_pszProfileName, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
				&hAppKey, &dw);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hAppKey;
}

// returns key for:
//      HKEY_LOCAL_MACHINE\"SOFTWARE"\RegistryKey\AppName\lpszSection
// read-only
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CTnoWinApp::GetSystemSectionKey(LPCTSTR lpszSection)
{
	assert(lpszSection != NULL);

	HKEY hSectionKey = NULL;
	HKEY hAppKey = GetAppSystemRegistryKey();
	if (hAppKey == NULL)
		return NULL;

	DWORD dw;
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}

unsigned int CTnoWinApp::GetSystemInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	assert(lpszSection != NULL);
	assert(lpszEntry != NULL);
	assert(m_pszRegistryKey != NULL);

	HKEY hSecKey = GetSystemSectionKey(lpszSection);
	if (hSecKey == NULL)
		return nDefault;
	DWORD dwValue;
	DWORD dwType;
	DWORD dwCount = sizeof(DWORD);
	LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
		(LPBYTE)&dwValue, &dwCount);
	RegCloseKey(hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		assert(dwType == REG_DWORD);
		assert(dwCount == sizeof(dwValue));
		return (unsigned int)dwValue;
	}
	return nDefault;
}


void CTnoWinApp::OnAppExit()
{
	bool prev = m_bExiting;
	m_bExiting = true;
	CWinApp::OnAppExit();
	m_bExiting = prev;
}

bool CTnoWinApp::IsExiting() const
{
	return m_bExiting;
}

void CTnoWinApp::SetIsExiting()
{
  m_bExiting = true;
}