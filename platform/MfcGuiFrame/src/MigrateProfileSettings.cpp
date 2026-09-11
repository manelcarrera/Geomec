
#include "StdAfx.h"

#include <vector>

#include "GlobalMessage.h"
#include "MigrateProfileSettings.h"

CMigrateProfileSettings::CMigrateProfileSettings() {}

namespace {

const CString PRODUCT_NAME = "GEOMEC %u.%u";
const CString SUCCESSFUL_MIGRATION = "Succesfully migrated the user settings from %s to %s";

} // anonymous namespace

void CMigrateProfileSettings::migrateProfileSettings(const LPCTSTR &pszRegistryKey, const LPCTSTR &pszProfileName,
                                                     WORD major, WORD minor) {
  CString migrateString;
  WORD iMinor;
  WORD iMajor;

  if (minor > 0) // migrating from previous minor
  {
    iMajor = major;
    iMinor = minor - 1;
  } else if (major == 4) // migrate from version 3.7
  {
    iMajor = 3;
    iMinor = 7;
  } else if (major == 5) // migrate from version 4.[0-3]
  {
    iMajor = 4;
    iMinor = 3;
  }

  bool bMigrated = false;

  while (!bMigrated && (short)iMinor >= 0) {
    // try to migrate user settings from a previous version if current doesn't exist

    CString strOldProductName;

    strOldProductName.Format(PRODUCT_NAME, iMajor, iMinor);
    bMigrated = migrateProfileSettings(strOldProductName, pszRegistryKey, pszProfileName);

    if (bMigrated) {
      migrateString.Format(SUCCESSFUL_MIGRATION, strOldProductName, pszProfileName);
    } else {
      --iMinor;
    }
  }

  if (!migrateString.IsEmpty()) {
    _m()->msg(migrateString, MB_ICONINFORMATION);
  }
}

// private

namespace {

const LPCTSTR SOFTWARE = _T("Software");

} // anonymous namespace

bool CMigrateProfileSettings::migrateProfileSettings(const CString &strOldProductName, const LPCTSTR &pszRegistryKey,
                                                     const LPCTSTR &pszProfileName) {
  assert(pszRegistryKey != NULL);
  assert(pszProfileName != NULL);

  HKEY hSoftKey = NULL;
  HKEY hCompanyKey = NULL;

  bool res = RegOpenKeyEx(HKEY_CURRENT_USER, SOFTWARE, 0, KEY_ALL_ACCESS, &hSoftKey);
  if (res == ERROR_SUCCESS) {
    CRegKey softKey(hSoftKey);
    DWORD dw;

    res = RegCreateKeyEx(hSoftKey, pszRegistryKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                         &hCompanyKey, &dw);
    if (res == ERROR_SUCCESS) {
      CRegKey companyKey(hCompanyKey);
      HKEY hProfileKey;

      res = RegOpenKeyEx(hCompanyKey, pszProfileName, 0, KEY_READ, &hProfileKey);
      if (res != ERROR_SUCCESS) {
        res = CopyRegistrySubTree(hCompanyKey, strOldProductName, hCompanyKey, pszProfileName);
        return res;
      } else {
        RegCloseKey(hProfileKey);
      }
    } // create
  } // open

  return false;
}

bool CMigrateProfileSettings::CopyRegistrySubTree(HKEY hSourceKey, const CString &strSourceSubKey, HKEY hTargetKey,
                                                  const CString &strTargetSubKey) {
  HKEY hSourceSubKey = NULL;
  HKEY hTargetSubKey = NULL;

  bool res = RegOpenKeyEx(hSourceKey, strSourceSubKey, 0, KEY_READ, &hSourceSubKey);
  if (res == ERROR_SUCCESS) {
    CRegKey srcKey(hSourceSubKey);
    DWORD dw;

    res = RegCreateKeyEx(hTargetKey, strTargetSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                         &hTargetSubKey, &dw);
    if (res == ERROR_SUCCESS) {
      CRegKey trgKey(hTargetSubKey);
      DWORD dwIndex;
      bool bContinue = true;
      TCHAR sName[1024];
      DWORD dwSize;

      // copy values
      std::vector<std::pair<CString, DWORD>> vcValues; // name and data size

      for (dwIndex = 0; bContinue; ++dwIndex) {
        dwSize = 1024;

        DWORD dwDataSize;

        bContinue = (RegEnumValue(hSourceSubKey, dwIndex, sName, &dwSize, 0, 0, NULL, &dwDataSize) == ERROR_SUCCESS);

        if (bContinue) {
          vcValues.push_back(std::make_pair(CString(sName), dwDataSize));
        }
      }

      for (size_t i = 0; i < vcValues.size(); ++i) {
        DWORD dwType;
        BYTE *bData = new BYTE[vcValues[i].second];

        res = RegQueryValueEx(hSourceSubKey, vcValues[i].first, 0, &dwType, bData, &vcValues[i].second);
        if (res != ERROR_SUCCESS) {
          return false;
        }

        res = RegSetValueEx(hTargetSubKey, vcValues[i].first, 0, dwType, bData, vcValues[i].second);
        if (res != ERROR_SUCCESS) {
          return false;
        }
      }

      // copy subkeys

      std::vector<CString> vcSubKeyNames;

      bContinue = true;

      for (dwIndex = 0; bContinue; ++dwIndex) {
        dwSize = 1024;
        bContinue = (RegEnumKeyEx(hSourceSubKey, dwIndex, sName, &dwSize, 0, 0, 0, 0) == ERROR_SUCCESS);

        if (bContinue) {
          vcSubKeyNames.push_back(sName);
        }
      }

      for (size_t i = 0; i < vcSubKeyNames.size(); ++i) {
        if (!CopyRegistrySubTree(hSourceSubKey, vcSubKeyNames[i], hTargetSubKey, vcSubKeyNames[i])) {
          return false;
        }
      }

      return true;
    }
  }

  return false;
}
