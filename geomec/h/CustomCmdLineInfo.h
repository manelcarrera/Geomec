#pragma once

class CCustomCmdLineInfo : public CCommandLineInfo {
public:
  struct Test {
    bool enabled;
    std::string url;
    std::string token;
  };

private:
  bool m_version;
  bool m_exit;
  bool m_output;

  Test m_test;

  CString m_outputFile;

public:
  CCustomCmdLineInfo(void);

  virtual void ParseParam(const TCHAR *pszParam, BOOL bFlag, BOOL bLast);

  bool Version() const { return m_version; }
  bool Exit() const { return m_exit; }
  bool Output() const { return m_output; }

  CString OutputFile() const { return m_outputFile; }

  Test test() { return m_test; }
};