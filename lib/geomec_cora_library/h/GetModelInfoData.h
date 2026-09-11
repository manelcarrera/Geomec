#ifndef _cora_GetModelInfoData_h_
#define _cora_GetModelInfoData_h_

#include "ModelData.h"
#include "SummaryResultFile.h"

namespace cora {

class CGetModelInfoData {
public:
  CGetModelInfoData(int argc, char *argv[]);
  ~CGetModelInfoData();

  CModelData &modelData();
  const CModelData &modelData() const;

  const QString &versionNumber() const;

  const char *modelInfoFileName() const;

  CSummaryResultFile &summaryResultFile();
  const CSummaryResultFile &summaryResultFile() const;

private:
  CGetModelInfoData(const CGetModelInfoData &rhs);
  CGetModelInfoData &operator=(const CGetModelInfoData &rhs);

  static bool isVersionNumberPresent(int argc, char *argv[]);

  CModelData m_modelData;
  bool m_versionNumberIsPresent;
  const QString m_versionNumber;
  const char *m_modelInfoFileName;
  CSummaryResultFile m_summaryResultFile;
};

} // namespace cora

#endif // _cora_GetModelInfoData_h_
