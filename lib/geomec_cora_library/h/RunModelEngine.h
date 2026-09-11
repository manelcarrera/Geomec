#ifndef _cora_RunModelEngine_h_
#define _cora_RunModelEngine_h_

#include "AnalysisType.h"
#include "RunModelEngineBase.h"

namespace cora {

class CRunModelEngine {
public:
  CRunModelEngine(CAnalysisType::TAnalysisType analysisType);

  bool run(CRunModelData &runModelData, const QString &modelName, const QString &applicationVersion) const;

private:
  CRunModelEngine(const CRunModelEngine &rhs);
  CRunModelEngine &operator=(CRunModelEngine rhs);

  static TRunModelEngineBase selectEngine(CAnalysisType::TAnalysisType analysisType);

  TRunModelEngineBase m_runModelEngineBase;
};

} // namespace cora

#endif // _cora_RunModelEngine_h_
