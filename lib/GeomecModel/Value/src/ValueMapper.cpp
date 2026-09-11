#include "ValueMapper.h"
#include "IElementSet.h"
#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "ISettings.h"
#include "ModelBase.h"
#include "WellCasingModel.h"
#include "DepletionStage.h"
#include "Material.h"
#include "FormationBase.h"
#include "WellCasingCement.h"
#include "Pressure.h"
#include "Temperature.h"
#include "VolumetricStrainLoad.h"
#include "MaterialMappingCache.h"
#include "MPKernel.h"
#include "Mesh.h"
#include "Point.h"
#include "IWedge.h"
#include "ITetrahedron.h"
#include "IHexahedron.h"
#include "ResultRegister.h"
#include "ResultCache.h"
#include "MeshDataCacher.h"
#include "StrainLoad.h"

#include <utility>

// The code guarded by this macro definition has been replaced by locks
// and hence pre-caching is no longer necessary; in case we find errors, we can easily turn them back on
//#define PRECACHE_LINES_ETC


CValueMapper::CValueMapper()
  : m_nLevelOfParallization(PARALLEL_FULL)
  , m_nDianaDistributedMapping(DIA_DM_YES)
  , m_bEncounteredStrainOrStress(false)
  , m_pResultRegister(0)
{
}

CValueMapper::~CValueMapper()
{
}

CValueMapper *CValueMapper::instance()
{
  static CValueMapper _instance;
  return &_instance;
}

bool CValueMapper::UseLegacyCode() const
{
  return m_nLevelOfParallization == PARALLEL_NONE;
}

void CValueMapper::UseLegacyCode(bool bUseLegacyCode)
{
  m_nLevelOfParallization = bUseLegacyCode ? PARALLEL_NONE : PARALLEL_PARTIAL;
}

bool CValueMapper::AllowParallelMapping() const
{
  return m_nLevelOfParallization == PARALLEL_FULL;
}

void CValueMapper::AllowParallelMapping(bool bAllowParallelMapping)
{
  m_nLevelOfParallization = bAllowParallelMapping ? PARALLEL_FULL : PARALLEL_PARTIAL;
}

bool CValueMapper::DianaDistributedMapping() const
{
  return m_nDianaDistributedMapping == DIA_DM_YES;
}

void CValueMapper::DianaDistributedMapping(bool bDianaDistributedMapping)
{
  m_nDianaDistributedMapping = bDianaDistributedMapping ? DIA_DM_YES : DIA_DM_NO;
}


void CValueMapper::EnterSections()
{
  // just to make sure
  m_stageInfo.clear();
  m_bEncounteredStrainOrStress = false;
  m_pResultRegister = 0;
}

void CValueMapper::NextSection()
{
  // set the flag to false so that these won't be reinitialized in the Prepare* routines below
  // we can only do this after all initialization has been done
  for (TStageInfoMap::iterator stageIt = m_stageInfo.begin(); stageIt != m_stageInfo.end(); ++stageIt)
  {
    for (TFormationElementMap::iterator it = stageIt->second.mpFormationElement.begin(); it != stageIt->second.mpFormationElement.end(); ++it)
      it->second.second = false;
    for (TCasingModelElementMap::iterator it = stageIt->second.mpCasingModelElement.begin(); it != stageIt->second.mpCasingModelElement.end(); ++it)
      it->second.second = false;
    for (TElementSetMap::iterator it = stageIt->second.mpHandledSets.begin(); it != stageIt->second.mpHandledSets.end(); ++it)
      it->second = false;
  }

  for (size_t i = 0; i < m_vcFormationVolumeThicknessCaches.size(); ++i)
  {
    m_vcFormationVolumeThicknessCaches[i].first->AddToCache(m_vcFormationVolumeThicknessCaches[i].second);
    delete m_vcFormationVolumeThicknessCaches[i].second;
  }
  m_vcFormationVolumeThicknessCaches.clear();

  for (size_t i = 0; i < m_vcResultDisplacementCaches.size(); ++i)
  {
    m_vcResultDisplacementCaches[i].first->AddToCache(m_vcResultDisplacementCaches[i].second);
    delete m_vcResultDisplacementCaches[i].second;
  }
  m_vcResultDisplacementCaches.clear();

  for (size_t i = 0; i < m_vcResultAvgVolumeCaches.size(); ++i)
  {
    m_vcResultAvgVolumeCaches[i].first->AddToCache(m_vcResultAvgVolumeCaches[i].second);
    delete m_vcResultAvgVolumeCaches[i].second;
  }
  m_vcResultAvgVolumeCaches.clear();
}

