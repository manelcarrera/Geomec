#ifndef _cora_DepletionStageAnalysisType_h_
#define _cora_DepletionStageAnalysisType_h_

#include <utility>
#include <vector>

class CDepletionStage;
class CAnalysisType;
class CModelBase;

namespace cora
{

typedef std::pair <const CDepletionStage*, CAnalysisType>
  TDepletionStageAnalysisType;
typedef std::vector <TDepletionStageAnalysisType>
  TDepletionStageAnalysisTypeVector;

class CDepletionStageAnalysisType
{
  public:
    CDepletionStageAnalysisType(CModelBase* modelBase);

    const TDepletionStageAnalysisTypeVector
      getDepletionStageAnalysisType() const;

  private:
    CDepletionStageAnalysisType(const CDepletionStageAnalysisType& rhs);
    CDepletionStageAnalysisType& operator = (CDepletionStageAnalysisType rhs);

    static TDepletionStageAnalysisTypeVector
      createDepletionStageAnalysisTypeVector(CModelBase* modelBase);

    TDepletionStageAnalysisTypeVector m_depletionStageAnalysisTypeVector;
};

} // namespace cora

#endif  // _cora_DepletionStageAnalysisType_h_
