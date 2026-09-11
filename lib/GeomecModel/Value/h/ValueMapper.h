#ifndef _VALUE_MAPPER_
#define _VALUE_MAPPER_

namespace geo
{
class IElement;
class IElementSet;
class IMesh;
}
class IValueComponentBase;
class IParallelInitializationCallback;
class CFormationBase;
class CWellCasingModel;
class CDepletionStage;
class CModelBase;
class CResultRegister;

#include "ResultTree.h"

#include <map>
#include <set>

// At some point we may want to have these settings per-model, but for development and evaluation a global instance is easier
class CValueMapper
{
  enum eLevelOfParallelization { PARALLEL_NONE, PARALLEL_PARTIAL, PARALLEL_FULL } m_nLevelOfParallization;
  enum eDianaDistributedMapping { DIA_DM_NO, DIA_DM_YES } m_nDianaDistributedMapping;

  CValueMapper();
  ~CValueMapper();
  CValueMapper(const CValueMapper&);
  CValueMapper& operator=(const CValueMapper&);

public:
  static CValueMapper *instance();

  bool UseLegacyCode() const;
  void UseLegacyCode(bool bUseLegacyCode);

  bool AllowParallelMapping() const;
  void AllowParallelMapping(bool bAllowParallelMapping);

  bool DianaDistributedMapping() const;
  void DianaDistributedMapping(bool bDianaDistributedMapping);

  void EnterSections();
  void NextSection();
  void LeaveSections();

  bool PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *valueComponent);
  void FinishMapping();

  // bit flag
  typedef enum {
    NONE = 0,
    MARKED_AS_INITIAL = 1,
    CHECK_CEMENT = 2,
    CHECK_MARKED_VS_INITIAL = 4
  } TPrepareMappingMaterialFlag;

  bool PrepareMappingMaterial(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage, int flag = NONE);
  bool HasUndrainedMaterial(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage);

  typedef enum {
    MAP_PRESSURE = 1,
    MAP_TEMPERATURE = 2,
    MAP_TEMPERATURE_HEAT = 3,
    MAP_VOLUMETRIC_STRAIN = 4
  } TPrepareMappingComponentType;

  bool PrepareMappingComponent(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage, TPrepareMappingComponentType type);

  bool PrepareMappingSurfaces(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage);

  bool PrepareMappingElementSet(const geo::IElementSet *pElementSet, geo::IMesh *pMesh = 0);

  bool PrepareMappingStress(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, bool change);
  bool PrepareMappingStrain(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister);
  bool PrepareMappingPlasticEnergy(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister);
  bool PrepareMappingPorePressure(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, const geo::IElementSet *pElementSet, bool change);
  bool PrepareMappingGammaValues(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, const geo::IElementSet *pElementSet);
  bool PrepareMappingIndex(int index, const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, bool change);

  bool EncounteredStrainOrStress() const;


  void Add(const CFormationVolume *pVolume, CFormationVolume::TThicknessCache *pCache);
  void Add(CResultTree::CScalarResult::CScalarResultComponent *pComponent, CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap *pCache);
  void Add(CResultTree::CScalarResult::CScalarResultComponent *pComponent, CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap *pCache);

private:

  bool m_bEncounteredStrainOrStress;

  typedef std::pair<const CFormationVolume *, CFormationVolume::TThicknessCache *> TFormationVolumeThicknessCachePair;
  typedef std::pair<CResultTree::CScalarResult::CScalarResultComponent *, CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap *> TResultDisplacementCachePair;
  typedef std::pair<CResultTree::CScalarResult::CScalarResultComponent *, CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap *> TResultAvgVolumeCachePair;

  tbb::concurrent_vector<TFormationVolumeThicknessCachePair> m_vcFormationVolumeThicknessCaches;
  tbb::concurrent_vector<TResultDisplacementCachePair> m_vcResultDisplacementCaches;
  tbb::concurrent_vector<TResultAvgVolumeCachePair> m_vcResultAvgVolumeCaches;

  typedef std::pair<const geo::IElement *, bool> TElementPair;

  typedef std::map<const CFormationBase *, TElementPair> TFormationElementMap;
  typedef std::map<const CWellCasingModel *, TElementPair> TCasingModelElementMap;
  typedef std::map<const geo::IElementSet *, bool> TElementSetMap;

  typedef struct {
    TFormationElementMap mpFormationElement;
    TCasingModelElementMap mpCasingModelElement;
    TElementSetMap mpHandledSets;
  } TStageInfo;
  typedef std::map<const CDepletionStage *, TStageInfo> TStageInfoMap;

  TStageInfoMap m_stageInfo;

  const CResultRegister *m_pResultRegister;

  void collectFormationModelElementMaps(const CModelBase& model, const geo::IElementSet& elementSet, const CDepletionStage& stage, bool bDoWellCasing = false);

};



#endif