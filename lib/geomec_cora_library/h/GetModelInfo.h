#ifndef _cora_GetModelInfo_h_
#define _cora_GetModelInfo_h_

class CModelData;

#include "GetElementSetInfo.h"
#include "GetEstimatedMemoryUsage.h"
#include "GetFaultInfo.h"
#include "GetFormationInfo.h"
#include "GetPointSetInfo.h"
#include "GetFailureModeInfo.h"
#include "GetSurfaceInfo.h"
#include "GetNonMeshedSurfaceInfo.h"
#include "GetHorizonInfo.h"
#include "GetWellPathInfo.h"

namespace cora
{

class CGetModelInfo
{
  public:
  CGetModelInfo(CModelData& modelData, const char* modelInfoFileName);

  bool operator () () const;

  const CGetFaultInfo& getFaultInfo() const;
  const CGetFormationInfo& getFormationInfo() const;
  CGetFailureModeInfo& getFailureModeInfo();
  const CGetSurfaceInfo& getSurfaceInfo() const;
  const CGetHorizonInfo& getHorizonInfo() const;
  const CGetWellPathInfo& getWellPathInfo() const;

  private:
  CGetModelInfo(const CGetModelInfo& rhs);
  CGetModelInfo& operator = (CGetModelInfo rhs);

  bool createModelInfo() const;

  CModelData& m_modelData;
  const char* m_modelInfoFileName;
  CGetElementSetInfo m_elementSetInfo;
  CGetEstimatedMemoryUsage m_estimatedMemoryUsage;
  CGetFaultInfo& m_faultInfo;
  CGetFormationInfo& m_formationInfo;
  CGetPointSetInfo m_pointSetInfo;
  CGetFailureModeInfo m_failureModeInfo;
  CGetSurfaceInfo& m_surfaceInfo;
  CGetNonMeshedSurfaceInfo& m_getNonMeshedSurfaceInfo;
  CGetHorizonInfo& m_horizonInfo;
  CGetWellPathInfo& m_wellPathInfo;
};

} // namespace cora

#endif  // _cora_GetModelInfo_h_
