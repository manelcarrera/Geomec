#ifndef _cora_ParameterFile_h_
#define _cora_ParameterFile_h_

#include <fstream>
#include <vector>

#include <QString>

namespace cora {

class CSummaryResultFile;
class CGetModelInfo;

} // namespace cora

#include "FailureTypeParameter.h"

namespace cora {

class CParameterFile {
public:
  CParameterFile(CSummaryResultFile &summaryResultFile, const CGetModelInfo &modelInfo, const QString &fileName);

  std::vector<TFailureTypeParameter> getParameters() const;

private:
  CParameterFile(const CParameterFile &rhs);
  CParameterFile &operator=(CParameterFile rhs);

  void validateParameterFile();
  void validatePressureOrdering();
  void validateDepletionStageDesignator();

  static std::vector<TFailureTypeParameter> readParameterFile(CSummaryResultFile &summaryResultFile,
                                                              const CGetModelInfo &modelInfo,
                                                              std::ifstream &parameterFile, const QString &fileName);

  const QString m_fileName;
  std::ifstream m_parameterFile;
  CSummaryResultFile &m_summaryResultFile;
  std::vector<TFailureTypeParameter> m_parameters;
};

} // namespace cora

#endif // _cora_ParameterFile_h_
