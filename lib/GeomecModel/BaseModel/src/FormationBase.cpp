// FormationBase.cpp: implementation of the CFormationBase class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "FormationBase.h"
#include "BaseEntryTypes.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "PropertyExcel.h"
#include "PropertyFile.h"
#include "GeoSurface.h"
#include "HorizonBase.h"
#include "Temperature.h"
#include "CalculatedTemperature.h"
#include "VolumetricStrainLoad.h"
#include "StrainLoad.h"
#include "ValueTypeFactory.h"
#include "ValueTensor.h"

#include <algorithm>

#include "DepletionStage.h"
#include "FemAppEntryTypes.h"
#include "StreamVersion.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "FieldFactor.h"
#include "ModifiedHint.h"

///////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IFormationElementSet::IFormationElementSet(CFormationBase& formation, bool bDoLink)
: IElementSet(formation.Name().toStdString().c_str(), formation.Model(), CQuantity::SI_UNIT, CQuantity::SI_UNIT),
  m_pFormation(&formation),
  m_pElementCenters(0)
{
  // We link to the formation and to the mesher, because we have to update when the mesh changes ...
//	reParent(&formation);
  if (bDoLink)
  {
  CModelBase& model = dynamic_cast<CModelBase&>(formation.Model());
  LinkTo(model.Mesh());
  }

  // search for: /* From IFormationElementSet constructor */ in the code

}

IFormationElementSet::IFormationElementSet(const IFormationElementSet& rhs)
: IElementSet(rhs),
  m_pFormation(rhs.m_pFormation),
  m_pElementCenters(0)
{
}

IFormationElementSet::~IFormationElementSet()
{
  ClearElementCenterCache();
}

bool IFormationElementSet::operator==(const IFormationElementSet& rhs) const
{
  if(!IElementSet::operator==(rhs))
    return false;

  return true;
}

IFormationElementSet& IFormationElementSet::operator=(const IFormationElementSet& rhs)
{
  IElementSet::operator =(rhs);
  m_pFormation = rhs.m_pFormation;
  return *this;
}

unsigned int IFormationElementSet::IconId() const
{
  assert(IsCopy());
  return Source().IconId();
}

unsigned int IFormationElementSet::TypeId() const
{
  assert(IsCopy());
  return Source().TypeId();
}

IFormationElementSet::DIMENSION IFormationElementSet::Dimension() const
{
  assert(IsCopy());
  const IFormationElementSet& element_set = dynamic_cast<const IFormationElementSet&>(Source());
  return element_set.Dimension();

}

const geo::IElementSet &IFormationElementSet::ElementSet() const
{
  assert(IsCopy());
  const IFormationElementSet& element_set = dynamic_cast<const IFormationElementSet&>(Source());
  return element_set.ElementSet();
}

geo::IElementSet &IFormationElementSet::ElementSet()
{
  assert(IsCopy());
  IFormationElementSet& element_set = dynamic_cast<IFormationElementSet&>(Source());
  return element_set.ElementSet();
}

const CFormationBase& IFormationElementSet::Formation() const
{
  return *m_pFormation;
}

CFormationBase& IFormationElementSet::Formation()
{
  return *m_pFormation;
}

void IFormationElementSet::OnNewNeighbour(const CGraphNode& node)
{
  if(dynamic_cast<const CFormationBase*>(&node))
    m_pFormation = (CFormationBase*)(&node);
  IElementSet::OnNewNeighbour(node);
}

void IFormationElementSet::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  CModelBase& model = dynamic_cast<CModelBase&>(m_pFormation->Model());
  if(&node == &model.Mesh() && uHint == MeshCleared)
  ClearElementCenterCache();
}

void IFormationElementSet::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // Unlink from the node entries
  UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));
  UnLink(*Model().GraphEntry(MD_BASE_COLOR_NODE));

  IElementSet::LoadStream(stream, version, progress);
  if( CStreamVersion(3, 0, 76) < version && version < CStreamVersion(3, 0, 90) ) {
    // used to load 'initial pressure' flag herer
    int tmp;
    stream >> tmp;
  }
}

void IFormationElementSet::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IElementSet::SaveStream(stream, progress);
}

int IFormationElementSet::Resemblance(const TGraphNodeSet& identifier) const
{
  std::vector<CGraphNode*> vcRet;
  TGraphNodeSet m_identifier = Identifier();
  std::set_intersection(m_identifier.begin(), m_identifier.end(), 
              identifier.begin(), identifier.end(),
              std::back_inserter(vcRet));
  return vcRet.size();
}

TGraphNodeSet IFormationElementSet::Identifier() const
{
  assert(false);
  return TGraphNodeSet();
}

void IFormationElementSet::CreateElementCenters() const
{
  assert(m_pElementCenters == 0);

  m_pElementCenters = new geo::CArray<geo::CElementPoint>;

  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  if(model.IsMesh())
  {
    int j;
    for(j = 0; j < ElementSet().ElementSize(); ++j)
    {
      geo::CElementPoint point = ElementSet().Element(j).MidPoint();
      m_pElementCenters->PushBack(point);
    }
  }
}

void IFormationElementSet::ClearElementCenterCache() const
{
  if(m_pElementCenters)
  {
    ((IFormationElementSet*)this)->Modified();
    delete m_pElementCenters;
    m_pElementCenters = 0;
  }
}

const geo::IObject& IFormationElementSet::ElementCenterPoints() const
{
  if(!m_pElementCenters)
  CreateElementCenters();

  assert(m_pElementCenters);
  return *m_pElementCenters;
}


/*!
\class CFormationBase
\brief Representing the geomechanical layers in the ground

Material properties and a pore pressure distribution have to be assigned to each formation 
and in that way indirectly to the IFormationElementSet of the formation. When the mesh is 
invalidated the elements from the IFormationElementSet are deleted but the formations and
IFormationElementSet remain present in the model. If the mesh is regenerated due to a model
change the bodies are regenerated and assigned in an intelligent way to prevent unnecessary 
loss of body assignment. 
\sa IMaterial
\sa CPressure
\sa CMaterialServer
\sa IFormationElementSet
*/
CFormationBase::CFormationBase(CFemAppModel &model)
  : TBase(model)
  , m_MinThickness(1, 0.01, 100000, CQuantity::SI_UNIT)
  , m_pParent(0)
  , m_FormationType(FT_NOTSPECIFIED)
  , m_formationElementOrder(dynamic_cast <const CModelBase&> (model), true)
{
  m_pThickness = new CFormationThickness(*this, model);
}

CFormationBase::CFormationBase(const CFormationBase& rhs)
  : TBase(rhs)
  , m_vcElementSet(rhs.m_vcElementSet)
  , m_mpPressure(rhs.m_mpPressure)
  , m_mpFractureMatrixPressure(rhs.m_mpFractureMatrixPressure)
  , m_mpTemperature(rhs.m_mpTemperature)
  , m_mpCalcTemper(rhs.m_mpCalcTemper)
  , m_MinThickness(rhs.m_MinThickness.Value(), 0.01, 100000, CQuantity::SI_UNIT)
  , m_pParent(rhs.m_pParent)
  , m_FormationType(rhs.m_FormationType)
  , m_pThickness(rhs.m_pThickness)
  , m_formationElementOrder(rhs.m_formationElementOrder)
{
}

