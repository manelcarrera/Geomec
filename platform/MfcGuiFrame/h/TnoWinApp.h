#ifndef _TNOWINAPP_H_
#define _TNOWINAPP_H_

// class to add functionality to MFC's CWinApp
// - Read from registry in HKEY_LOCAL_MACHINE through functions GetSystemInt and GetSystemString

class CTnoWinApp : public CWinApp
{
private:
  class CRegKey
  {
  public:
  CRegKey(HKEY hKey);
  ~CRegKey();

  HKEY Key() const;

  private:
  HKEY m_hKey;
  };

private:
  CString m_strAppName;
  bool m_bExiting;

private:
  HKEY GetAppSystemRegistryKey();
  HKEY GetSystemSectionKey(LPCTSTR lpszSection);

public:
  CTnoWinApp();
  ~CTnoWinApp();

  unsigned int GetSystemInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);

  bool IsExiting() const;
  void SetIsExiting();

protected:
  //{{AFX_MSG(CWinApp)
  afx_msg void OnAppExit();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

#endif /* _TNOWINAPP_H_ */
