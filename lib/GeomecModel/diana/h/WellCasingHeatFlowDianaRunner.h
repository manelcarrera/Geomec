#ifndef _WELLCASINGHEATFLOWDIANARUNNER_H_
#define _WELLCASINGHEATFLOWDIANARUNNER_H_

#include "GeomecHeatFlowDianaRunner.h"

class CWellCasingModel;

class CWellCasingHeatFlowDianaRunner : public CGeomecHeatFlowDianaRunner
{
public:
  CWellCasingHeatFlowDianaRunner(CWellCasingModel& model, CDianaRunController& controller);

  const dia::IMaterial& Material(const geo::IElement &element) const;
	virtual const dia::IMaterial* InterfaceMaterial(const geo::IElement &element) const;

  const CWellCasingModel& CasingModel() const;
  CWellCasingModel& CasingModel();

  virtual bool TranslateElementsToXYOrigin() const { return true; }

protected:
  virtual int LoadsProgressSize() const;
  virtual void WriteModelSpecificLoads(IProgressBase& progress, const TStageMap& mpStages);

private:
  bool ParentLinearResults() const;
};

#endif // _WELLCASINGHEATFLOWDIANARUNNER_H_
