#include "DepletionStageAnalysisType.h"
#include "ModelBase.h"

namespace cora {

CDepletionStageAnalysisType::CDepletionStageAnalysisType(CModelBase *modelBase)
    : m_depletionStageAnalysisTypeVector(createDepletionStageAnalysisTypeVector(modelBase)) {}

const TDepletionStageAnalysisTypeVector CDepletionStageAnalysisType::getDepletionStageAnalysisType() const {
  return m_depletionStageAnalysisTypeVector;
}

// private

// static

TDepletionStageAnalysisTypeVector
CDepletionStageAnalysisType::createDepletionStageAnalysisTypeVector(CModelBase *modelBase) {
  TDepletionStageAnalysisTypeVector depletionStageAnalysisTypeVector;
  const CDepletionStage *depletionStage = &(modelBase->InitialDepletionStage());

  while ((depletionStage = (depletionStage->Last()) ? 0 : &(depletionStage->Next())) != 0) {
    CAnalysisType analysisType = CAnalysisType::FirstType();

    while (analysisType.Valid()) {
      depletionStageAnalysisTypeVector.push_back(TDepletionStageAnalysisType(depletionStage, analysisType));
      analysisType = analysisType.NextType();
    }
  }

  return depletionStageAnalysisTypeVector;
}

} // namespace cora
