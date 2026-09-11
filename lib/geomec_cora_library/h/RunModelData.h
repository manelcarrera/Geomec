#ifndef _cora_RunModelData_h_
#define _cora_RunModelData_h_

#include "ModelData.h"
#include "ParameterFile.h"
#include "SelectedLSFs.h"
#include "SummaryResultFile.h"
#include "GetModelInfo.h"
#include "ResponseParameterFile.h"

namespace cora
{

class CRunModelData
{
  public:
    static const QString MINIMUM_OUTPUT;

    CRunModelData(int argc, char* argv[]);
    ~CRunModelData();

    CModelData& modelData();
    const CModelData& modelData() const;

    const QString& versionNumber() const;

    CSummaryResultFile& summaryResultFile();
    const CSummaryResultFile& summaryResultFile() const;

    CGetModelInfo& getModelInfo();
    const CGetModelInfo& getModelInfo() const;

    const CParameterFile& parameterFile() const;
    const CSelectedLSFs& selectedLSFs() const;

    CResponseParameterFile& responseParameterFile();
    const CResponseParameterFile& responseParameterFile() const;

    bool minimumOutput() const;

  private:
    CRunModelData(const CRunModelData& rhs);
    CRunModelData& operator = (const CRunModelData& rhs);

    CModelData m_modelData;
    const QString m_versionNumber;
    CSummaryResultFile m_summaryResultFile;
    CGetModelInfo m_getModelInfo;
    const CParameterFile m_parameterFile;
    const CSelectedLSFs m_selectedLSFs;
    CResponseParameterFile m_responseParameterFile;
    bool m_minimumOutput;
};

} // namespace cora

#endif  // _cora_RunModelData_h_
