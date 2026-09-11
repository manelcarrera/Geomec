#ifndef _cora_SelectedLSFs_h_
#define _cora_SelectedLSFs_h_

#include <fstream>
#include <vector>

#include <QString>

namespace cora {

class CSummaryResultFile;

} // namespace cora

#include "LimitStateFunction.h"

namespace cora {

class CSelectedLSFs {
public:
  CSelectedLSFs(CSummaryResultFile &summaryResultFile, CGetModelInfo &modelInfo, const QString &versionNumber,
                const QString &fileName);

  std::vector<TLimitStateFunction> getLimitStateFunctions() const;

private:
  CSelectedLSFs(const CSelectedLSFs &rhs);
  CSelectedLSFs &operator=(CSelectedLSFs rhs);

  static std::vector<TLimitStateFunction> readSelectedLSFs(CSummaryResultFile &summaryResultFile,
                                                           std::ifstream &selectedLSFsFile, CGetModelInfo &modelInfo,
                                                           const QString &versionNumber, const QString &fileName);

  const QString m_fileName;
  std::ifstream m_selectedLSFsFile;
  CSummaryResultFile &m_summaryResultFile;
  std::vector<TLimitStateFunction> m_limitStateFunctions;
};

} // namespace cora

#endif // _cora_SelectedLSFs_h_
