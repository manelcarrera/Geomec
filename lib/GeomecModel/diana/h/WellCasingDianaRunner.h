#ifndef _WELLCASINGDIANARUNNER_H_
#define _WELLCASINGDIANARUNNER_H_

#include "IWellModelDianaRunner.h"

class CWellCasingModel;

class CWellCasingDianaRunner : public IWellModelDianaRunner
{
public:
  CWellCasingDianaRunner(CWellCasingModel& model, CDianaRunController& controller);
  virtual ~CWellCasingDianaRunner();

  virtual bool FetchInitialPressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialPressures);
  void ElementPressures(const geo::IElement& elm, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcValues);
  virtual bool FetchInitialTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcInitialTemperatures);
  virtual bool CreateInitialLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
  virtual bool CreateStageLoads(const geo::IElement &element, const CDepletionStage &stage, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec &vcInitialPressures, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
  virtual bool WriteBoundaryLoads(TStageMap &mpStages);
  virtual const dia::IMaterial* InterfaceMaterial(const geo::IElement &element) const;
  virtual const dia::IMaterial &Material(const geo::IElement &element) const;
  virtual bool HasTemperatureValue(const geo::IElement& element) const;
  virtual bool HasConcentrationValue(const geo::IElement& element) const;

  const CWellCasingModel& CasingModel() const;
  CWellCasingModel& CasingModel();

protected:
  virtual void CreateStrainLoad(const geo::IElement& elm, const CDepletionStage& stage, dia::CLoadCase& lcase);
  virtual int LoadsProgressSize() const;
  virtual void FetchStagePressures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcPressures);
  virtual void FetchStageTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcTemperatures);
  virtual void executeCommandInGeomec() const;

  virtual TSolver Solver() const;

private:
  // a map from nodes to the interpolated stresses in the parent mesh results
  typedef std::map<const geo::INode*, std::vector<double> > TInterpolatedStressMap;
  TInterpolatedStressMap m_mpInterpolatedStress;
};

#endif // _WELLCASINGDIANARUNNER_H_