void CValueMapper::LeaveSections()
{
  // clear up memory
  m_stageInfo.clear();
}

bool CValueMapper::PrepareMapping(const geo::IElementSet *pElementSet, const IValueComponentBase *valueComponent)
{
  bool retval = false;

  geo::ILine::PrepareMapping();
  geo::IWedge::PrepareMapping();
  geo::ITriangle::PrepareMapping();
  geo::ITetrahedron::PrepareMapping();
  geo::IQuadrilateral::PrepareMapping();
  geo::IHexahedron::PrepareMapping();

  if (valueComponent)
  {
    retval = const_cast<IValueComponentBase *>(valueComponent)->PrepareMapping(pElementSet);

    if (dynamic_cast<const IResultComponent *>(valueComponent))
      m_pResultRegister = &static_cast<const IResultComponent *>(valueComponent)->ResultRegister();
  }

  if (retval)
  {
    CMaterialMappingCache::instance()->UseCache(true);
  }

  return retval;
}

void CValueMapper::FinishMapping()
{
  if (m_pResultRegister && m_pResultRegister->cacheExists())
    m_pResultRegister->Cache().ActiveCacher().EndCriticalSection();

  CMaterialMappingCache::instance()->UseCache(false);
}


void CValueMapper::Add(const CFormationVolume *pVolume, CFormationVolume::TThicknessCache *pCache)
{
  m_vcFormationVolumeThicknessCaches.push_back(std::make_pair(pVolume, pCache));
}

void CValueMapper::Add(CResultTree::CScalarResult::CScalarResultComponent *pComponent, CResultTree::CScalarResult::CScalarResultComponent::TDisplacementMap *pCache)
{
  m_vcResultDisplacementCaches.push_back(std::make_pair(pComponent, pCache));
}

void CValueMapper::Add(CResultTree::CScalarResult::CScalarResultComponent *pComponent, CResultTree::CScalarResult::CScalarResultComponent::TFormAvgVolMap *pCache)
{
  m_vcResultAvgVolumeCaches.push_back(std::make_pair(pComponent, pCache));
}

void CValueMapper::collectFormationModelElementMaps(const CModelBase& model, const geo::IElementSet& elementSet, const CDepletionStage& stage, bool bDoWellCasing)
{
  const CFormationBase *pFormation = 0;
  const geo::IElement *pElement = 0;


  TStageInfo *stageInfo = &m_stageInfo.insert(TStageInfoMap::value_type(&stage, TStageInfo())).first->second;


  // check if we already did this
  {
    std::pair<TElementSetMap::iterator, bool> retval = stageInfo->mpHandledSets.insert(TElementSetMap::value_type(&elementSet, false));
    if (retval.second)
      retval.first->second = true;

    if (!retval.first->second)
    {
      if (!bDoWellCasing || stageInfo->mpCasingModelElement.size() > 0) // just to be sure
        return;
    }
  }


  for (int i = 0; i < elementSet.ElementSize(); ++i)
  {
    if (!elementSet.Element(i).IsInterfaceElement())
    {
      pElement = &elementSet.Element(i);
      pFormation = model.Mesh().Formation(*pElement);

      if (pFormation)
      {
        std::pair<TFormationElementMap::iterator, bool> retval = stageInfo->mpFormationElement.insert(TFormationElementMap::value_type(pFormation, TElementPair(pElement, false)));
        if (retval.second)
          retval.first->second.second = true;
      }
      else if (bDoWellCasing)
      {
        const CWellCasingModel* pWellCasingModel = dynamic_cast<const CWellCasingModel*>(&model);
        if (pWellCasingModel)
        {
          std::pair<TCasingModelElementMap::iterator, bool> retval = stageInfo->mpCasingModelElement.insert(TCasingModelElementMap::value_type(pWellCasingModel, TElementPair(pElement, false)));
          if (retval.second)
            retval.first->second.second = true;
        }
      }
    }
  }
}

