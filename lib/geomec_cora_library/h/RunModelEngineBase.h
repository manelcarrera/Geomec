#ifndef _cora_RunModelEngineBase_h_
#define _cora_RunModelEngineBase_h_

#include "safeQSharedPointer.h"

namespace cora
{

class CRunModelData;

} // namespace cora

class CDianaExecuter;
class CSaveModelConsole;
class CRetrieveDianaFileNamesConsole;

#include "AnalysisType.h"

namespace cora
{

class CRunModelEngineBase
{
  public:
  CRunModelEngineBase(CAnalysisType::TAnalysisType analysisType);
  virtual ~CRunModelEngineBase() = 0;

  virtual bool run(CRunModelData& runModelData, CDianaExecuter& dianaExecuter,
      const QString& modelName, CSaveModelConsole& saveModelConsole,
      CRetrieveDianaFileNamesConsole& retrieveDianaFileNamesConsole) const = 0;

  protected:
  CAnalysisType::TAnalysisType m_analysisType;

  private:
  CRunModelEngineBase(const CRunModelEngineBase& rhs);
  CRunModelEngineBase& operator = (const CRunModelEngineBase& rhs);
};

typedef QSharedPointer <CRunModelEngineBase> TRunModelEngineBase;

} // namespace cora

#endif  // _cora_RunModelEngineBase_h_
