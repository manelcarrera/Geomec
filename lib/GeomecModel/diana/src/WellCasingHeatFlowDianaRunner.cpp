
#include "WellCasingHeatFlowDianaRunner.h"
#include "WellCasingModel.h"
#include "WellCasingDianaRunnerHelper.h"
#include "DianaRunController.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingNode.h"
#include "DCBoundaCase.h"
#include "DCNodalTemperature.h"

CWellCasingHeatFlowDianaRunner::CWellCasingHeatFlowDianaRunner(CWellCasingModel& model, CDianaRunController& controller)
: CGeomecHeatFlowDianaRunner(model.Mesh(), model, controller)
{
}

const dia::IMaterial& CWellCasingHeatFlowDianaRunner::Material(const geo::IElement &element) const
{
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  const dia::IMaterial* pMat = helper.Material(element);
  if(pMat)
    return *pMat;

  return CGeomecHeatFlowDianaRunner::Material(element);
}

const dia::IMaterial* CWellCasingHeatFlowDianaRunner::InterfaceMaterial(const geo::IElement &element) const
{
  CWellCasingDianaRunnerHelper helper(CasingModel(), Controller(), ParentLinearResults());
  const dia::IMaterial* pMat = helper.InterfaceMaterial(element);
  return pMat;
}

const CWellCasingModel& CWellCasingHeatFlowDianaRunner::CasingModel() const
{
  return static_cast<const CWellCasingModel&>(Model());
}

CWellCasingModel& CWellCasingHeatFlowDianaRunner::CasingModel()
{
  return static_cast<CWellCasingModel&>(Model());
}

int CWellCasingHeatFlowDianaRunner::LoadsProgressSize() const
{
  int nGrids = CasingModel().Mesh().NumberOfGrids();
  int nSpokes = CasingModel().Mesh().TangentialElements();

  return CGeomecHeatFlowDianaRunner::LoadsProgressSize() + nGrids * nSpokes;
}

void CWellCasingHeatFlowDianaRunner::WriteModelSpecificLoads(IProgressBase& progress, const TStageMap& mpStages)
{
  // apply the inner temperatures to the inner nodes of the steel elements

  int nNodesPerSpoke = CasingModel().Mesh().NodesPerSpoke();
  int nGrids = CasingModel().Mesh().NumberOfGrids();
  int nSpokes = CasingModel().Mesh().TangentialElements();
  const geo::IMesh& mesh = CasingModel().Mesh().Mesh();

  // the current node index
  int nNode = 0;

  int n;
  for(n = 0; n < nGrids; ++n)
  {
    bool bCap = (n == 0 || n == nGrids - 1);

    // loop over the spokes
    int i;
    for(i = 0; i < nSpokes; ++i)
    {
      const geo::INode& node = mesh.Node(nNode);

      CDepletionStageEntry::const_iterator it;
      for(it = Model().DepletionStageEntry().begin(); it != Model().DepletionStageEntry().end(); ++it)
      {
        const CDepletionStage& stage = *it;
        TStageMap::const_iterator its = mpStages.find(&stage);
        assert(its != mpStages.end());
        dia::CBoundaCase& bcase = *its->second;
        if(bcase.Active())
        {
          const CWellCasingInternalTemperature& internalTemper = CasingModel().CasingNode().InternalTemperature(stage);
          double dTemper = internalTemper.Component().ScalarData().ValuePoint(node).Value();
          if(fabs(dTemper) > EPS)
            new dia::CNodalTemperature(bcase, dTemper + 273.15, node);
        }
      }

      nNode += (bCap ? 2 * nNodesPerSpoke : nNodesPerSpoke);

      progress.Step();
    }
  }
}

bool CWellCasingHeatFlowDianaRunner::ParentLinearResults() const
{
  return CasingModel().ParentLinearResults(Controller().AnalysisType());
}