bool CValueMapper::PrepareMappingMaterial(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage, int flag)
{
  if (!pElementSet)
    pElementSet = &model.Mesh().Mesh();

  collectFormationModelElementMaps(model, *pElementSet, stage, flag & CHECK_CEMENT);

  TStageInfo *stageInfo = &m_stageInfo.find(&stage)->second;

  for (TFormationElementMap::const_iterator it = stageInfo->mpFormationElement.begin(); it != stageInfo->mpFormationElement.end(); ++it)
  {
    if (!it->second.second)
      continue;

    if (flag & MARKED_AS_INITIAL)
    {
      const CDepletionStage& local_stage = model.ResultRegister().DepletionStageEntry().MarkedAsInitialStage();

      {
        const CMaterialServer& matServer = it->first->Material(local_stage);
        matServer.Material(*it->second.first);
        matServer.PrepareMapping(pElementSet);
      }

      if (flag & CHECK_MARKED_VS_INITIAL)
      {
        if (&local_stage != &local_stage.InitialStage())
        {
          const CMaterialServer& matServer = it->first->Material(local_stage.InitialStage());
          matServer.Material(*it->second.first);
          matServer.PrepareMapping(pElementSet);
        }
      }
    }
    const CMaterialServer& matServer = it->first->Material(stage);
    matServer.Material(*it->second.first);
    matServer.PrepareMapping(pElementSet);
  }

  for (TCasingModelElementMap::const_iterator it = stageInfo->mpCasingModelElement.begin(); it != stageInfo->mpCasingModelElement.end(); ++it)
  {
    if (!it->second.second)
      continue;

    const CMaterialServer& matServer = it->first->CasingCement().Material(stage);
    matServer.Material(*it->second.first);
    matServer.PrepareMapping(pElementSet);
  }

  return true;
}

bool CValueMapper::HasUndrainedMaterial(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage)
{
  if (!pElementSet)
    return false;

  collectFormationModelElementMaps(model, *pElementSet, stage);

  TStageInfo *stageInfo = &m_stageInfo.find(&stage)->second;

  bool retval = false;

  for (TFormationElementMap::const_iterator it = stageInfo->mpFormationElement.begin(); it != stageInfo->mpFormationElement.end(); ++it)
  {
    if (it->first->Material(stage).MaterialModel() == MM_UNDRAINED)
      retval = true;
  }

  return retval;
}

bool CValueMapper::PrepareMappingComponent(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage, TPrepareMappingComponentType type)
{
  if (!pElementSet)
    return true;

  collectFormationModelElementMaps(model, *pElementSet, stage);

  TStageInfo *stageInfo = &m_stageInfo.find(&stage)->second;

  bool retval = true;

  for (TFormationElementMap::const_iterator it = stageInfo->mpFormationElement.begin(); it != stageInfo->mpFormationElement.end(); ++it)
  {
    if (!it->second.second)
      continue;

    switch (type)
    {
    case MAP_PRESSURE:
      if (!const_cast<IValueComponentBase&>(it->first->Pressure(stage).Component()).PrepareMapping(pElementSet))
        retval = false;
      break;
    case MAP_TEMPERATURE:
      if (!const_cast<IValueComponentBase&>(it->first->EffectiveTemperature(stage).Component()).PrepareMapping(pElementSet))
        retval = false;
      break;
    case MAP_TEMPERATURE_HEAT:
      if (!const_cast<IValueComponentBase&>(it->first->UserTemperature(stage).Component()).PrepareMapping(pElementSet))
        retval = false;
      break;
    case MAP_VOLUMETRIC_STRAIN:
      if (!const_cast<IValueComponentBase&>(it->first->Strain(stage).Component()).PrepareMapping(pElementSet))
        retval = false;
      break;
    }
  }

  return retval;
}