CFormationBase::CFormationBase(const QString &strName, CModelBase& model)
  : TBase(strName, model)
  , m_MinThickness(1, 0.01, 100000, CQuantity::SI_UNIT)
  , m_pParent(0)
  , m_FormationType(FT_NOTSPECIFIED)
  , m_formationElementOrder(model, true)
{
  // Add pressures and temperatures...
  CreatePressuresAndTemperatures();

  // Link to the formation base entry
  TFormationBaseEntry *pFormationEntry = dynamic_cast<TFormationBaseEntry*>(model.GraphEntry(MD_BASE_FORMATION));
  assert(pFormationEntry);		// Should be there
  LinkTo(*pFormationEntry);

  m_pThickness = new CFormationThickness(*this, model);
}

void CFormationBase::CreatePressuresAndTemperatures()
{
  // Get depletionstage entry ....
  CDepletionStageEntry& entry = static_cast<CDepletionStageEntry&>(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  for(CDepletionStageEntry::iterator it = entry.begin(); it != entry.end(); ++it)
  {
    CPressure *pPressure = new CPressure(*this, *it);
    assert(IsLinkedTo(*pPressure));
    assert(m_mpPressure.find(&*it) != m_mpPressure.end());

  assert(pPressure != 0);
    CFractureMatrixPressure *pFractureMatrixPressure = new CFractureMatrixPressure(pPressure, *this, *it);
    assert(IsLinkedTo(*pFractureMatrixPressure));
    assert(m_mpFractureMatrixPressure.find(&*it) != m_mpFractureMatrixPressure.end());

    CTemperature* pTemperature = new CTemperature(*this, *it);
    assert(IsLinkedTo(*pTemperature));
    assert(m_mpTemperature.find(&*it) != m_mpTemperature.end());

  CCalculatedTemperature* pCalcTemper = new CCalculatedTemperature(*this, *it);
  assert(IsLinkedTo(*pCalcTemper));
  assert(m_mpCalcTemper.find(&*it) != m_mpCalcTemper.end());

  if(!it->Initial())
  {
      CStrainLoad* pStrain = new CStrainLoad(*(new CNormalStrainType()), *this, *it, false);
      assert(IsLinkedTo(*pStrain));
      assert(m_mpStrains.find(&*it) != m_mpStrains.end());
  }
  }
}

namespace
{

template <typename LOAD, typename LOAD_MAP>
  void unLinkLoads(CFormationBase& formation, LOAD_MAP& loadMap)
{
  std::vector <LOAD*> loads;

  for (typename LOAD_MAP::iterator iterator = loadMap.begin();
  iterator != loadMap.end(); ++iterator)
  {
  loads.push_back(iterator->second);
  }

  for (typename std::vector <LOAD*> ::iterator load = loads.begin(),
  end = loads.end(); load != end; ++load)
  {
  (*load)->UnLink(formation);
  }
}

} // anonymous namespace

CFormationBase::~CFormationBase()
{
  if(!IsCopy())
  {
  unLinkLoads <CPressure, TPressureMap> (*this, m_mpPressure);
  unLinkLoads <CFractureMatrixPressure, TFractureMatrixPressureMap> (*this,
      m_mpFractureMatrixPressure);
  unLinkLoads <CTemperature, TTemperatureMap> (*this, m_mpTemperature);
  unLinkLoads <CCalculatedTemperature, TCalcTemperMap> (*this,
      m_mpCalcTemper);
  unLinkLoads <CStrainLoad, TStrainMap> (*this, m_mpStrains);

/*
  std::vector<CMaterialServer*> vcMaterials;
  TMaterialMap::iterator itm;
  for(itm = m_mpMaterial.begin(); itm != m_mpMaterial.end(); ++itm)
      vcMaterials.push_back(itm->second);

  for(size_t i = 0; i < vcMaterials.size(); ++i)
      vcMaterials[i]->UnLink(*this);
*/
  }
}

CFormationBase& CFormationBase::operator=(const CFormationBase &rhs)
{
  TBase::operator=(rhs);

  m_mpPressure = rhs.m_mpPressure;
  m_mpFractureMatrixPressure = rhs.m_mpFractureMatrixPressure;
  m_mpTemperature = rhs.m_mpTemperature;
  m_mpCalcTemper = rhs.m_mpCalcTemper;
  m_FormationType = rhs.m_FormationType;
  m_vcElementSet = rhs.m_vcElementSet;
  m_MinThickness.Value(rhs.m_MinThickness.Value(), CQuantity::SI_UNIT);
  m_pParent = rhs.m_pParent;
  m_formationElementOrder = rhs.m_formationElementOrder;
  m_min = rhs.m_min;
  m_max = rhs.m_max;

  return *this;
}

const CFormationBase *CFormationBase::ParentFormation() const
{
  return m_pParent;
}

void CFormationBase::FindParentFormation()
{
  if (Model().parentModel() && !m_pParent)
  {
  TFormationBaseEntry::TNodeSet stNodes = static_cast<TFormationBaseEntry *>(static_cast<CModelBase *>(Model().parentModel())->GraphEntry(MD_BASE_FORMATION))->EntryNodes();

  CFormationBase *parent = 0;
  size_t parentCount = 0;

  for (TFormationBaseEntry::TNodeSet::iterator formationIt = stNodes.begin(); formationIt != stNodes.end(); ++formationIt)
  {
      size_t count = 0;

      for (size_t eltSet = 0; eltSet < m_vcElementSet.size(); ++eltSet)
      {
    size_t step = 1;
    size_t size = m_vcElementSet[eltSet]->PointSize();

    if (size > 100)
          step = size / 100;

    for (size_t i = 0; i < size; i += step)
    {
          if ((*formationIt)->Contains(m_vcElementSet[eltSet]->PointAt(i), false))
      ++count;
    }
      }

      if (count > parentCount)
      {
    parentCount = count;
    parent = *formationIt;
      }
  }

  m_pParent = parent;
  }
}


bool CFormationBase::ShowCenterPoints() const
{
  assert(IsCopy());

  const CFormationBase* pSource = dynamic_cast<const CFormationBase*>(&Source());
  if(pSource)
    return pSource->ShowCenterPoints();

  return false;
}

CFormationBase::TFormationType CFormationBase::FormationType() const
{
  return m_FormationType;
}

void CFormationBase::FormationType(CFormationBase::TFormationType newtype)
{
  m_FormationType=newtype;
}


bool CFormationBase::operator==(const CFormationBase &rhs) const
{
  if(!TBase::operator==(rhs))
    return false;


  return (m_FormationType == rhs.m_FormationType &&
    m_MinThickness.Value() == rhs.m_MinThickness.Value());
}

void CFormationBase::Export(const QString& fileName, const IExportFormat::CExportArg& arg)
{
      if(fileName.right(4).compare(".xls", Qt::CaseInsensitive) == 0 || fileName.right(5).left(4).compare(".xls", Qt::CaseInsensitive) == 0)
      {
        CElementSetExportFormat format(*this, CElementSetExportFormat::TDataVec(), CElementSetExportFormat::TDataVec(), arg );
        CElementSetExcelFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create( eProgress::Geo, fileName ));
          file.Write(fileName.toStdString().c_str(), *prog);
        } catch (CProgressCancel* e)
        {
          delete e;
        }
      }
      else
      {
        CElementSetExportFormat format(*this, CElementSetExportFormat::TDataVec(), CElementSetExportFormat::TDataVec(), arg );
        CElementSetFile file(format);
        std::auto_ptr <IProgressBase> prog;
        try {
          prog.reset(_g->prog()->create( eProgress::Geo, "" ));
          file.Save(fileName, *prog);
        } catch (CProgressCancel* e)
        {
          delete e;
          file.Close();
        }
      }
}

bool CFormationBase::CanExport() const
{
  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  return model.IsMesh() == true;
}

