#ifndef _cora_RunModelSingleRunEngine_h_
#define _cora_RunModelSingleRunEngine_h_

#include "RunModelEngineBase.h"

namespace cora
{

class CRunModelSingleRunEngine : public CRunModelEngineBase
{
  public:
    CRunModelSingleRunEngine(CAnalysisType::TAnalysisType analysisType);

    virtual bool run(CRunModelData& runModelData, CDianaExecuter& dianaExecuter,
      const QString& modelName, CSaveModelConsole& saveModelConsole,
      CRetrieveDianaFileNamesConsole& retrieveDianaFileNamesConsole) const;

  private:
    CRunModelSingleRunEngine(const CRunModelSingleRunEngine& rhs);
    CRunModelSingleRunEngine& operator = (CRunModelSingleRunEngine rhs);
};

} // namespace cora

#endif  // _cora_RunModelSingleRunEngine_h_
