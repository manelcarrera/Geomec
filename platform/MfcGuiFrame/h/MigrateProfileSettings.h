#ifndef _MigrateProfileSettings_h_
#define _MigrateProfileSettings_h_

class CMigrateProfileSettings
{
  public:
  CMigrateProfileSettings();

  void migrateProfileSettings(const LPCTSTR& pszRegistryKey,
      const LPCTSTR& pszProfileName, WORD major, WORD minor);

  private:
  CMigrateProfileSettings(const CMigrateProfileSettings& rhs);
  CMigrateProfileSettings& operator = (const CMigrateProfileSettings& rhs);

  bool migrateProfileSettings(const CString& strOldProductName,
      const LPCTSTR& pszRegistryKey, const LPCTSTR& pszProfileName);

  // hSourceKey can be opened with KEY_READ access,
  // hTargetKey requires KEY_ALL_ACCESS.

  bool CopyRegistrySubTree(HKEY hSourceKey, const CString& strSourceSubKey,
      HKEY hTargetKey, const CString& strTargetSubKey);
};

#endif  // _MigrateProfileSettings_h_
