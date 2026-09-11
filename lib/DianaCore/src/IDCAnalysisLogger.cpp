// IDCAnalysisLogger.cpp: implementation of the IAnalysisLogger class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>

#include "IDCAnalysisLogger.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

IAnalysisLogger::IAnalysisLogger() : m_bSilent(false) {}

IAnalysisLogger::~IAnalysisLogger() {}

void IAnalysisLogger::Empty() { m_vcStrings.clear(); }

void IAnalysisLogger::Silent(bool flag) { m_bSilent = flag; }

void IAnalysisLogger::AddLine(const std::string &Line, bool bVerbose) {
  if (!m_bSilent)
    m_vcStrings.push_back(std::make_pair(Line, bVerbose));
}

bool IAnalysisLogger::IsEmpty() const { return m_vcStrings.empty(); }

long IAnalysisLogger::LineSize() const { return (long)m_vcStrings.size(); }

const std::string &IAnalysisLogger::Line(int nIndex) const { return m_vcStrings[nIndex].first; }

bool IAnalysisLogger::Verbose(int nIndex) const { return m_vcStrings[nIndex].second; }

// export the log to an ASCII file given the (absolute) path
bool IAnalysisLogger::ExportToFile(const std::string &path, bool bVerbose) const {
  FILE *fp;
  fp = fopen(path.c_str(), "w");
  if (!fp)
    return false;

  int i;
  for (i = 0; i < LineSize(); i++) {
    if (bVerbose || !Verbose(i))
      fprintf(fp, "%s\n", Line(i).c_str());
  }

  fclose(fp);
  return true;
}

} // namespace dia
