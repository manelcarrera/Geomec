#ifndef _cora_RunModel_h_
#define _cora_RunModel_h_

#include <set>

#include <QString>

namespace cora
{

class CRunModelData;
class CSummaryResultFile;
class CSelectedLSFs;

} // namespace cora

#include "AnalysisType.h"

namespace cora
{

class CRunModel
{
  public:
  CRunModel(CRunModelData& runModelData, const QString& applicationVersion);

  bool operator () ();

  private:
  CRunModel(const CRunModel& rhs);
  CRunModel& operator = (CRunModel rhs);

  void validateVersionNumber();

  typedef std::set <CAnalysisType::TAnalysisType> TAnalysisTypes;

  void processModelData();

  static TAnalysisTypes selectAnalysisTypes(
      CSummaryResultFile& summaryResultFile, const CSelectedLSFs& selectedLSFs);

  CRunModelData& m_runModelData;
  const QString m_applicationVersion;
  TAnalysisTypes m_analysisTypes;
};

} // namespace cora

#endif  // _cora_RunModel_h_
