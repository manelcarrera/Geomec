#ifndef _IMATERIALSERVERTEMPL_H_
#define _IMATERIALSERVERTEMPL_H_

#include "IMaterialServerBase.h"

#include "Material.h"
#include "IMaterial.h"
#include "MaterialParameter.h"
#include "PointSet.h"
#include "ResultRegister.h"
#include "BranchState.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "GlobalMessage.h"
#include "FemAppEntryTypes.h"
#include "IElementSet.h"

//#define MATERIAL_ATTACHED 320
//#define MATERIAL_REMOVED  330

class CValueType;
class CDepletionStage;

namespace geo {
  class IElement;
  class IPoint;
}

#include "ivaluecomponent.h"
#include "resourceIDI.h"
#include "resourceIDP.h"
#include "GlobalMessage.h"
#include "StreamVersion.h"
#include "resourceIDS.h"
#include "ConvexHull.h"
#include "ConvexHull_2D.h"

#include "ivaluecomponent.h"
#include "isMaterialConnected.h"
#include "MaterialMappingCache.h"


template <class MATERIAL, class PARENT>
class IMaterialServerTempl : public IMaterialServerBase
{
public:
	//wjrx mantis 2583
	typedef std::pair<const CValueType*,unsigned int> TValueTypePair;
	typedef std::vector<TValueTypePair> TValueTypePairVec;

private:
  mutable CMaterialMappingCache::TMaterialMap m_mpMaterials;

public:

  virtual void AddToCache(CMaterialKey& key, CFFMaterial *material) const
  {
    m_mpMaterials[key] = material;
  }

private:
	// Members
	TValueTypePairVec m_vcValueType;

public:
  typedef std::set<const CValueType*> TValueTypeSet;
private:
  TValueTypeSet m_stExtrapolatingValueTypes; // any valuetype (distributed property) in here is extrapolated outside the convex hull

  const CDepletionStage& m_stage;
  PARENT& m_parent;
	MATERIAL* m_pMaterial;
	bool m_bLoading;
  mutable QString m_strName;

private:
	void UpdateLinks();
public:
  bool CanConfigureDistributions() const;
private:
  virtual bool MatchingMaterials(const MATERIAL& mat1, const MATERIAL& mat2) const;
  void UpdateDependentServers(enum ModifiedHint uHint);

protected:
	virtual bool IsValueTypeValid(const CValueType& value_type) const;
  void StatusMessage(const QString& strMessage) const;
  void DefaultStatusMessage() const;
  bool IsExtrapolatingValueType(const CValueType& valuetype) const;
  void MapValueTypes(const geo::IElement& element, CFFMaterial::TValueMap& mpValue) const;
  CFFMaterial& FetchFFMaterial(const CFFMaterial::TValueMap& mpValue) const;
  virtual MATERIAL* FindIndexInEntry(int nIndex) const = 0;

public:
	IMaterialServerTempl(PARENT& parent, const CDepletionStage& stage);
	virtual ~IMaterialServerTempl();
  const QString& MaterialName() const;
  bool IsExtrapolatingParameter(unsigned int valueTypeID) const;
  bool IsDistributedProperty(unsigned int valueTypeID) const;
	void Clear() const;
	virtual const QString& Name() const;
	virtual unsigned int IconId() const;
	virtual bool Destroy();
	virtual bool CanDestroy() const;
	virtual	bool ConnectItem(const CGraphNode &item);
	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;
  virtual void OnDragLeave(const CGraphNode& item) const;
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode& node);
	virtual unsigned int TypeId() const;
  virtual const CFFMaterial &Material(const geo::IElement& element, const CFFMaterial::TValueMap& mpValueInit = CFFMaterial::TValueMap()) const;
	virtual const CFFMaterial &Material(const geo::IPoint& point, const CFFMaterial::TValueMap& mpValueInit = CFFMaterial::TValueMap()) const;
  const PARENT& Parent() const;
  PARENT& Parent();
  const CDepletionStage& Stage() const;

	// NB This function returns the material as it is defined in the material library,
	// so NO distributed values; Use Material(element) for distributed values;
	// return 0 if no material has been assigned yet.
	const MATERIAL *LibraryMaterial() const;
	MATERIAL *LibraryMaterial();
  virtual bool Valid() const;
  virtual bool ValidValues() const;
  virtual const IMaterialBase* Material() const;

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet) const;

	// Save and load
	// Stream
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;
	virtual bool Empty() const;
	const TValueTypePairVec& ValueTypePairVec() const;
  TValueTypeSet& getExtrapolatingValueTypes();
  virtual bool ReadOnly() const;
  bool Less(const CGraphNode& rhs) const;
  bool Loading() const;
  //void clearCachingComponents(std::vector <const CValueType*>& valueTypes,
  //  TValueTypeSet& extrapolatingValueTypesCopy);
};