void CFormationBase::OnNeighbourDeleted(const CGraphNode &node)
{
  for(size_t i = 0; i < m_vcElementSet.size(); i++)
  {
    if(m_vcElementSet[i] == &node)
    {
      TElementSetVec::iterator it = m_vcElementSet.begin() + i;
      m_vcElementSet.erase(it);
      Modified(GEOMETRY_CHANGED);
      return;
    }
  }

  // Is there a pressure deleted
  for(TPressureMap::iterator it = m_mpPressure.begin(); it != m_mpPressure.end(); it++)
  {
    if(it->second == &node)
    {
      m_mpPressure.erase(it);
      TBase::OnNeighbourDeleted(node);
      return;
    }
  }

  // Is there a fracture matrix pressure deleted
  for(TFractureMatrixPressureMap::iterator itd = m_mpFractureMatrixPressure.begin(); itd != m_mpFractureMatrixPressure.end(); itd++)
  {
    if(itd->second == &node)
    {
      m_mpFractureMatrixPressure.erase(itd);
      TBase::OnNeighbourDeleted(node);
      return;
    }
  }

  // Is there a temperature deleted
  for(TTemperatureMap::iterator itt = m_mpTemperature.begin(); itt != m_mpTemperature.end(); ++itt)
  {
    if(itt->second == &node)
    {
      m_mpTemperature.erase(itt);
      TBase::OnNeighbourDeleted(node);
      return;
    }
  }

  for(TCalcTemperMap::iterator itc = m_mpCalcTemper.begin(); itc != m_mpCalcTemper.end(); ++itc)
  {
  if(itc->second == &node)
  {
      m_mpCalcTemper.erase(itc);
      TBase::OnNeighbourDeleted(node);
      return;
  }
  }

  for(TStrainMap::iterator itv = m_mpStrains.begin(); itv != m_mpStrains.end(); ++itv)
  {
  if(itv->second == &node)
  {
      m_mpStrains.erase(itv);
      TBase::OnNeighbourDeleted(node);
      return;
  }
  }
/*
  TMaterialMap::iterator itm;
  for(itm = m_mpMaterial.begin(); itm != m_mpMaterial.end(); ++itm)
  {
  if(itm->second == &node)
  {
      m_mpMaterial.erase(itm);
      TBase::OnNeighbourDeleted(node);
      return;
  }
  }
*/
  TBase::OnNeighbourDeleted(node);
}

void CFormationBase::OnNewNeighbour(const CGraphNode& node)
{
  const CPressure* pPressure = dynamic_cast<const CPressure*>(&node);
  const CFractureMatrixPressure* pFractureMatrixPressure = dynamic_cast<const CFractureMatrixPressure*>(&node);
  const CTemperature* pTemperature = dynamic_cast<const CTemperature*>(&node);
  const CCalculatedTemperature* pCalcTemper = dynamic_cast<const CCalculatedTemperature*>(&node);
  const CStrainLoad* pStrain = dynamic_cast<const CStrainLoad*>(&node);
  const IFormationElementSet* pElementSet = dynamic_cast<const IFormationElementSet*>(&node);
//  const CMaterialServer* pMatServer = dynamic_cast<const CMaterialServer*>(&node);

  if(pPressure)
  {
    VERIFY(m_mpPressure.insert(TPressureMap::value_type(&pPressure->DepletionStage(), (CPressure*)(pPressure))).second);
    assert(pPressure->Formation() == this);
  }

  if(pFractureMatrixPressure)
  {
    VERIFY(m_mpFractureMatrixPressure.insert(TFractureMatrixPressureMap::value_type(&pFractureMatrixPressure->DepletionStage(), (CFractureMatrixPressure*)(pFractureMatrixPressure))).second);
    assert(pFractureMatrixPressure->Formation() == this);
  }

  if(pTemperature)
  {
    VERIFY(m_mpTemperature.insert(TTemperatureMap::value_type(&pTemperature->DepletionStage(), (CTemperature*)(pTemperature))).second);
    assert(pTemperature->Formation() == this);
  }

  if(pCalcTemper)
  {
  VERIFY(m_mpCalcTemper.insert(TCalcTemperMap::value_type(&pCalcTemper->DepletionStage(), (CCalculatedTemperature*)(pCalcTemper))).second);
  assert(pCalcTemper->Formation() == this);
  }

  if(pStrain)
  {
  VERIFY(m_mpStrains.insert(TStrainMap::value_type(&pStrain->DepletionStage(), (CStrainLoad*)(pStrain))).second);
  assert(pStrain->Formation() == this);
  }

  if(pElementSet)
  {
    m_vcElementSet.push_back(const_cast<IFormationElementSet*>(pElementSet));
  Modified(GEOMETRY_CHANGED);
  }
/*
  if(pMatServer)
  {
  VERIFY(m_mpMaterial.insert(TMaterialMap::value_type(&pMatServer->Stage(), const_cast<CMaterialServer*>(pMatServer))).second);
  }
*/
  TBase::OnNewNeighbour(node);
}

/*
 * The assumption is that the numerous maps contain at least two items,
 * an initial stage at 0, stage 1 at 1. Another assumption is that the
 * CDepletionStageEntry::iterator is guaranteed to walk all depletion stages
 * consecutively.
 * The CDepletionStageEntry::iterator walks over all depletion stages including
 * the recently added stages. The numerous maps do not yet contain the recently
 * added depletion stages!
 */

