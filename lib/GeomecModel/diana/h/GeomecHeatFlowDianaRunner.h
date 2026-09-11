#ifndef _GEOMECHEATFLOWDIANARUNNER_H_
#define _GEOMECHEATFLOWDIANARUNNER_H_

#include "GeomecDianaRunner.h"

class CGeomecHeatFlowDianaRunner : public CGeomecDianaRunnerBase {
public:
  CGeomecHeatFlowDianaRunner(CMeshBase &meshbase, CModelBase &model, CDianaRunController &controller);

protected:
  typedef std::map<const CDepletionStage *, dia::CBoundaCase *> TStageMap;

  virtual bool AllowLinearCalc() const;
  virtual bool OnApplyLoads(dia::CLoadManager &manager);
  virtual bool OnWriteCommands();
  virtual bool OnWriteSupports();
  virtual bool OnWriteTyings();
  virtual dia::ITemperatureTable *OnCreateTemperatureTable() const { return 0; }
  virtual dia::IConcentrationTable *OnCreateConcentrationTable() const { return 0; }

  virtual int LoadsProgressSize() const;
  virtual int CommandsProgressSize() const;
  virtual int SupportProgressSize() const;
  virtual int TyingsProgressSize() const;

  // for retrieval of interface materials (only if element is an interface, otherwise 0) from derived classes
  virtual const dia::IMaterial *InterfaceMaterial(const geo::IElement &element) const;
  virtual void WriteModelSpecificLoads(const TStageMap &mpStages);

private:
  void FetchFixedPotentialNodes() const;
  void ProcessStages(dia::CLoadManager &manager, TStageMap &mpStages);
  void CreateNodalTemperatures(const TStageMap &mpStages, CModelBase::TNodeElementsMap &mpNodeElements,
                               CModelBase::TNodeElementsMap *pmpNodeElementsExclude);
  geo::CValue GetAverageTemperatureInNode(int iNode, const std::set<int> &stElements,
                                          const CDepletionStage &stage) const;
  void CreateTimeBoundaTables(const TStageMap &mpStages);

private:
  mutable CModelBase::TNodeElementsMap m_mpBoundaryNodes;
  mutable CModelBase::TNodeElementsMap m_mpDepletingFormationNodes;
  mutable std::set<int> m_stFixedNodes;
};

#endif // _GEOMECHEATFLOWDIANARUNNER_H_