///// implementation

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::UpdateLinks()
{
  std::vector<CValueType*> vcDeleted;
  for(size_t i = 0; i < m_vcValueType.size(); i++)
  {
	  CValueType *pValueType = (CValueType*)(m_vcValueType[i].first);
	  if(pValueType && !IsValueTypeValid(*pValueType))
		  vcDeleted.push_back(pValueType);
  }

  for(size_t i = 0; i < vcDeleted.size(); i++)
	  UnLink(*vcDeleted[i]);
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::CanConfigureDistributions() const
{
  return m_pMaterial != 0 && !ReadOnly() && !m_vcValueType.empty();
}

template <class MATERIAL, class PARENT>
const QString& IMaterialServerTempl<MATERIAL, PARENT>::MaterialName() const
{
  if(m_pMaterial)
	  return m_pMaterial->Name();
	
  return IMaterialServerBase::Name();
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::MatchingMaterials(const MATERIAL& /*mat1*/, const MATERIAL& /*mat2*/) const
{
  return true;
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::UpdateDependentServers(enum ModifiedHint uHint)
{
  const CDepletionStage* pStage = &m_stage.InitialStage();

  while(pStage)
  {
    IMaterialServerTempl* pMatServer = m_parent.ConnectedMaterial(*pStage);
    if(pMatServer)
    {
      if(m_pMaterial && pStage->isAfter(m_stage))
      {
        MATERIAL* pMat = pMatServer->LibraryMaterial();
        if(pMat && !MatchingMaterials(*pMat, *m_pMaterial))
          pMatServer->UnLink(*pMat);
      }

      pMatServer->Modified(uHint);
      if(pMatServer->LibraryMaterial())
        pMatServer->LibraryMaterial()->Modified(uHint);
    }

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::IsValueTypeValid(const CValueType& value_type) const
{
  if(!m_pMaterial)
    return false;

  return m_pMaterial->IsParameter(value_type.TypeId());
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::StatusMessage(const QString& strMessage) const
{
  _m()->status(strMessage);
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::DefaultStatusMessage() const
{
  StatusMessage(getStringTableEntry(AFX_IDS_IDLEMESSAGE));
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::IsExtrapolatingValueType(const CValueType& valuetype) const
{
  return m_stExtrapolatingValueTypes.find(&valuetype) != m_stExtrapolatingValueTypes.end();
}

namespace
{
  typedef struct _Mapping
  {
    bool contained;
    int count;
    bool valid;
    double value;

    _Mapping()
      : contained(false)
      , count(0)
      , valid(false)
      , value(0)
    {
    }
  } Mapping;

  Mapping processValueVector(Mapping& mapping,
    const IValueDomainScalar::TValueVec& vcValues)
  {
    assert(vcValues.size() != 0);

    mapping.valid = true;

    for (size_t j = 0; j < vcValues.size(); j++)
    {
      if (vcValues[j].Valid())
      {
        mapping.count++;
        mapping.value += vcValues[j].Value();
      }
      else
      {
        mapping.valid = false;
        break;
      }
    }

    return mapping;
  }

  template <class H>
    void calculateDistance(double& distance, int& count, double& value,
      unsigned int& valueType, const CValueType* pValueType, H* hull,
      const geo::IElement& element)
  {
    double element2Center = hull->MidPoint().Distance(element.MidPoint());
    double representativeLength = hull->RepresentativeLength();

    if ((distance == 0) ||
      ((element2Center / representativeLength) < distance))
    {
      Mapping mapping;

      processValueVector(mapping,
        pValueType->Component().ScalarData().ValueElement(element));

      if (mapping.valid)
      {
        count = mapping.count;
        distance = element2Center / representativeLength;
        value = mapping.value;
        valueType = pValueType->TypeId();
      }
    }
  }

  class CElSetWrapper
  {
  public:
    CElSetWrapper(const IElementSet& elset)
    : m_elset(elset)
    {
    }

    geo::CPoint MidPoint() const
    {
      return geo::CPoint((Min().X() + Max().X()) / 2.,
                         (Min().Y() + Max().Y()) / 2.,
                         (Min().Z() + Max().Z()) / 2.);
    }

    double RepresentativeLength() const
    {
      return sqrt((Max().X() - Min().X()) * (Max().X() - Min().X()) +
                  (Max().Y() - Min().Y()) * (Max().Y() - Min().Y()) +
                  (Max().Z() - Min().Z()) * (Max().Z() - Min().Z()));
    }

  private:
    const geo::CPoint& Min() const
    {
      if(m_ptMin.Empty())
        m_ptMin = m_elset.ElementSet().Min();

      return m_ptMin;
    }

    const geo::CPoint& Max() const
    {
      if(m_ptMax.Empty())
        m_ptMax = m_elset.ElementSet().Max();

      return m_ptMax;
    }

  private:
    const IElementSet& m_elset;
    mutable geo::CPoint m_ptMin;
    mutable geo::CPoint m_ptMax;
  };

} // anonymous namespace

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::MapValueTypes(const geo::IElement& element, CFFMaterial::TValueMap& mpValue) const
{
  std::map <unsigned int, Mapping> materialMapping;
  std::map <unsigned int, Mapping>::iterator materialMappingIterator;

  for(size_t i = 0; i < m_vcValueType.size(); i++)
  {
	  const CValueType *pValueType = m_vcValueType[i].first;
	  assert(pValueType);

    materialMappingIterator = materialMapping.find(pValueType->TypeId());

    if (materialMappingIterator == materialMapping.end())
    {
      materialMappingIterator =
        (materialMapping.insert(std::make_pair(pValueType->TypeId(), Mapping()))).first;
    }

    // this is a distributed value linked to this material, so
	  // the value from the material library will be replaced by it
    // wedx mantis #2628: if required
    if(pValueType->PointSet().PointInConvexHull(element.MidPoint()))
    {
      materialMappingIterator->second.contained = true;
      processValueVector(materialMappingIterator->second,
        pValueType->Component().ScalarData().ValueElement(element));
    }
  }

  for (materialMappingIterator = materialMapping.begin();
    materialMappingIterator != materialMapping.end(); ++materialMappingIterator)
  {
    if (materialMappingIterator->second.contained)
    {
      if (materialMappingIterator->second.valid &&
        (materialMappingIterator->second.count != 0))
      {
        mpValue.insert(std::make_pair(materialMappingIterator->first,
          materialMappingIterator->second.value / materialMappingIterator->second.count));
      }
    }
    else
    {
      if (IsExtrapolatingParameter(materialMappingIterator->first))
      {
        double distance = 0;
        int count = 0;
        double value = 0;
        unsigned int valueType = 0;

        for (size_t i = 0; i < m_vcValueType.size(); i++)
        {
          const CValueType *pValueType = m_vcValueType[i].first;

          if(pValueType->TypeId() == materialMappingIterator->first)
          {
            const CPointSet* pPointSet = dynamic_cast <const CPointSet*> (&pValueType->PointSet());

            if(pPointSet)
            {
              if ((pPointSet->m_pHull_2D == 0) &&
                (pPointSet->m_pHull_3D == 0))
              {
                pPointSet->CreateHull();
              }

              if (pPointSet->m_pHull_3D != 0)
              {
                calculateDistance(distance, count, value, valueType, pValueType,
                  pPointSet->m_pHull_3D, element);
              }

              if (pPointSet->m_pHull_2D != 0)
              {
                calculateDistance(distance, count, value, valueType, pValueType,
                  pPointSet->m_pHull_2D, element);
              }
            }
            else
            {
              const IElementSet* pElementSet = dynamic_cast<const IElementSet*>(&pValueType->PointSet());
              if(pElementSet)
              {
                CElSetWrapper esw(*pElementSet);
                calculateDistance(distance, count, value, valueType, pValueType, &esw, element);
              }
            }
          }
        }

        if ((valueType != 0) && (count != 0))
        {
          mpValue.insert(std::make_pair(valueType, value / count));
        }
      }
    }
  }
}

template <class MATERIAL, class PARENT>
CFFMaterial& IMaterialServerTempl<MATERIAL, PARENT>::FetchFFMaterial(const CFFMaterial::TValueMap& mpValue) const
{
  CMaterialMappingCache *cache = CMaterialMappingCache::instance();
 
  CFFMaterial::TValueMap *values = const_cast<CFFMaterial::TValueMap *>(&mpValue);

  CMaterialKey key(values);

  if (!cache->NoCaching())
  {
    typename CMaterialMappingCache::TMaterialMap::const_iterator it = m_mpMaterials.find(key);
    if (it != m_mpMaterials.end())
      return *it->second;
  }

  key.SetMap(values);

  CFFMaterial *material = new CFFMaterial(*m_pMaterial, mpValue);

  if (cache->UseCache() || cache->NoCaching())
    cache->Add(this, key, material);
  else
    m_mpMaterials[key] = material;

  return *material;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::PrepareMapping(const geo::IElementSet *pElementSet) const
{
  for(size_t i = 0; i < m_vcValueType.size(); i++)
  {
    const CValueType *pValueType = m_vcValueType[i].first;
	  assert(pValueType);

    CPointSet* pPointSet = dynamic_cast<CPointSet *>(const_cast<IPointSet *>(&pValueType->PointSet()));

    if (pPointSet)
    {
      pPointSet->PrepareMapping(pElementSet);
    }
  }

  return true;
}


template <class MATERIAL, class PARENT>
IMaterialServerTempl<MATERIAL, PARENT>::IMaterialServerTempl(PARENT& parent, const CDepletionStage& stage)
: IMaterialServerBase("NONE", parent.Model()),
  m_stage(stage),
  m_parent(parent),
  m_pMaterial(0),
  m_bLoading(false)
{
}

template <class MATERIAL, class PARENT>
IMaterialServerTempl<MATERIAL, PARENT>::~IMaterialServerTempl()
{
  Clear();

  while (m_vcValueType.size() > 0)
  {
    const CValueType* valueType = m_vcValueType.back().first;
    size_t sizeBefore = m_vcValueType.size();

    UnLink(const_cast<CValueType&>(*valueType));

    if (sizeBefore == m_vcValueType.size())
    {
      m_vcValueType.pop_back();
    }
  }
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::IsExtrapolatingParameter(unsigned int valueTypeID) const
{
  TValueTypeSet::const_iterator it;
  for(it = m_stExtrapolatingValueTypes.begin(); it != m_stExtrapolatingValueTypes.end(); ++it)
  {
    if((*it)->TypeId() == valueTypeID)
      return true;
  }

  return false;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::IsDistributedProperty(unsigned int valueTypeID) const
{
  for(size_t i = 0; i < m_vcValueType.size(); ++i)
  {
    if(m_vcValueType[i].second == valueTypeID)
      return true;
  }
  return false;
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::Clear() const
{
  m_mpMaterials.clear();
}

template <class MATERIAL, class PARENT>
const QString& IMaterialServerTempl<MATERIAL, PARENT>::Name() const
{
  m_strName = m_stage.Name() + " (" + MaterialName() + ")";
  return m_strName;
}

template <class MATERIAL, class PARENT>
unsigned int IMaterialServerTempl<MATERIAL, PARENT>::IconId() const
{
  if(m_pMaterial)
	  return m_pMaterial->IconId();

  if(m_stage.Initial())
    return IDI_MATERIAL_NONE;

  const IMaterialServerTempl& matserver = m_parent.Material(m_stage);
  if(matserver.LibraryMaterial())
    return IDI_MATERIAL_PREV;

  return IDI_MATERIAL_NONE_PREV;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::Destroy()
{
  assert(CanDestroy());
  UnLink(*m_pMaterial);
  assert(m_pMaterial == 0);
  return true;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::CanDestroy() const
{
	return m_pMaterial != 0 && !ReadOnly();
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::ConnectItem(const CGraphNode &item)
{
  assert(CanConnectItem(item));
  const MATERIAL* pMaterial = 0;
  const IMaterialServerBase *pServer1 = dynamic_cast<const IMaterialServerBase*>(&item);
  const CValueType *pValueType = dynamic_cast<const CValueType*>(&item);
  const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&item);

  DefaultStatusMessage();

  if(pServer1)
	  pMaterial = dynamic_cast<const MATERIAL*>(pServer1->Material());
  else
	  pMaterial = dynamic_cast<const MATERIAL*>(&item);

  if(pMaterial)
  {
    if(m_stage.Initial())
    {
      const CDepletionStage* pStage = m_stage.Last() ? 0 : &m_stage.Next();
      while(pStage)
      {
        const IMaterialServerTempl* pServer = m_parent.ConnectedMaterial(*pStage);
        if(pServer)
        {
          const MATERIAL* pMat = pServer->LibraryMaterial();
          if(pMat && !MatchingMaterials(*pMat, *pMaterial))
          {
            // if the user continues, the connected materials for all subsequent servers will be unlinked
            // ask for confirmation
            if(_m()->msg(IDP_CONFIRM_REPLACE_INITIAL_MATERIAL_MODEL, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
              return false; // action aborted
            else
              break;
          }
        }

        if(pStage->Last())
          pStage = 0;
        else
          pStage = &pStage->Next();
      }
    }

	  if(m_pMaterial)
		  m_pMaterial->UnLink(*this);

	  assert(m_pMaterial == 0);

	  LinkTo((MATERIAL&)(*pMaterial));

	  assert(m_pMaterial != 0);
  }

  if(pPointSet)
  {
	  std::vector<CValueType*> vcVT = pPointSet->ValueTypes();
	  for(size_t i = 0; i < vcVT.size(); i++)
	  {
		  if(CanConnectItem(*vcVT[i]))
			  ConnectItem(*vcVT[i]);
	  }
  }

  if(pValueType)
    LinkTo(const_cast<CValueType&>(*pValueType));

  return true;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::CanConnectItem(const CGraphNode &item) const
{
  if(!ReadOnly())
  {
	  // Accepting materials beside our selves
	  const MATERIAL *pMaterial = dynamic_cast<const MATERIAL*>(&item);
	  if(pMaterial)
    {
      if(m_pMaterial == pMaterial)
      {
        StatusMessage("This material is already connected");
        return false;
      }

      if(m_stage.Initial())
      {
        DefaultStatusMessage();
        return true;
      }

      const CModelBase& model = static_cast<const CModelBase&>(Model());
      const IMaterialServerTempl& InitialServer = m_parent.Material(model.InitialDepletionStage());
      if(!InitialServer.LibraryMaterial())
      {
        StatusMessage("First connect a material to the initial depletion stage");
        return false;
      }

      if(!MatchingMaterials(*InitialServer.LibraryMaterial(), *pMaterial))
      {
        StatusMessage("This material does not match with the material connected to the initial depletion stage");
        return false;
      }

      DefaultStatusMessage();
      return true;
    }

	  // Accepting materials servers if they contain material
	  const IMaterialServerBase* pMatServer = dynamic_cast<const IMaterialServerBase*>(&item);
	  if(pMatServer)
	  {
		  if(pMatServer->Material() == 0)
			  return false;

		  return CanConnectItem(*pMatServer->Material());
	  }

	  const CPointSet *pPointSet = dynamic_cast<const CPointSet*> (&item);
	  if(pPointSet)
	  {
		  std::vector<CValueType*> vcVT = pPointSet->ValueTypes();
		  for(size_t i = 0; i < vcVT.size(); i++)
		  {
			  bool bVal = CanConnectItem(*vcVT[i]);
			  if(bVal)
				  return true; // at least one 'material' valuetype present we can connect to
		  }
	  }

    // Accepting property entries
	  const CValueType *pType = dynamic_cast<const CValueType*> (&item);
	  if(pType && m_pMaterial)
	  {
		  if(!IsLinkedTo(item) && IsValueTypeValid(*pType))
      {
        DefaultStatusMessage();
			  return true;
      }
      QString sParamName;
      CMaterialParameter::MatLibXIDFromValueTypeID(pType->TypeId(), sParamName);
      QString msg = QString("Material '%1' does not accept property '%2'").arg(m_pMaterial->Name(), sParamName);
      StatusMessage(msg);
      return false;
	  }

    if (pPointSet)
    {
      StatusMessage(QString("Could not connect to pointset '%1'").arg(pPointSet->Name()));
      return false;
    }
  }

  StatusMessage("The material is read-only");
  return false;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::CanDisconnectItem(const CGraphNode& item) const
{
  if(&item == &m_parent)
    return CanDestroy();

  return !ReadOnly();
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::OnDragLeave(const CGraphNode& /*item*/) const
{
  DefaultStatusMessage();
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::OnNewNeighbour(const CGraphNode &node)
{
  CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
  const MATERIAL *pMaterial = dynamic_cast<const MATERIAL*>(&node);

  if(pMaterial)
  {
    if(m_pMaterial)
      UnLink(*m_pMaterial);

    m_pMaterial = const_cast<MATERIAL*>(pMaterial);
    UpdateLinks();
    Clear();
    pModel->ResultRegister().ClearAll();
    Modified(MATERIAL_ATTACHED);
    m_pMaterial->Modified(MATERIAL_ATTACHED);
    UpdateDependentServers(MATERIAL_ATTACHED);
  }

  if(dynamic_cast<const CValueType*>(&node))
  {
    Clear();
    pModel->ResultRegister().ClearAll();
    //wjrx mantis 2583
    const CValueType *pValueType= dynamic_cast<const CValueType*>(&node);
    assert( pValueType);
    if (pValueType )
    {
      m_vcValueType.push_back(std::make_pair(pValueType, pValueType->TypeId()));

      const CDepletionStageEntry& entry = m_stage.Entry();
      bool isExtrapolatingValueType = false;

      for (CDepletionStageEntry::const_iterator iterator = entry.begin();
        iterator != entry.end(); ++iterator)
      {
        const IMaterialServerTempl* server =
          this->Parent().ConnectedMaterial(*iterator);

        if (server && (server != this))
        {
          isExtrapolatingValueType = isExtrapolatingValueType ||
            server->IsExtrapolatingValueType(*pValueType);
        }
      }

      if (isExtrapolatingValueType)
      {
        m_stExtrapolatingValueTypes.insert(pValueType);
      }
    }

    Modified();
  }

  IMaterialServerBase::OnNewNeighbour(node);
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  CModelBase *pModel = dynamic_cast<CModelBase*> (&Model());
  if(m_pMaterial == &node)
  {
    if(uHint != MATERIAL_REMOVED)
  	  UpdateLinks();
	  if(uHint == MAT_PARAM_CHANGED)
	  {
      if (isMaterialConnected(pModel, m_pMaterial))
      {
        Clear();
        pModel->ResultRegister().ClearAll();
        Modified(uHint);
      }
	  }
    else if(uHint == MATERIAL_ATTACHED || uHint == MATERIAL_REMOVED)
    {
      Modified(uHint);
    }
  }

  if(dynamic_cast<const CValueType*>(&node))
  {
    Clear();
	  pModel->ResultRegister().ClearAll();
	  Modified();
  }

  IMaterialServerBase::OnNeighbourModified(node, uHint);
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::OnNeighbourDeleted(const CGraphNode& node)
{
  // not worth living anymore?
  if(&node == &m_stage || &node == &m_parent)
  {
    delete this;
    return;
  }

  CModelBase *pModel = static_cast<CModelBase*> (&Model());
  if(m_pMaterial == &node)
  {
    MATERIAL* pMat = m_pMaterial;
    m_pMaterial = 0;
    Clear();
    pModel->ResultRegister().ClearAll();
    UpdateLinks();
    Modified(MATERIAL_REMOVED);
    pMat->Modified(MATERIAL_REMOVED);
    UpdateDependentServers(MATERIAL_REMOVED);
  }

  for(size_t i = 0; i < m_vcValueType.size(); i++)
  {
    if(m_vcValueType[i].first == &node)
    {
      Clear();
      if(!pModel->getDeleting())
        pModel->ResultRegister().ClearAll();
       
      // TODO: Delete the following code when a more generic solution has been implemented

      /////////////////////////////////////////////////////////////////////////////////////

      m_stExtrapolatingValueTypes.erase(m_vcValueType[i].first);
      m_vcValueType.erase(m_vcValueType.begin() + i);

      Modified();
    }
  }

  IMaterialServerBase::OnNeighbourDeleted(node);
}

template <class MATERIAL, class PARENT>
unsigned int IMaterialServerTempl<MATERIAL, PARENT>::TypeId() const
{
  return 0;
}

template <class MATERIAL, class PARENT>
const CFFMaterial &IMaterialServerTempl<MATERIAL, PARENT>::Material(const geo::IElement& element, const CFFMaterial::TValueMap& mpValueInit) const
{
  assert(m_pMaterial != 0);

  CFFMaterial::TValueMap mpValue(mpValueInit);
  MapValueTypes(element, mpValue);

  return FetchFFMaterial(mpValue);
}

template <class MATERIAL, class PARENT>
const CFFMaterial &IMaterialServerTempl<MATERIAL, PARENT>::Material(const geo::IPoint& point, const CFFMaterial::TValueMap& mpValueInit) const
{
  assert(m_pMaterial != 0);

  CFFMaterial::TValueMap mpValue(mpValueInit);

  for(size_t i = 0; i < m_vcValueType.size(); i++)
  {
	  const CValueType *pValueType = m_vcValueType[i].first;
	  assert(pValueType);
	  // this is a distributed value linked to this material, so
	  // the value from the material library will be replaced by it

    geo::CValue value = pValueType->Component().ScalarData().ValuePoint(point);
    if(value.Valid() &&
       (m_stExtrapolatingValueTypes.find(pValueType) != m_stExtrapolatingValueTypes.end() ||
        pValueType->PointSet().PointInConvexHull(point)))
    {
	    mpValue.insert(CFFMaterial::TValueMap::value_type(pValueType->TypeId(), value.Value()));
    }
  }

  return FetchFFMaterial(mpValue);
}

template <class MATERIAL, class PARENT>
const PARENT& IMaterialServerTempl<MATERIAL, PARENT>::Parent() const
{
  return m_parent;
}

template <class MATERIAL, class PARENT>
PARENT& IMaterialServerTempl<MATERIAL, PARENT>::Parent()
{
  return m_parent;
}

template <class MATERIAL, class PARENT>
const CDepletionStage& IMaterialServerTempl<MATERIAL, PARENT>::Stage() const
{
  return m_stage;
}

// NB This function returns the material as it is defined in the material library,
// so NO distributed values; Use Material(element) for distributed values;
// return 0 if no material has been assigned yet.
template <class MATERIAL, class PARENT>
const MATERIAL *IMaterialServerTempl<MATERIAL, PARENT>::LibraryMaterial() const
{
  return m_pMaterial;
}

template <class MATERIAL, class PARENT>
MATERIAL *IMaterialServerTempl<MATERIAL, PARENT>::LibraryMaterial()
{
  return m_pMaterial;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::Valid() const
{
  return m_pMaterial != 0;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::ValidValues() const
{
  for (TValueTypePairVec::const_iterator it = m_vcValueType.begin(); it != m_vcValueType.end(); ++it)
  {
    for (size_t i = 0; i < it->first->ComponentSize(); ++i)
    {
      const CValueComponent *pComponent = dynamic_cast<const CValueComponent *>(&it->first->Component((unsigned int)i));
      if (!pComponent->IsInRange(pComponent->ValueSet()))
        return false;
    }
  }

  return true;
}

template <class MATERIAL, class PARENT>
const IMaterialBase* IMaterialServerTempl<MATERIAL, PARENT>::Material() const
{
  return m_pMaterial;
}

// Save and load
// Stream
template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  m_bLoading = true;

  TValueCompositeEntry& composite_entry = (TValueCompositeEntry&)*((CModelBase&)Model()).GraphEntry(MD_BASE_VALUE_COMPOSITE);

  // Load material first
  int nMaterial;
  stream >> nMaterial;
  if(nMaterial != -1)
  {
    MATERIAL* pNode = FindIndexInEntry(nMaterial);
	  if(pNode)
      ConnectItem(*pNode);
  }
  progress.Step();

  // Load the value types
  int nSize;
  stream >> nSize;
  std::map<int, int> mpIndices;
  int valueTypeIndex = 0;
  for(int i = 0; i < nSize; i++)
  {
	  int nProperty;
	  stream >> nProperty;
    IValueComposite* pComposite = composite_entry.FindIndex(nProperty);
    if((pComposite == 0) || CanConnectItem(*pComposite))
    {
      mpIndices.insert(std::map<int, int>::value_type(i, valueTypeIndex++));
      if (pComposite != 0)
      {
        ConnectItem(*pComposite);
      }
      else
      {
        composite_entry.LinkNodeToIndex(*this, nProperty);
      }
    }
    else
    {
      mpIndices.insert(std::map<int, int>::value_type(i, -1));
    }
	  progress.Step();
  }

  if(version > CStreamVersion(3, 0, 98))
  {
    // load extrapolation status
    stream >> nSize;

    for(int i = 0; i < nSize; ++i)
    {
      int nIndex;
      stream >> nIndex;

      valueTypeIndex = (mpIndices.find(nIndex))->second;
      if (valueTypeIndex == -1)
        continue;

      assert(valueTypeIndex >= 0 && valueTypeIndex < m_vcValueType.size());
      m_stExtrapolatingValueTypes.insert(m_vcValueType[valueTypeIndex].first);
    }
  }
  else
  {
    // old default was to extrapolate all
    for(size_t i = 0; i < m_vcValueType.size(); ++i)
      m_stExtrapolatingValueTypes.insert(m_vcValueType[i].first);
  }

  m_bLoading = false;
}

template <class MATERIAL, class PARENT>
void IMaterialServerTempl<MATERIAL, PARENT>::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save attached material index if any else save -1
  if(m_pMaterial)
	  stream << m_pMaterial->Index();
  else 
	  stream << -1;
  progress.Step();

  // Save attached material properties
  std::vector<CValueType*> vcProperties;
  for(size_t i = 0; i < referenceSize(); i++)
  {
	  CValueType* pType = dynamic_cast<CValueType*> (&referenceAt(i));
	  if(pType)
		  vcProperties.push_back(pType);
  }

  std::vector<int> vcExtrapolatingIndices;

  // Save the size and indices
  int nSize = (int)vcProperties.size();
  stream << nSize;
  for(size_t i = 0; i < vcProperties.size(); i++)
  {
	  stream << vcProperties[i]->Index();
	  progress.Step();

    if(m_stExtrapolatingValueTypes.find(vcProperties[i]) != m_stExtrapolatingValueTypes.end())
      vcExtrapolatingIndices.push_back((int)i);
  }

  // save extrapolating indices
  nSize = (int)vcExtrapolatingIndices.size();
  stream << nSize;
  for(int i = 0; i < nSize; ++i)
    stream << vcExtrapolatingIndices[i];
}

template <class MATERIAL, class PARENT>
long IMaterialServerTempl<MATERIAL, PARENT>::SavedItems() const
{
  long lRet = 1;

  for(size_t i = 0; i < referenceSize(); i++)
  {
	  if(dynamic_cast<const CValueType*> (&referenceAt(i)))
		  lRet++;
  }

  return lRet;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::Empty() const
{
  return false;
}

template <class MATERIAL, class PARENT>
const typename IMaterialServerTempl<MATERIAL, PARENT>::TValueTypePairVec& IMaterialServerTempl<MATERIAL, PARENT>::ValueTypePairVec() const
{
  return m_vcValueType;
}

template <class MATERIAL, class PARENT>
typename IMaterialServerTempl<MATERIAL, PARENT>::TValueTypeSet&
  IMaterialServerTempl<MATERIAL, PARENT>::getExtrapolatingValueTypes()
{
  return m_stExtrapolatingValueTypes;
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::ReadOnly() const
{
  // material is read-only in branch models
  return !m_bLoading && (static_cast<const CModelBase&>(Model())).BranchState().IsFixedStage(m_stage);
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::Less(const CGraphNode& rhs) const
{
  const IMaterialServerTempl* pRhs = dynamic_cast<const IMaterialServerTempl*>(&rhs);
  if(!pRhs)
    return IMaterialServerBase::Less(rhs);

  return Stage().isBefore(pRhs->Stage());
}

template <class MATERIAL, class PARENT>
bool IMaterialServerTempl<MATERIAL, PARENT>::Loading() const
{
  return m_bLoading;
}

/*
template <class MATERIAL, class PARENT>
  void IMaterialServerTempl <MATERIAL, PARENT> ::
    clearCachingComponents(std::vector <const CValueType*>& valueTypes,
      TValueTypeSet& extrapolatingValueTypesCopy)
{
  for (std::vector <const CValueType*> ::const_iterator iterator =
    valueTypes.begin(); iterator != valueTypes.end(); ++iterator)
  {
    bool presentInCopy = (extrapolatingValueTypesCopy.find(*iterator) !=
      extrapolatingValueTypesCopy.end());
    bool presentinOriginal = (m_stExtrapolatingValueTypes.find(*iterator) !=
      m_stExtrapolatingValueTypes.end());

    if (presentInCopy != presentinOriginal)
    {
      for (unsigned int mode = 0; mode < (*iterator)->ModeSize(); ++mode)
      {
        for (unsigned int index = 0; index < (*iterator)->ComponentSize(); ++index)
        {
          ICachingComponent* cachingComponent =
            dynamic_cast <ICachingComponent*> (&((const_cast <CValueType*> (
              *iterator))->Component(index, mode)));

          if (cachingComponent)
          {
            cachingComponent->clearCache();
          }
        }
      }
    }
  }
}
*/

#endif // _IMATERIALSERVERTEMPL_H_