bool CValueMapper::PrepareMappingSurfaces(const CModelBase& model, const geo::IElementSet *pElementSet, const CDepletionStage& stage)
{
  if (!pElementSet)
    return true;

  collectFormationModelElementMaps(model, *pElementSet, stage);

  TStageInfo *stageInfo = &m_stageInfo.find(&stage)->second;

  /*bool retval = true;*/

  geo::CPoint top, bottom;

  for (TFormationElementMap::const_iterator it = stageInfo->mpFormationElement.begin(); it != stageInfo->mpFormationElement.end(); ++it)
  {
    if (!it->second.second)
      continue;

    for (size_t i = 0; i < it->first->ElementSetSize(); ++i)
    {
      const CFormationVolume *pVolume = dynamic_cast<const CFormationVolume *>(&it->first->ElementSet(i));
      if (pVolume)
      {
        pVolume->Min();
        pVolume->Max();
#ifdef PRECACHE_LINES_ETC
        for (size_t j = 0; j < pVolume->Volume().SideSurfaceSize(); ++j)
        {
          const geo::CBodyGroup::CSideSurface& surface = pVolume->Volume().SideSurface(j);
          surface.Candidates(it->second.first->Point(0));

          for (size_t k = 0; k < surface.FaceSize(); ++k)
          {
            surface.Face(k).Line(0);
          }
        }
#endif
      }
    }
  }

#ifdef PRECACHE_LINES_ETC
  if (dynamic_cast<const geo::ISurface *>(pElementSet))
  {
    const geo::ISurface *pSurface = static_cast<const geo::ISurface *>(pElementSet);

    pSurface->Candidates(geo::CPoint(0, 0, 0));

    for (size_t k = 0; k < pSurface->FaceSize(); ++k)
    {
      pSurface->Face(k).Line(0);
    }
  }
#endif

#ifdef PRECACHE_LINES_ETC
  if (stageInfo->mpHandledSets.find(pElementSet)->second)
  {
    for (size_t i = 0; i < pElementSet->ElementSize(); ++i)
    {
      if (pElementSet->Element(i).IsBody())
      {
        const geo::IBody& body = static_cast<const geo::IBody&>(pElementSet->Element(i));
        for (size_t j = 0; j < body.NrOfFaces(); ++j)
          body.Face(j).Line(0);
      }
    }
  }
#endif

  return true;
}



namespace {

template<bool CheckForBody>
class CTaskFillCache
{
  const geo::IElementSet *m_pElementSet;

public:
  CTaskFillCache(const geo::IElementSet *pElementSet) : m_pElementSet(pElementSet) {}

  MP_DUMMY_DATA

    size_t size() const { return m_pElementSet->ElementSize(); }

  void operator()(const tbb::blocked_range<size_t>& r) const;
};

template<>
void CTaskFillCache<false>::operator()(const tbb::blocked_range<size_t>& r) const
{
  for (size_t i = r.begin(); i != r.end(); ++i)
  {
    m_pElementSet->Element(i).Min();
    m_pElementSet->Element(i).Max();
    m_pElementSet->Element(i).MidPoint();
  }
}

template<>
void CTaskFillCache<true>::operator()(const tbb::blocked_range<size_t>& r) const
{
  for (size_t i = r.begin(); i != r.end(); ++i)
  {
    if (m_pElementSet->Element(i).IsBody())
    {
      m_pElementSet->Element(i).Min();
      m_pElementSet->Element(i).Max();
      m_pElementSet->Element(i).MidPoint();
    }
  }
}
typedef CTaskFillCache<true> CTaskFillCacheCheck;
typedef CTaskFillCache<false> CTaskFillCacheNoCheck;

}

bool CValueMapper::PrepareMappingElementSet(const geo::IElementSet *pElementSet, geo::IMesh *pMesh)
{
  if (!pElementSet)
    return true;

  for (int i = 0; i < pElementSet->ElementSize(); ++i)
  {
    if (pElementSet->Element(i).IsBody())
    {
      const geo::IBody *pBody = static_cast<const geo::IBody *>(&pElementSet->Element(i));

      assert(pBody);

      geo::ICacheInterface &ci = const_cast<geo::IElementSet *>(pElementSet)->CacheInterface();

      if (pMesh)
      {
        if (ci.Cached(pMesh))
          return true;

        // apparently executed for the side-effects, cached is not used anymore

        /* geo::CElementCacheObject *cached = */ ci.ElementCacheObject(pMesh, pBody);
      }

      break;
    }
  }

  CTaskFillCacheCheck taskFillCacheCheck(pElementSet);

  mp::CKernelParallel<CTaskFillCacheCheck> parKernelCheck;

  parKernelCheck.execute(taskFillCacheCheck);

  if (pMesh)
  {
    CTaskFillCacheNoCheck taskFillCacheNoCheck(pMesh);

    mp::CKernelParallel<CTaskFillCacheNoCheck> parKernelNoCheck;

    parKernelNoCheck.execute(taskFillCacheNoCheck);

#ifdef PRECACHE_LINES_ETC
    for (size_t i = 0; i < pMesh->ElementSize(); ++i)
    {
      if (pMesh->Element(i).IsBody())
      {
        const geo::IBody& body = static_cast<const geo::IBody&>(pMesh->Element(i));
        for (size_t j = 0; j < body.NrOfFaces(); ++j)
        {
          const geo::IFace& face = body.Face(j);
          for (size_t k = 0; k < face.NrOfLines(); ++k)
          {
            face.Line(k).Length();
          }
        }
      }
    }
#endif
  }

  // Consider the cache to be thread-safe only in the experimental settings
  return CValueMapper::instance()->AllowParallelMapping() ? true : false;
}



