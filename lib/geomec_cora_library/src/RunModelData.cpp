#include "RunModelData.h"

namespace cora
{

namespace
{

const char* EMPTY = "";

} // anonymous namespace

const QString CRunModelData::MINIMUM_OUTPUT = "minimum_output";

CRunModelData::CRunModelData(int argc, char* argv[])
: m_modelData(argv[1])
, m_versionNumber(argv[3])
, m_summaryResultFile(argv[7])
, m_getModelInfo(m_modelData, EMPTY)
, m_parameterFile(m_summaryResultFile, m_getModelInfo, argv[4])
, m_selectedLSFs(m_summaryResultFile, m_getModelInfo, m_versionNumber, argv[5])
, m_responseParameterFile(argv[6])
, m_minimumOutput(argc >= 9 ? argv[8] == MINIMUM_OUTPUT : false)
{
}

CRunModelData::~CRunModelData()
{
}

CModelData& CRunModelData::modelData()
{
  return m_modelData;
}

const CModelData& CRunModelData::modelData() const
{
  return m_modelData;
}

const QString& CRunModelData::versionNumber() const
{
  return m_versionNumber;
}

CSummaryResultFile& CRunModelData::summaryResultFile()
{
  return m_summaryResultFile;
}

const CSummaryResultFile& CRunModelData::summaryResultFile() const
{
  return m_summaryResultFile;
}

CGetModelInfo& CRunModelData::getModelInfo()
{
  return m_getModelInfo;
}

const CGetModelInfo& CRunModelData::getModelInfo() const
{
  return m_getModelInfo;
}

const CParameterFile& CRunModelData::parameterFile() const
{
  return m_parameterFile;
}

const CSelectedLSFs& CRunModelData::selectedLSFs() const
{
  return m_selectedLSFs;
}

CResponseParameterFile& CRunModelData::responseParameterFile()
{
  return m_responseParameterFile;
}

const CResponseParameterFile& CRunModelData::responseParameterFile() const
{
  return m_responseParameterFile;
}

bool CRunModelData::minimumOutput() const
{
  return m_minimumOutput;
}

} // namespace cora
