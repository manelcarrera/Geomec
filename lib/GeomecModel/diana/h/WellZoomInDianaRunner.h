#ifndef _WELLZOOMINDIANARUNNER_H_
#define _WELLZOOMINDIANARUNNER_H_

#include "IWellModelDianaRunner.h"

class CWellZoomInModel;

class CWellZoomInDianaRunner : public IWellModelDianaRunner {
public:
  CWellZoomInDianaRunner(CWellZoomInModel &model, CDianaRunController &controller);
  virtual ~CWellZoomInDianaRunner();

  virtual bool FetchInitialPressures(const geo::IElement &element, const CDepletionStage &stage,
                                     IValueDomainScalar::TValueVec &vcInitialPressures);
  virtual bool CreateInitialLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase,
                                  const IValueDomainScalar::TValueVec &vcInitialPressures,
                                  const IValueDomainScalar::TValueVec &vcInitialTemperatures);
  virtual bool CreateStageLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase,
                                const IValueDomainScalar::TValueVec &vcInitialPressures,
                                const IValueDomainScalar::TValueVec &vcInitialTemperatures);
  virtual bool WriteBoundaryLoads(TStageMap &mpStages);
  virtual const dia::IMaterial *InterfaceMaterial(const geo::IElement &element) const;

  const CWellZoomInModel &ZoomInModel() const;
  CWellZoomInModel &ZoomInModel();

private:
  bool WriteBoundaryLoadsFromDistributions(TStageMap &mpStages);
  bool WriteBoundaryLoadsFromParentModel(TStageMap &mpStages);
};

#endif // _WELLZOOMINDIANARUNNER_H_
