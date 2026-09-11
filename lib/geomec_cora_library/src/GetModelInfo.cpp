#include "GetModelInfo.h"
#include "ModelData.h"
#include "VersionNumber.h"

namespace cora {

CGetModelInfo::CGetModelInfo(CModelData &modelData, const char *modelInfoFileName)
    : m_modelData(modelData), m_modelInfoFileName(modelInfoFileName), m_elementSetInfo(m_modelData()),
      m_estimatedMemoryUsage(), m_faultInfo(CGetFaultInfo::instance(m_modelData())),
      m_formationInfo(CGetFormationInfo::instance(m_modelData())), m_pointSetInfo(m_modelData()),
      m_failureModeInfo(m_modelData()), m_surfaceInfo(CGetSurfaceInfo::instance(m_modelData())),
      m_getNonMeshedSurfaceInfo(CGetNonMeshedSurfaceInfo::instance(m_modelData())),
      m_horizonInfo(CGetHorizonInfo::instance(m_modelData())),
      m_wellPathInfo(CGetWellPathInfo::instance(m_modelData())) {}

bool CGetModelInfo::operator()() const { return (((m_modelData() != 0) && createModelInfo()) ? true : false); }

const CGetFaultInfo &CGetModelInfo::getFaultInfo() const { return m_faultInfo; }

const CGetFormationInfo &CGetModelInfo::getFormationInfo() const { return m_formationInfo; }

CGetFailureModeInfo &CGetModelInfo::getFailureModeInfo() { return m_failureModeInfo; }

const CGetSurfaceInfo &CGetModelInfo::getSurfaceInfo() const { return m_surfaceInfo; }

const CGetHorizonInfo &CGetModelInfo::getHorizonInfo() const { return m_horizonInfo; }

const CGetWellPathInfo &CGetModelInfo::getWellPathInfo() const { return m_wellPathInfo; }

// private

bool CGetModelInfo::createModelInfo() const {
  std::ofstream modelInfoFile(m_modelInfoFileName);

  modelInfoFile << VERSION_NUMBER.toStdString() << std::endl;

  m_formationInfo(modelInfoFile);
  m_faultInfo(modelInfoFile);
  m_wellPathInfo(modelInfoFile);

  // TODO SURFACE-HORIZON
  // for now do not list the surfaces and non-meshed surfaces,
  // only the horizons (top-horizon)

  // m_surfaceInfo(modelInfoFile);
  // m_getNonMeshedSurfaceInfo(modelInfoFile);

  m_horizonInfo(modelInfoFile);
  m_pointSetInfo(modelInfoFile);
  m_elementSetInfo(modelInfoFile);
  m_failureModeInfo(modelInfoFile);
  m_estimatedMemoryUsage(modelInfoFile);

  return true;
}

} // namespace cora