bool CValueMapper::PrepareMappingStress(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, bool change)
{
  bool retval = true;

  m_bEncounteredStrainOrStress = true;

  if (change)
  {
    int c = resultRegister.ColumnNumber(component.AnalysisType(), resultRegister.DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRESS_XX);

    if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
      retval = false;
  }

  int c = resultRegister.ColumnNumber(component.AnalysisType(), component.Stage().Index(), CI_STRESS_XX);

  if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  if (!PrepareMappingIndex(CI_POREPRES, model, component, resultRegister, change))
    retval = false;

  return retval;
}

bool CValueMapper::PrepareMappingStrain(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister)
{
  bool retval = true;

  m_bEncounteredStrainOrStress = true;

  // Strains always pick up the initial values

  int c = resultRegister.ColumnNumber(component.AnalysisType(), resultRegister.DepletionStageEntry().MarkedAsInitialStage().Index(), CI_STRAIN_XX);

  if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  c = resultRegister.ColumnNumber(component.AnalysisType(), component.Stage().Index(), CI_STRAIN_XX);

  if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  if (!PrepareMappingIndex(CI_POREPRES, model, component, resultRegister, true))
    retval = false;

  return retval;
}

bool CValueMapper::PrepareMappingPlasticEnergy(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister)
{
  bool retval = true;

  const CDepletionStage *pStage = &component.Stage();

  while (pStage && !pStage->Initial())
  {
    int c = resultRegister.ColumnNumber(component.AnalysisType(), pStage->Index(), CI_STRESS_XX);
    if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
      retval = false;

    c = resultRegister.ColumnNumber(component.AnalysisType(), pStage->Index(), CI_PSTRAIN_XX);
    if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
      retval = false;

    if (!pStage->Initial())
      pStage = &pStage->Previous();
    else
      pStage = 0;
  }

  int c = resultRegister.ColumnNumber(component.AnalysisType(), resultRegister.DepletionStageEntry().MarkedAsInitialStage().Index(), CI_PSTRAIN_XX);
  if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 6))
    retval = false;

  if (!PrepareMappingStress(model, component, resultRegister, true))
    retval = false;

  return retval;
}

bool CValueMapper::PrepareMappingPorePressure(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, const geo::IElementSet *pElementSet, bool change)
{
  bool retval = true;

  if (!PrepareMappingIndex(CI_POREPRES, model, component, resultRegister, change))
    retval = false;

  CValueMapper *vm = CValueMapper::instance();

  if (vm->HasUndrainedMaterial(model, pElementSet, component.Stage()))
  {
    if (!PrepareMappingStress(model, component, resultRegister, true))
      retval = false;

    if (!PrepareMappingIndex(CI_TEMPERATURE, model, component, resultRegister, true))
      retval = false;

    if (!vm->PrepareMappingMaterial(model, pElementSet, component.Stage(), CValueMapper::MARKED_AS_INITIAL))
      retval = false;
  }

  return retval;
}

bool CValueMapper::PrepareMappingGammaValues(const CModelBase& model, IResultComponent& component, CResultRegister& resultRegister, const geo::IElementSet *pElementSet)
{
  bool retval = true;

  if (!PrepareMappingStress(model, component, resultRegister, true))
    retval = false;

  if (!PrepareMappingPorePressure(model, component, resultRegister, pElementSet, true))
    retval = false;

  return retval;
}

bool CValueMapper::PrepareMappingIndex(int index, const CModelBase& /*model*/, IResultComponent& component, CResultRegister &resultRegister, bool change)
{
  bool retval = true;

  if (change)
  {
    int c = resultRegister.ColumnNumber(component.AnalysisType(), resultRegister.DepletionStageEntry().MarkedAsInitialStage().Index(), index);

    if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 1))
      retval = false;
  }

  int c = resultRegister.ColumnNumber(component.AnalysisType(), component.Stage().Index(), index);

  if (!resultRegister.Cache().ActiveCacher().StartCriticalSection(c, 1))
    retval = false;

  return retval;
}


bool CValueMapper::EncounteredStrainOrStress() const
{
  return m_bEncounteredStrainOrStress;
}