void CFormationBase::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) 
{
  bool bModified = false;
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  CDepletionStageEntry& entry = (CDepletionStageEntry&)(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  assert(IsLinkedTo(entry));
  if(&entry == &node && !model.Loading())
  {
  CDepletionStageEntry::iterator previous = entry.end();

  for(CDepletionStageEntry::iterator itd = entry.begin(); itd != entry.end(); ++itd)
    {
      CPressure *pPressure = 0;

      if(m_mpPressure.find(&*itd) == m_mpPressure.end())
      {
        // Create new one ...
        pPressure = new CPressure(*this, *itd);	// Default "Same as previous"
    assert(pPressure->IsLinkedTo(*itd));
        assert(pPressure == m_mpPressure.find(&*itd)->second);
    if (m_mpPressure.find(&*previous) != m_mpPressure.end())
    {
          pPressure->DistributedOnly(m_mpPressure.find(&*previous)->
      second->DistributedOnly());
    }
        bModified = true;
      }

      if(m_mpFractureMatrixPressure.find(&*itd) == m_mpFractureMatrixPressure.end())
      {
        // Create new one ...
    assert(pPressure != 0);
        CFractureMatrixPressure *pFractureMatrixPressure = new CFractureMatrixPressure(pPressure, *this, *itd);	// Default "Same as previous"
    assert(pFractureMatrixPressure->IsLinkedTo(*itd));
        assert(pFractureMatrixPressure == m_mpFractureMatrixPressure.find(&*itd)->second);
    if (m_mpFractureMatrixPressure.find(&*previous) != m_mpFractureMatrixPressure.end())
    {
          pFractureMatrixPressure->DistributedOnly(m_mpFractureMatrixPressure.find(&*previous)->
      second->DistributedOnly());
    }
        bModified = true;
      }

      if(m_mpTemperature.find(&*itd) == m_mpTemperature.end())
      {
        // Create new one ...
        CTemperature* pTemperature = new CTemperature(*this, *itd);
        assert(pTemperature == m_mpTemperature.find(&*itd)->second);
    if (m_mpTemperature.find(&*previous) != m_mpTemperature.end())
    {
          pTemperature->DistributedOnly(m_mpTemperature.find(&*previous)->
      second->DistributedOnly());
    }
        bModified = true;
      }

      if(m_mpCalcTemper.find(&*itd) == m_mpCalcTemper.end())
      {
    // Create new one ...
    CCalculatedTemperature* pCalcTemper = new CCalculatedTemperature(*this, *itd);
    assert(pCalcTemper == m_mpCalcTemper.find(&*itd)->second);
    bModified = true;
      }

      if(m_mpStrains.find(&*itd) == m_mpStrains.end() && !itd->Initial())
      {
    CStrainLoad* pStrain = new CStrainLoad(*(new CNormalStrainType()), *this, *itd, false);
    assert(pStrain == m_mpStrains.find(&*itd)->second);
    if (m_mpStrains.find(&*previous) != m_mpStrains.end())
    {
          pStrain->DistributedOnly(m_mpStrains.find(&*previous)->
      second->DistributedOnly());
    }
    bModified = true;
      }

      previous = itd;

/*
      TMaterialMap::iterator it = m_mpMaterial.find(&*itd);
      if(it == m_mpMaterial.end())
      {
    // is it an initial or after-branch stage?
    if(HasMaterial(*itd))
    {
          CMaterialServer* pMatServer = new CMaterialServer(*this, *itd); // create material server for it
          bModified = true;
          if(!itd->Last())
          {
      TMaterialMap::iterator itnext = m_mpMaterial.find(&itd->Next());
      if(itnext != m_mpMaterial.end())
      {
              CMaterialServer* pNextServer = itnext->second;
              if(pNextServer->LibraryMaterial())
              {
        pMatServer->LinkTo(*pNextServer->LibraryMaterial());
        const CMaterialServer::TValueTypePairVec vcValueTypes = pNextServer->ValueTypePairVec();
        int i;
        for(size_t i = 0; i < vcValueTypes.size(); ++i)
                  pMatServer->LinkTo((CValueType&)*vcValueTypes[i].first);
              }
      }
          }
    }
      }
      else
      {
    // is it not an initial or after-branch stage anymore?
    if(!HasMaterial(*itd))
    {
          it->second->UnLink(*this); // destroy the material server
          bModified = true;
    }
      }
*/
    }
  }

  for(size_t i = 0; i < m_vcElementSet.size(); i++)
  {
    if(m_vcElementSet[i] == &node)
    {
      bModified = true;
    }
  }

  if (bModified)
  {
  if (uHint == GEOMETRY_CHANGED)
      CalculateBoundingBox();
  Modified(uHint);
  }

  TBase::OnNeighbourModified(node, uHint);
}

bool CFormationBase::CanDestroy() const
{
  return ElementSetSize() == 0 && !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();
}

unsigned int CFormationBase::IconId() const
{
  if(Depleting())
    return IDI_DEPLETING_FORMATION;

  return IDI_FORMATION;
}

unsigned int CFormationBase::TypeId() const
{
  return IDT_TREE_FORMATIONS;
}

QString CFormationBase::TypeName() const
{
  return getStringTableEntry(IDS_TREE_FORMATIONS);
}

CPressure& CFormationBase::Pressure(const CDepletionStage &dep)
{
  assert(m_mpPressure.find(&dep) != m_mpPressure.end());
  return *m_mpPressure.find(&dep)->second;
}

  
const CPressure& CFormationBase::Pressure(const CDepletionStage &dep) const
{
  assert(m_mpPressure.find(&dep) != m_mpPressure.end());
  return *m_mpPressure.find(&dep)->second;
}

CFractureMatrixPressure& CFormationBase::FractureMatrixPressure(const CDepletionStage &dep)
{
  return const_cast <CFractureMatrixPressure&> (
  static_cast <const CFormationBase&> (*this).FractureMatrixPressure(dep));
}

  
const CFractureMatrixPressure& CFormationBase::FractureMatrixPressure(const CDepletionStage &dep) const
{
  assert(m_mpFractureMatrixPressure.find(&dep) != m_mpFractureMatrixPressure.end());
  return *m_mpFractureMatrixPressure.find(&dep)->second;
}

CTemperature& CFormationBase::UserTemperature(const CDepletionStage& dep)
{
  assert(m_mpTemperature.find(&dep) != m_mpTemperature.end());
  return *m_mpTemperature.find(&dep)->second;
}

const CTemperature& CFormationBase::UserTemperature(const CDepletionStage& dep) const
{
  assert(m_mpTemperature.find(&dep) != m_mpTemperature.end());
  return *m_mpTemperature.find(&dep)->second;
}

CCalculatedTemperature& CFormationBase::CalculatedTemperature(const CDepletionStage& stage)
{
  return const_cast <CCalculatedTemperature&> (
  static_cast <const CFormationBase&> (*this).CalculatedTemperature(stage));
}

const CCalculatedTemperature& CFormationBase::CalculatedTemperature(const CDepletionStage& stage) const
{
  assert(m_mpCalcTemper.find(&stage) != m_mpCalcTemper.end());
  return *m_mpCalcTemper.find(&stage)->second;
}

CStrainLoad& CFormationBase::Strain(const CDepletionStage& stage)
{
  assert(m_mpStrains.find(&stage) != m_mpStrains.end());
  return (CStrainLoad&)(*m_mpStrains.find(&stage)->second);
}


const CStrainLoad& CFormationBase::Strain(const CDepletionStage& stage) const
{
  assert(m_mpStrains.find(&stage) != m_mpStrains.end());
  return (CStrainLoad&)(*m_mpStrains.find(&stage)->second);
}

/*
  For a detailed explanation, please see the heading "Avoid Duplication in
  const and Non-const Member Function," on p. 23, in Item 3 "Use const whenever
  possible," in Effective C++, 3d ed by Scott Meyers, ISBN-13: 9780321334879.
*/

IValueComposite& CFormationBase::EffectiveTemperature(const CDepletionStage& stage)
{
  return const_cast <IValueComposite&> (
  static_cast <const CFormationBase&> (*this).EffectiveTemperature(stage));
}

const IValueComposite& CFormationBase::EffectiveTemperature(const CDepletionStage& stage) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(!model.CanUseCalculatedTemperatures() || !CalculatedTemperature(stage).Component().Defined())
  return UserTemperature(stage);

  return CalculatedTemperature(stage);
}

bool CFormationBase::Depleting() const
{
  return PressureChange() ||
         TemperatureChange() ||
         StrainChange();
}

bool CFormationBase::PressureChange() const
{
  for(TPressureMap::const_iterator itp = m_mpPressure.begin(); itp != m_mpPressure.end(); ++itp)
  {
    const CPressure *pPressure = itp->second;
    assert(pPressure);
    if(((pPressure->Type() != CPressure::REPEAT_MODE) || (pPressure->DistributedSize() != 0)) && (!pPressure->DepletionStage().Initial()))
    {
      return true;
    }
  }

  return false;
}

bool CFormationBase::TemperatureChange() const
{
  for(TTemperatureMap::const_iterator itt = m_mpTemperature.begin(); itt != m_mpTemperature.end(); ++itt)
  {
    const CTemperature *pTemperature = itt->second;
    assert(pTemperature);
    if(((pTemperature->Type() != CTemperature::REPEAT_MODE) || (pTemperature->DistributedSize() != 0)) && (!pTemperature->DepletionStage().Initial()))
    {
      return true;
    }
  }

  return false;
}

bool CFormationBase::StrainChange() const
{
  for(TStrainMap::const_iterator it = m_mpStrains.begin(); it != m_mpStrains.end(); ++it)
  {
  const CStrainLoad* pStrainLoad = it->second;
  assert(pStrainLoad);
  if(pStrainLoad->Type() != CStrainLoad::REPEAT_MODE || pStrainLoad->DistributedSize() != 0)
      return true;
  }

  return false;
}

int CFormationBase::ElementSetSize() const
{
  return m_vcElementSet.size();
}

