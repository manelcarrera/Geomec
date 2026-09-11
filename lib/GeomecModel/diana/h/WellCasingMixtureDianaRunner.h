#ifndef _WELLCASINGMIXTUREDIANARUNNER_H_
#define _WELLCASINGMIXTUREDIANARUNNER_H_

#include "GeomecMixtureDianaRunner.h"

class CWellCasingModel;

class CWellCasingMixtureDianaRunner : public CGeomecMixtureDianaRunner
{
private:
	typedef std::map<const CDepletionStage*, dia::CLoadCase*> TStageMap;

public:
  CWellCasingMixtureDianaRunner(CWellCasingModel& model, CDianaRunController& controller);

  const dia::IMaterial& Material(const geo::IElement &element) const;
	virtual const dia::IMaterial* InterfaceMaterial(const geo::IElement &element) const;

  const CWellCasingModel& CasingModel() const;
  CWellCasingModel& CasingModel();

  virtual bool HasTemperatureValue(const geo::IElement& element) const;
  virtual bool HasConcentrationValue(const geo::IElement& element) const;
  virtual void ElementPressures(const geo::IElement& elm, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcValues) const;
  virtual IValueDomainScalar::TValueVec ElementTemperatures(const geo::IElement& element, const CDepletionStage& stage) const;
  virtual IValueDomainScalar::TValueVec ElementConcentrations(const geo::IElement& element, const CDepletionStage& stage) const;

  virtual bool TranslateElementsToXYOrigin() const { return true; }

protected:
  virtual void WriteModelSpecificLoads(const TStageMap& mpStages);
  virtual void CreateStrainLoad(const geo::IElement& elm, const CDepletionStage& stage, dia::CLoadCase& lcase);
	virtual bool IsZoomInModel() const { return true; }
  virtual void executeCommandInGeomec() const;
  virtual bool CheckConvergenceMinimumVolume() const { return false; }

private:
	virtual bool WriteBoundaryLoads(const TStageMap &mpStages);
  bool ParentLinearResults() const;
};

#endif // _WELLCASINGMIXTUREDIANARUNNER_H_
