#ifndef _GEOMECMIXTUREDIANARUNNER_H_
#define _GEOMECMIXTUREDIANARUNNER_H_

#include "GeomecDianaRunner.h"

class CGeomecMixtureDianaRunner : public CGeomecDianaRunnerBase
{
public:
  CGeomecMixtureDianaRunner(CMeshBase &meshbase,
                    CModelBase &model,
                    CDianaRunController& controller);

protected:
  virtual bool AllowLinearCalc() const;
  virtual bool OnApplyLoads(dia::CLoadManager &manager);
  virtual bool OnWriteCommands();
  virtual bool OnWriteSupports();
  virtual bool OnWriteTyings();

  virtual int LoadsProgressSize() const;
  virtual int CommandsProgressSize() const;
  virtual int SupportProgressSize() const;
  virtual int TyingsProgressSize() const;

  // for retrieval of interface materials (only if element is an interface, otherwise 0) from derived classes
  virtual const dia::IMaterial* InterfaceMaterial(const geo::IElement &element) const;

  virtual void ElementPressures(const geo::IElement& elm, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcValues) const;

  virtual void WriteModelSpecificLoads(const TStageMap& mpStages);

  void ModifyMaterialForContainment(const dia::IMaterial& mat, const geo::IElement& element) const;

private:
  void FetchFixedPotentialNodes() const;
  void ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages);
  void CreateTimeLoadTables(const TStageMap& mpStages);
  geo::CValue Pressure(const geo::INode& node, const std::set<int>& stAttachedElements, const CDepletionStage& stage) const;
  bool IsInterfaceElement(const geo::IElement& elm) const;
  void WriteElementInitialLoads(const CDepletionStage& stage, const geo::IElement& element, dia::CLoadCase& lcase, const IValueDomainScalar::TValueVec& vcInitialTemperatures);
  void FetchStageTemperatures(const geo::IElement& element, const CDepletionStage& stage, IValueDomainScalar::TValueVec& vcTemperatures);
  void WriteElementStageLoads(const CDepletionStage &stage, const geo::IElement &element, dia::CLoadCase &lcase, const IValueDomainScalar::TValueVec& vcInitialTemperatures);

private:
  mutable CModelBase::TNodeElementsMap m_mpBoundaryNodes;
  mutable CModelBase::TNodeElementsMap m_mpDepletingFormationNodes;
  mutable CModelBase::TNodeElementsMap m_mpDepletingFaultNodes;
  mutable std::set<int> m_stFixedNodes;
};

#endif // _GEOMECMIXTUREDIANARUNNER_H_