const geo::IObject& CFormationBase::DisplayList(int nIndex) const
{
  int nFloor = 0;

  for(int i = 0; i < ElementSetSize(); i++)
  {
    if(nIndex < (nFloor + ElementSet(i).DisplayListSize()))
      return ElementSet(i).DisplayList(nIndex - nFloor);
    nFloor += ElementSet(i).DisplayListSize();
  }

  assert(false);
  const geo::IObject* pObject = 0;
  return *pObject;
}

int CFormationBase::DisplayListSize() const
{
  // in default view, redirect displaylists from formation elementsets
  int nRet = 0;
  for(int i = 0; i < ElementSetSize(); i++)
    nRet += ElementSet(i).DisplayListSize();

  return nRet;
}

const IFormationElementSet& CFormationBase::ElementSet(int nIndex) const
{
  return *m_vcElementSet[nIndex];
}

IFormationElementSet& CFormationBase::ElementSet(int nIndex)
{
  return const_cast<IFormationElementSet&>(const_cast<const CFormationBase *>(this)->ElementSet(nIndex));
}
/*
bool CFormationBase::HasMaterial(const CDepletionStage& stage) const
{
  return (stage.Initial() || stage.Previous().OutputType() == CDepletionStage::BRANCH || stage.Previous().OutputType() == CDepletionStage::PHASE);
}

CMaterialServer& CFormationBase::Material(const CDepletionStage& stage)
{
  CMaterialServer* pMat = ConnectedMaterial(stage);
  if(pMat && (pMat->LibraryMaterial() || stage.Initial()))
  return *pMat;

  assert(!stage.Initial());
  return Material(stage.Previous());
}

const CMaterialServer& CFormationBase::Material(const CDepletionStage& stage) const
{
  const CMaterialServer* pMat = ConnectedMaterial(stage);
  if(pMat && (pMat->LibraryMaterial() || stage.Initial()))
  return *pMat;

  assert(!stage.Initial());
  return Material(stage.Previous());
}

CMaterialServer* CFormationBase::ConnectedMaterial(const CDepletionStage& stage)
{
  TMaterialMap::iterator it = m_mpMaterial.find(&stage);
  if(it != m_mpMaterial.end())
    return it->second;

  return 0;
}
 
const CMaterialServer* CFormationBase::ConnectedMaterial(const CDepletionStage& stage) const
{
  TMaterialMap::const_iterator it = m_mpMaterial.find(&stage);
  if(it != m_mpMaterial.end())
    return it->second;

  return 0;
}
*/
CMaterialServer& CFormationBase::ActiveMaterial()
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return Material(model.BranchState().ActiveStage());
}

const CMaterialServer& CFormationBase::ActiveMaterial() const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return Material(model.BranchState().ActiveStage());
}

CFormationThickness& CFormationBase::FormationThickness()
{
  assert(m_pThickness);
  return *m_pThickness;
}

const CFormationThickness& CFormationBase::FormationThickness() const
{
  assert(m_pThickness);
  return *m_pThickness;
}

// Stream
void CFormationBase::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  // Link to depletionstage entry
  LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  // Load base
  TBase::LoadStream(stream, version, progress);

  // Load pressures
  int nTimeStepSize;
  stream >> nTimeStepSize;

  bool bShowVolStrainWarning = false;

  // Add pressures ...
  CDepletionStage *pStage = &((CModelBase&)Model()).InitialDepletionStage();
  while(pStage)
  {
    CPressure* pPressure = new CPressure(*this, *pStage, true);
    pPressure->LoadStream(stream, version, progress);

  assert(pPressure != 0);

    if(version < CStreamVersion(4, 1, 9))
    {
      // old version, create default pressure
      new CFractureMatrixPressure(pPressure, *this, *pStage, false);
    }
    else
    {
      // create empty fracture matrix pressure and load it
      CFractureMatrixPressure* pFractureMatrixPressure = new CFractureMatrixPressure(pPressure, *this, *pStage, true);
      pFractureMatrixPressure->LoadStream(stream, version, progress);
    }

    if(version < CStreamVersion(3, 0, 86))
    {
      // old version, create default temperature
      new CTemperature(*this, *pStage, false);
    }
    else
    {
      // create empty temperature and load it
      CTemperature* pTemperature = new CTemperature(*this, *pStage, true);
      pTemperature->LoadStream(stream, version, progress);
    }

  // create calculated temperature
  CCalculatedTemperature* pCalcTemper = new CCalculatedTemperature(*this, *pStage);
  assert(IsLinkedTo(*pCalcTemper));
  assert(m_mpCalcTemper.find(pStage) != m_mpCalcTemper.end());

  CVolumetricStrainLoad* pVolStrainLoad = 0;
  if(version < CStreamVersion(4, 1, 46))
  {
      // load and convert old volumetric strain loads

      if(version < CStreamVersion(3, 7, 2))
      {
    // old version, do nothing
      }
      else if(version < CStreamVersion(3, 7, 3))
      {
    // this version stored a CVolumetricStrainLoad for the initial stage as well
    if(pStage->Initial())
    {
          // create, load and destroy
          CVolumetricStrainLoad* p = new CVolumetricStrainLoad(*this, *pStage, true);
          p->LoadStream(stream, version, progress);
          delete p;
    }
    else
    {
          // load and convert
          pVolStrainLoad = new CVolumetricStrainLoad(*this, *pStage, true);
          pVolStrainLoad->LoadStream(stream, version, progress);
    }
      }
      else
      {
    if(!pStage->Initial())
    {
          // load and convert
          pVolStrainLoad = new CVolumetricStrainLoad(*this, *pStage, true);
          pVolStrainLoad->LoadStream(stream, version, progress);
    }
      }
  }

  if(!pStage->Initial())
  {
      CStrainLoad* pStrainLoad = new CStrainLoad(*(new CNormalStrainType()), *this, *pStage, true /*0, CDoubleQuantity::SI_UNIT*/);
      pStrainLoad->LoadStream(stream, version, progress);
      // TODO: memory management; as we only have a few types, we can change this to template or singleton

      // convert old style volumetric strain load
      if(pVolStrainLoad)
      {
    if(pVolStrainLoad->Type() == CVolumetricStrainLoad::CONSTANT_MODE && pVolStrainLoad->Constant().Defined())
    {
          if(pStrainLoad->Type() == CStrainLoad::CONSTANT_MODE && pStrainLoad->IsVolumetric())
          {
      // don't overwrite, issue warning
      bShowVolStrainWarning = true;
          }
          else
          {
      double dValue = pVolStrainLoad->Constant().ReferenceValue().Value();
      pStrainLoad->Constant().Set(dValue, 0, 0);
      pStrainLoad->Constant().UseVolumetric(true);
      pStrainLoad->Type(CStrainLoad::CONSTANT_MODE);
          }
    }

    for(size_t i = 0; i < pVolStrainLoad->DistributedSize(); ++i)
    {
          TVolumetricStrain& distvolstrain = const_cast<TVolumetricStrain&>(pVolStrainLoad->DistributedValue(i));
          pStrainLoad->LinkTo(distvolstrain);
    }

    delete pVolStrainLoad;
      }
  }

  if(version < CStreamVersion(3, 7, 9))
      LoadMaterial(*pStage, stream, version, progress);

  if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
    nTimeStepSize--;
  }
  assert(nTimeStepSize == 0);

  if(bShowVolStrainWarning)
  {
  _m()->msg("At least one 'old style' volumetric strain definition has been ignored because a 'new style' volumetric strain definition is also present for that stage");
  }

  // Create material and load it
  if(version < CStreamVersion(3, 0, 101))
  {
  CMaterialServer* pMatServer = ConnectedMaterial(((CModelBase&)Model()).InitialDepletionStage());
  assert(pMatServer);
  pMatServer->LoadStream(stream, version, progress);
//    TMaterialMap::iterator it = m_mpMaterial.find(&((CModelBase&)Model()).InitialDepletionStage());
//    assert(it != m_mpMaterial.end());
//	  it->second->LoadStream(stream, version, progress);
  }

  if(version < CStreamVersion(0,9,1))
  {
    m_FormationType=FT_NOTSPECIFIED;
  }
  else
  {
    int nFormationType;
    stream >> nFormationType;
    m_FormationType=(CFormationBase::TFormationType)nFormationType;
  }

  if(CStreamVersion(3, 0, 43) < version && version < CStreamVersion(3, 0, 88))
  {
    // formation fluid type was saved
    int iDummy;
    stream >> iDummy;
  }

  if(CStreamVersion(3, 0, 56) < version)
  {
    double dValue;
    stream >> dValue;
    MinThickness(dValue, CQuantity::SI_UNIT);
  }

  m_formationElementOrder.loadStream(stream, version, progress);

  // Link to the formation base entry
  assert(Model().GraphEntry(MD_BASE_FORMATION));		// Should be there
  LinkTo(*Model().GraphEntry(MD_BASE_FORMATION));

}

