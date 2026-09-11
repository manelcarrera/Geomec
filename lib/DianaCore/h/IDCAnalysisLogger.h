// IDCAnalysisLogger.h: interface for the IAnalysisLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDCANALYSISLOGGER_H__07F11926_E5EA_4A30_8ADD_11492E5D947E__INCLUDED_)
#define AFX_IDCANALYSISLOGGER_H__07F11926_E5EA_4A30_8ADD_11492E5D947E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

namespace dia {

class IAnalysisLogger {
public:
  IAnalysisLogger();
  virtual ~IAnalysisLogger();

  void Empty();
  void AddLine(const std::string &Line, bool bVerbose = false);
  bool IsEmpty() const;

  long LineSize() const;
  const std::string &Line(int nIndex) const;
  bool Verbose(int nIndex) const;

  // export the log to an ASCII file given the (absolute) path
  bool ExportToFile(const std::string &path, bool bVerbose = false) const;

  // Qt's signal handlers get in the way of using a specific logger, so we introduce a flag to ignore messages
  void Silent(bool flag);

private:
  bool m_bSilent;
  std::vector<std::pair<std::string, bool>> m_vcStrings;
};

} // namespace dia

#endif // !defined(AFX_IDCANALYSISLOGGER_H__07F11926_E5EA_4A30_8ADD_11492E5D947E__INCLUDED_)
