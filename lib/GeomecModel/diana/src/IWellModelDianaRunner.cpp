
#include "IWellModelDianaRunner.h"

#include "DCWeightLoad.h"
#include "DianaRunController.h"
#include "IWellModel.h"
#include "WellCasingDianaRunnerHelper.h"

IWellModelDianaRunner::IWellModelDianaRunner(IWellModel &model, CDianaRunController &controller)
    : CGeomecCompactionDianaRunner(model.Mesh(), model, controller) {}

IWellModelDianaRunner::~IWellModelDianaRunner() {}

bool IWellModelDianaRunner::CreateTopLoad(const CHorizonBase & /*top*/, dia::CLoadCase & /*lcase*/) { return false; }

void IWellModelDianaRunner::CreateWeightLoad(dia::CLoadCase &lcase, const double &gravity) {
  new dia::CWeightLoad(lcase, gravity, geo::CVector::Zaxis);
}

bool IWellModelDianaRunner::ParentLinearResults() const {
  const IWellModel &model = static_cast<const IWellModel &>(Model());
  return model.ParentLinearResults(Controller().AnalysisType());
}

int IWellModelDianaRunner::LoadsProgressSize() const {
  const IWellModel::CBoundary &boundary = static_cast<const IWellModel::CBoundary &>(Model().Boundary());
  int nBoundary = boundary.BoundaryElementSize();

  int nStages = Controller().EndStage().Index() - Controller().StartStage().Index() + 1;
  int nElements = Mesh().ElementSize();

  return nBoundary + nStages * nElements;
}