void CFormationBase::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save base
  TBase::SaveStream(stream, progress);

  // Save pressure
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  int nTimeStepSize = pEntry->EntryNodes().size();
  stream << nTimeStepSize;

  // Now walk depletionstages and save every pressure
  CDepletionStage *pStage = &((CModelBase&)Model()).InitialDepletionStage();
  while(pStage)
  {
    Pressure(*pStage).SaveStream(stream, progress);
    FractureMatrixPressure(*pStage).SaveStream(stream, progress);
    UserTemperature(*pStage).SaveStream(stream, progress);

  if(!pStage->Initial())
      Strain(*pStage).SaveStream(stream, progress);

    if(pStage->Last())
      pStage = 0;
    else
      pStage  = &pStage->Next();
  }

  int nFormationType = m_FormationType;  //from version 0.9.1
  stream << nFormationType;

  stream << m_MinThickness.Value();

  m_formationElementOrder.saveStream(stream, progress);
}

long CFormationBase::SavedItems() const
{
  long lRet = TBase::SavedItems();

  // Save pressure
  CDepletionStage *pStage = &((CModelBase&)Model()).InitialDepletionStage();
  while(pStage)
  {
    lRet += Pressure(*pStage).SavedItems();
    lRet += FractureMatrixPressure(*pStage).SavedItems();
  lRet += UserTemperature(*pStage).SavedItems();

    if(pStage->Last())
      pStage = 0;
    else
      pStage  = &pStage->Next();
  }

  return lRet;
}

// This function will return true if a point in one of the volumes does not have a value assigned to it 
// for either the material or pressure (NAN; not a number)
bool CFormationBase::InvalidValue(const CDepletionStage &dep, eValueType type) const
{
  int i, j;
  const CPressure &PresComposite = Pressure(dep);

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(!model.Mesh().IsMesh())
  return true;
  	
  std::vector<geo::CValue> val;

  switch(type)
  {
  case PRESSURE:
    {
      if(PresComposite.DistributedSize() == 0)
        return false;	// It is only possible for a point to have NAN as a value when only a distributed
                // pressure is used.

      for(i = 0; i < ElementSetSize(); i++)
      {
        const IFormationElementSet& elementset = ElementSet(i);
    const geo::IElementSet& eltset = elementset.ElementSet();
    int size = eltset.ElementSize();

        for(j = 0; j < size; j++)
        {
          const geo::IElement& elt = eltset.Element(j);

          if (elt.IsInterfaceElement())
      continue;

          for(size_t k = 0; k < PresComposite.DistributedSize(); k++)
          {
            TPressure PresCompos = PresComposite.DistributedValue(k);
            IValueComponentBase &PresComponent = PresCompos.Component();
      val = PresComponent.ScalarData().ValueElement(elt);

      for (int l = 0; l < val.size(); ++l)
              if(val[l].Representation() == "NAN")
        return true;
          }
        }
      }
      break;
    }
  default:
    assert(false);
  }

  return false;
}

bool CFormationBase::CanDisconnectItem(const CGraphNode& item) const
{
  const IFormationElementSet* pElementSet = dynamic_cast<const IFormationElementSet*>(&item);
  if(pElementSet)
    return !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

  const IPointSet *pPointSet = dynamic_cast<const IPointSet *> (&item);
  if(pPointSet)
    return !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

  return TBase::CanDisconnectItem(item);
}

int CFormationBase::StageIndexFromName(const QString& strName) const
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return model.DepletionStageEntry().StageIndexFromName(strName);
}

bool CFormationBase::CanConnectItem(const CGraphNode& item) const
{
  // We only accept formation element set's of other 
  const IFormationElementSet* pElementSet = dynamic_cast<const IFormationElementSet*>(&item);
  if(pElementSet)
  {
    if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
      return false;

  if (IsCopy() || pElementSet->IsCopy())
      return false;

    // Do already contain the volume
    for(int i = 0; i < ElementSetSize(); i++)
    {
      if(pElementSet == &ElementSet(i))
        return false;
    }

    return true;
  }

  const IPointSet *pPointSet = dynamic_cast<const IPointSet *> (&item);
  if(pPointSet)
  {
    if((static_cast<const CModelBase&>(Model())).BranchState().IsBranch())
      return false;

    for(int i = 0; i < pPointSet->ValueSetSize(); i++)
    {
      if(pPointSet->ValueSet(i).Component())
      {
    const IValueComposite& parent = pPointSet->ValueSet(i).Component()->Parent();
    int idxParentStage = StageIndexFromName(parent.Name().toStdString().c_str());

    if(dynamic_cast<const TPressure*>        (&parent) ||
           dynamic_cast<const TTemperature*>     (&parent) ||
           dynamic_cast<const TVolumetricStrain*>(&parent) ||
           dynamic_cast<const TLateralStrain*>   (&parent) ||
           dynamic_cast<const TNormalStrain*>    (&parent))
    {
          // pressure, temperatures and volumetric strains must have valid depletion stage suffixes (_Dxx)
          if(idxParentStage > 0)
      return true;
    }
/*
    // material?
    if(idxParentStage > 0)
    {
          const CMaterialServer* pMatServer = ConnectedMaterial(model.DepletionStageEntry().StageByIndex(idxParentStage));
          if(pMatServer && pMatServer->CanConnectItem(parent))
      return true;
    }
    else
    {
          const CMaterialServer& iniMatServer = Material(model.InitialDepletionStage());
          if(iniMatServer.CanConnectItem(parent))
      return true;
    }
*/
      }
    }
  }

  return TBase::CanConnectItem(item);
}

