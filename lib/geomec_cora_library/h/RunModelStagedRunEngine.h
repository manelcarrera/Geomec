#ifndef _cora_RunModelStagedRunEngine_h_
#define _cora_RunModelStagedRunEngine_h_

#include "RunModelEngineBase.h"

namespace cora
{

class CRunModelStagedRunEngine : public CRunModelEngineBase
{
  public:
  CRunModelStagedRunEngine(CAnalysisType::TAnalysisType analysisType);

  virtual bool run(CRunModelData& runModelData, CDianaExecuter& dianaExecuter,
      const QString& modelName, CSaveModelConsole& saveModelConsole,
      CRetrieveDianaFileNamesConsole& retrieveDianaFileNamesConsole) const;

  private:
  CRunModelStagedRunEngine(const CRunModelStagedRunEngine& rhs);
  CRunModelStagedRunEngine& operator = (CRunModelStagedRunEngine rhs);
};

} // namespace cora

#endif  // _cora_RunModelStagedRunEngine_h_