bool CFormationBase::ConnectItem(const CGraphNode& item)
{
  // We must have a volume
  const IFormationElementSet* pElementSet = dynamic_cast<const IFormationElementSet*>(&item);
  if(pElementSet)
  {
    // find the formation of the volume
    CFormationBase *pFormation = (CFormationBase*)&pElementSet->Formation();

    assert(pFormation);				// Should always have a tetraformation
    assert(pFormation != this);		// We should not own this formation

    // Unlink from other formation and link to our selves

  assert(!IsCopy() && !pElementSet->IsCopy()); // reParent doesn't work with copies
  const_cast<IFormationElementSet *>(pElementSet)->reParent(this);

  Modified();

    return true;
  }

  const IPointSet *pPointSet = dynamic_cast<const IPointSet *> (&item);
  if(pPointSet)
  {
    assert(CanConnectItem(item));
    for(int i = 0; i < pPointSet->ValueSetSize(); i++)
    {
      if(pPointSet->ValueSet(i).Component())
      {
        const IValueComposite &parent = pPointSet->ValueSet(i).Component()->Parent();
    const CModelBase& model = static_cast<const CModelBase&>(Model());
    int idxParentStage = StageIndexFromName(parent.Name().toStdString().c_str());
    if(idxParentStage >= 0)
    {
          const CDepletionStage& stage = model.DepletionStageEntry().StageByIndex(idxParentStage);

          const TPressure* pPressure = dynamic_cast<const TPressure*>(&parent);
          if(pPressure)
          {
      CPressure& pressure = Pressure(stage);
      for(size_t j = 0; j < pressure.DistributedSize(); ++j)
              pressure.UnLink(const_cast<TPressure&>(pressure.DistributedValue(j)));
      pressure.LinkTo(const_cast<TPressure&>(*pPressure));
      continue;
          }

          const TTemperature* pTemperature = dynamic_cast<const TTemperature*>(&parent);
          if(pTemperature)
          {
      CTemperature& temperature = UserTemperature(stage);
      for(size_t j = 0; j < temperature.DistributedSize(); ++j)
              temperature.UnLink(const_cast<TTemperature&>(temperature.DistributedValue(j)));
      temperature.LinkTo(const_cast<TTemperature&>(*pTemperature));
      continue;
          }

          const TVolumetricStrain* pVolStrain = dynamic_cast<const TVolumetricStrain*>(&parent);
          if(pVolStrain)
          {
      CStrainLoad& strain = Strain(stage);
      for(size_t j = 0; j < strain.DistributedSize(); ++j)
      {
              const TVolumetricStrain* pDistriStrain = dynamic_cast<const TVolumetricStrain*>(&strain.DistributedValue(j));
              if(pDistriStrain)
        strain.UnLink(const_cast<TVolumetricStrain&>(*pDistriStrain));
      }
      strain.LinkTo(const_cast<TVolumetricStrain&>(*pVolStrain));
      continue;
          }

          const TNormalStrain* pNormalStrain = dynamic_cast<const TNormalStrain*>(&parent);
          if(pNormalStrain)
          {
      CStrainLoad& strain = Strain(stage);
      for(size_t j = 0; j < strain.DistributedSize(); ++j)
      {
              const TNormalStrain* pDistriStrain = dynamic_cast<const TNormalStrain*>(&strain.DistributedValue(j));
              if(pDistriStrain)
        strain.UnLink(const_cast<TNormalStrain&>(*pDistriStrain));
      }
      strain.LinkTo(const_cast<TNormalStrain&>(*pNormalStrain));
      continue;
          }

          const TLateralStrain* pLateralStrain = dynamic_cast<const TLateralStrain*>(&parent);
          if(pLateralStrain)
          {
      CStrainLoad& strain = Strain(stage);
      for(size_t j = 0; j < strain.DistributedSize(); ++j)
      {
              const TLateralStrain* pDistriStrain = dynamic_cast<const TLateralStrain*>(&strain.DistributedValue(j));
              if(pDistriStrain)
        strain.UnLink(const_cast<TLateralStrain&>(*pDistriStrain));
      }
      strain.LinkTo(const_cast<TLateralStrain&>(*pLateralStrain));
      continue;
          }

          CMaterialServer* pMatServer = ConnectedMaterial(stage);
          if(pMatServer)
          {
      if(pMatServer->IsLinkedTo(parent))
              continue; // nothing to be done...

      if(pMatServer->CanConnectItem(parent))
      {
              pMatServer->ConnectItem(parent);
              continue;
      }
          }
    }
    else
    {
          // try initial material (no valid depletion stage suffix)
          CMaterialServer& iniMatServer = Material(model.InitialDepletionStage());
          if(iniMatServer.CanConnectItem(parent))
      iniMatServer.ConnectItem(parent);
    }
      }
  }

    return true;
  }

  return TBase::ConnectItem(item);
}

bool CFormationBase::Contains(const geo::IPoint &pt,bool bIncludeEdge) const
{
  if (InBoundingBox(pt))
  {
  for (int j = 0; j < ElementSetSize(); j++)
  {
      if (ElementSet(j).ElementSet().Contains(pt, bIncludeEdge))
      {
    return true;
      }
  }
  }

  return false;
}

geo::CValue CFormationBase::ThicknessAt(const geo::IPoint& pt, geo::IPoint& ptTop, geo::IPoint& ptBottom, geo::IParallelInitializationCallback *cb) const
{
  geo::CValue valRet;

  int i;
  for(i = 0; i < ElementSetSize(); ++i)
  {
  geo::CValue v = ElementSet(i).ThicknessAt(pt, ptTop, ptBottom, cb);
  if (v.Valid()) // in case of wellpaths and multiple volumes, we don't need to intersect at all
  {
      if(valRet.Valid())
    valRet = valRet.Value() + v.Value();
      else
    valRet = v;
  }
  }

  return valRet;
}

void CFormationBase::clearThicknessCache()
{
  for (int i = 0; i < ElementSetSize(); ++i)
  {
  try
  {
      dynamic_cast <CFormationVolume&> (ElementSet(i)).clearThicknessCache();
  }

  catch (const std::bad_cast&)
  {
  }
  }
}

geo::CValue CFormationBase::MinZ() const
{
  // We take the min and max of the horizons the formation is surrounded by.
  geo::CValue ret;
  for(int i = 0; i < ElementSetSize(); i++)
  {
    TGraphNodeSet indentifier =	ElementSet(i).Identifier();
    for(TGraphNodeSet::iterator it = indentifier.begin(); it != indentifier.end(); it++)
    {
      const CHorizonBase* pHorizon = 0;
      const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(*it);
      if( pSurface )
        pHorizon = dynamic_cast<const CHorizonBase*>(pSurface->Used());

      if(pHorizon)
      {
        geo::CPoint ptMin = pHorizon->Min();
        if(!ptMin.Empty())
          ret = ret.Min(geo::CValue(ptMin.Z()));
      }
    }
  }

  return ret;
}

bool CFormationBase::Less(const CGraphNode &node) const
{
  // We take the min and max of the horizons the formation is surrounded by.
  const CFormationBase* pRhs = dynamic_cast<const CFormationBase*>(&node);
  geo::CValue min_this = MinZ();
  geo::CValue min_rhs;
  if(pRhs)
    min_rhs = pRhs->MinZ();

  if(min_this.Valid() && min_rhs.Valid())	return min_this.Value() < min_rhs.Value();
  if(min_this.Valid() && !min_rhs.Valid()) return false;
  if(!min_this.Valid() && min_rhs.Valid()) return true;
  return TBase::Less(node);
}

const CLengthQuantity& CFormationBase::MinThickness() const
{
  return m_MinThickness;
}

void CFormationBase::MinThickness(const double& value, CQuantity::UNIT unit)
{
  m_MinThickness.Value(value, unit);
}

CElementOrder::TElementOrder CFormationBase::ElementOrder() const
{
  return m_formationElementOrder.elementOrder();
}

void CFormationBase::ElementOrder(CElementOrder::TElementOrder elementOrder)
{
  m_formationElementOrder.elementOrder(elementOrder);
}

bool CFormationBase::inheritElementOrderFromModel() const
{
  return m_formationElementOrder.inheritFromModel();
}

void CFormationBase::inheritElementOrderFromModel(
  bool inheritElementOrderFromModel)
{
  m_formationElementOrder.inheritFromModel(inheritElementOrderFromModel);
}

void CFormationBase::CalculateBoundingBox() const
{
  m_min.Set(DBL_MAX, DBL_MAX, DBL_MAX);
  m_max.Set(-DBL_MAX, -DBL_MAX, -DBL_MAX);
  for (int i = 0; i < ElementSetSize(); ++i)
  {
  const geo::IElementSet& eltset = ElementSet(i).ElementSet();
  for (int j = 0; j < eltset.ElementSize(); ++j)
  {
      const geo::IElement& elt = eltset.Element(j);

      for (int k = 0; k < elt.NrOfNodes(); ++k)
      {
    const geo::INode& node = elt.Node(k);

    if (node.X() < m_min.X()) m_min.X(node.X());
    if (node.Y() < m_min.Y()) m_min.Y(node.Y());
    if (node.Z() < m_min.Z()) m_min.Z(node.Z());

    if (node.X() > m_max.X()) m_max.X(node.X());
    if (node.Y() > m_max.Y()) m_max.Y(node.Y());
    if (node.Z() > m_max.Z()) m_max.Z(node.Z());
      }
  }
  }
}

bool CFormationBase::InBoundingBox(const geo::CPoint & point, double eps) const
{
  if (m_min.Empty() || m_max.Empty())
  CalculateBoundingBox();

  if (point.X() < m_min.X() - eps)
  return false;
  if (point.Y() < m_min.Y() - eps)
  return false;
  if (point.Z() < m_min.Z() - eps)
  return false;
  if (point.X() > m_max.X() + eps)
  return false;
  if (point.Y() > m_max.Y() + eps)
  return false;
  if (point.Z() > m_max.Z() + eps)
  return false;

  return true;
}

/*
void CFormationBase::SaveMaterial(CDepletionStage& stage, TSTREAM& stream, TPROGRESS& progress)
{
  CMaterialServer* pMatServer = ConnectedMaterial(stage);
  if(pMatServer)
  pMatServer->SaveStream(stream, progress);
}

void CFormationBase::LoadMaterial(CDepletionStage& stage, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if(HasMaterial(stage))
  {
  // create material server for this stage
  CMaterialServer* pMatServer = new CMaterialServer(*this, stage);

  if(version > CStreamVersion(3, 0, 100))
      pMatServer->LoadStream(stream, version, progress);
  }
}

long CFormationBase::SavedMaterialItems(const CDepletionStage& stage) const
{
  if(HasMaterial(stage))
  return Material(stage).SavedItems();

  return 0;
}
*/

/////////////////////////////////////////////////////////////
// CFormationThickness
/////////////////////////////////////////////////////////////

CFormationThickness::CFormationThickness(CFormationBase& formation, CFemAppModel& model)
: IValueComposite("Thickness", model)
{
  reParent(&formation);
  new CFormationThicknessComponent(*this, formation);
}

unsigned int CFormationThickness::IconId() const
{
  return IDI_FORMATION_THICKNESS;
}

unsigned int CFormationThickness::TypeId() const
{
  return 0;
}

long CFormationThickness::SavedItems() const
{
  return 0;
}

/////////////////////////////////////////////////////////////
// CFormationThickness::CFormationThicknessComponent
/////////////////////////////////////////////////////////////

CFormationThickness::CFormationThicknessComponent::CFormationThicknessComponent(CFormationThickness& parent, const CFormationBase& formation)
: IValueComponentBase(formation.Name(), parent),
  m_Data(*this),
  m_formation(formation)
{
}

IValueComponentBase::TValueComponentType CFormationThickness::CFormationThicknessComponent::Type() const
{
  return static_cast<IValueComponentBase::TValueComponentType>(ScalarData().Type());
}

const IValueDataInterfaceScalar& CFormationThickness::CFormationThicknessComponent::ScalarData() const
{
  return m_Data;
}

IValueDataInterfaceScalar& CFormationThickness::CFormationThicknessComponent::ScalarData()
{
  return m_Data;
}

unsigned int CFormationThickness::CFormationThicknessComponent::IconId() const
{
  return IDI_RESULT;
}

unsigned int CFormationThickness::CFormationThicknessComponent::TypeId() const
{
  return 0;
}

bool CFormationThickness::CFormationThicknessComponent::Empty() const
{
  return false;
}

long CFormationThickness::CFormationThicknessComponent::SavedItems() const
{
  assert(false);
  return 0;
}

QString CFormationThickness::CFormationThicknessComponent::ExportLabel() const
{
  return getStringTableEntry(IDS_ET_FORMATIONTHICKNESS);
}

QString CFormationThickness::CFormationThicknessComponent::UnitName(const UNIT unit) const
{
  QString sRet;
  if(unit == CQuantity::SI_UNIT)
  sRet = getStringTableEntry(IDS_UNIT_SI_LENGTH);
  else
  sRet = getStringTableEntry(IDS_UNIT_FIELD_LENGTH);

  return sRet;
}

IValueDomainScalar::TMinMax CFormationThickness::CFormationThicknessComponent::MinMax(IProgressBase& /*progressBase*/, const CQuantity::UNIT /*unit*/) const
{
  return IValueDomainScalar::TMinMax();
}

IValueDomainScalar::TValue CFormationThickness::CFormationThicknessComponent::Average(IProgressBase&, const CQuantity::UNIT) const
{
  return IValueDomainScalar::TValue();
}

IValueDomainScalar::TValue CFormationThickness::CFormationThicknessComponent::ValuePoint(const geo::IPoint& pt, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  // dummy top and bottom points
  geo::CPoint ptTop;
  geo::CPoint ptBottom;
  geo::CValue v = m_formation.ThicknessAt(pt, ptTop, ptBottom, cb);
  if(v.Valid() && unit == CQuantity::FIELD_UNIT)
  v = v.Value() * FF_FACTOR_LENGTH;

  return v;
}

IValueDomainScalar::TValueVec CFormationThickness::CFormationThicknessComponent::ValueElement(const geo::IElement& elm, const UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  int i;
  IValueDomainScalar::TValueVec vcValue(elm.NrOfNodes());
  for(i = 0; i < elm.NrOfNodes(); ++i)
  vcValue[i] = ValuePoint(elm.Node(i), unit, cb);

  return vcValue;
}

bool CFormationThickness::CFormationThicknessComponent::Defined() const
{
  return true;
}

const CFormationBase& CFormationThickness::CFormationThicknessComponent::Formation() const
{
  return m_formation;
}

////////////////////////////////////////
// TFormationBaseEntry
////////////////////////////////////////

TFormationBaseEntry::TFormationBaseEntry(int nEntryId,unsigned int uIconId,const QString &strName,CFemAppModel &model):
CGraphEntryTemp<CFormationBase>(nEntryId, uIconId, strName, model)
{
}

TFormationBaseEntry::TFormationBaseEntry(int nEntryId,unsigned int uIconId,unsigned int uNameId,CFemAppModel &model):
CGraphEntryTemp<CFormationBase>(nEntryId, uIconId, uNameId, model)
{
}


/*virtual*/ void TFormationBaseEntry::OnNewNeighbour(const CGraphNode &node)
{
  CGraphEntryTemp<CFormationBase>::OnNewNeighbour(node);
  if(dynamic_cast<const CFormationBase*>(&node))
    Modified(NEW_FORMATION_ATTACHED);
}

/*virtual*/ void TFormationBaseEntry::OnNeighbourDeleted(const CGraphNode &node)
{
  CGraphEntryTemp<CFormationBase>::OnNeighbourDeleted(node);
  if(dynamic_cast<const CFormationBase*>(&node))
    Modified(FORMATION_DELETED);
}
