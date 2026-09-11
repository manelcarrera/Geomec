
#include "WellCasingNode.h"

#include "WellCasingModel.h"
#include "WellCasingSteel.h"
#include "WellCasingCementInterface.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalTemperature.h"
#include "DrawDef.h"

CWellCasingNode::CWellCasingNode(CWellCasingModel& model)
: COpenGLNode("Casing", model),
  m_pSteel(0),
  m_pCementInterface(0)
{
  if(!model.Loading())
  {
  // create the casing steel node
  m_pSteel = new CWellCasingSteel("Casing Steel", model);
  m_pSteel->reParent(this);

  // create new casing steel material
  CWellCasingSteelMaterialEntry& steel_entry = (CWellCasingSteelMaterialEntry&)(*model.GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY));
  CWellCasingSteelMaterial& steel_mat = steel_entry.InsertNew();

  // link casing steel material to the casing
  assert(!steel_entry.EntryNodes().empty());
  CWellCasingMaterialServer& casingmatserver = m_pSteel->Material(model.InitialDepletionStage());
  casingmatserver.LinkTo(steel_mat);

  // create the casing-cement interface node
  m_pCementInterface = new CWellCasingCementInterface("Steel-Cement Interface", model);
  m_pCementInterface->reParent(this);

  // create new interface material if necessary
  CInterfaceMaterialEntry& iface_entry = (CInterfaceMaterialEntry&)(*model.GraphEntry(MD_BASE_INTERFACEMATERIAL));
  CInterfaceMaterial& iface_mat = iface_entry.InsertNew();

  // link interface material to the casing-cement interface
  assert(!iface_entry.EntryNodes().empty());
  CWellCasingCementInterfaceMaterialServer& ifacematserver = m_pCementInterface->Material(model.InitialDepletionStage());
  ifacematserver.LinkTo(iface_mat);

  LinkTo(model.DepletionStageEntry());
  UpdatePressuresAndTemperatures();
  }
}

unsigned int CWellCasingNode::IconId() const
{
  return IDI_WELLCASINGMODEL;
}

unsigned int CWellCasingNode::TypeId() const
{
  return 0;
}

bool CWellCasingNode::Empty() const
{
  return false;
}

int CWellCasingNode::DisplayListSize() const
{
  if(m_pSteel && m_pCementInterface)
  return m_pSteel->DisplayListSize() + m_pCementInterface->DisplayListSize();

  return 0;
}

const geo::IObject& CWellCasingNode::DisplayList(int nIndex) const
{
  assert(m_pSteel && m_pCementInterface);
  if(nIndex < m_pSteel->DisplayListSize())
  return m_pSteel->DisplayList(nIndex);

  nIndex -= m_pSteel->DisplayListSize();
  assert(nIndex < m_pCementInterface->DisplayListSize());
  return m_pCementInterface->DisplayList(nIndex);
}

CWellCasingNode::TColor CWellCasingNode::Color() const
{
  assert(false);
  return TColor();
}

std::vector<CDrawDef::TColor> CWellCasingNode::OnColor(const geo::IObject &object) const
{
  assert(m_pSteel && m_pCementInterface);

  int i;
  for(i = 0; i < m_pSteel->DisplayListSize(); ++i)
  {
  const geo::IObject& displaylist = m_pSteel->DisplayList(i);
  const geo::IElementSet* pElSet = dynamic_cast<const geo::IElementSet*>(&displaylist);
  if(pElSet)
  {
      for(int j = 0; j < pElSet->ElementSize(); ++j)
      {
    if(&pElSet->Element(j) == &object)
          return m_pSteel->OnColor(object);
      }
  }
  else if(&displaylist == &object)
  {
      return m_pSteel->OnColor(object);
  }
  }

  return m_pCementInterface->OnColor(object);
}

void CWellCasingNode::OnNewNeighbour(const CGraphNode& node)
{
  const CWellCasingInternalPressure* pPressure = dynamic_cast<const CWellCasingInternalPressure*>(&node);
  if(pPressure)
  m_mpPressures.insert(TPressureMap::value_type(&pPressure->Stage(), const_cast<CWellCasingInternalPressure*>(pPressure)));

  const CWellCasingInternalTemperature* pTemperature = dynamic_cast<const CWellCasingInternalTemperature*>(&node);
  if(pTemperature)
  m_mpTemperatures.insert(TTemperatureMap::value_type(&pTemperature->Stage(), const_cast<CWellCasingInternalTemperature*>(pTemperature)));

  COpenGLNode::OnNewNeighbour(node);
}

void CWellCasingNode::OnNeighbourDeleted(const CGraphNode& node)
{
  TPressureMap::iterator itp;
  for(itp = m_mpPressures.begin(); itp != m_mpPressures.end(); ++itp)
  {
  if(itp->second == &node)
  {
      m_mpPressures.erase(itp);
      return;
  }
  }

  TTemperatureMap::iterator itt;
  for(itt = m_mpTemperatures.begin(); itt != m_mpTemperatures.end(); ++itt)
  {
  if(itt->second == &node)
  {
      m_mpTemperatures.erase(itt);
      return;
  }
  }

  COpenGLNode::OnNeighbourDeleted(node);
}

void CWellCasingNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(&node == &model.DepletionStageEntry())
  UpdatePressuresAndTemperatures();

  if(&node == m_pSteel || &node == m_pCementInterface)
  Modified(); // may require redraw

  COpenGLNode::OnNeighbourModified(node, uHint);
}

CWellCasingSteel& CWellCasingNode::Steel()
{
  return *m_pSteel;
}

const CWellCasingSteel& CWellCasingNode::Steel() const
{
  return *m_pSteel;
}

CWellCasingCementInterface& CWellCasingNode::CementInterface()
{
  return *m_pCementInterface;
}

const CWellCasingCementInterface& CWellCasingNode::CementInterface() const
{
  return *m_pCementInterface;
}

CWellCasingInternalPressure& CWellCasingNode::InternalPressure(const CDepletionStage& stage)
{
  TPressureMap::iterator it = m_mpPressures.find(&stage);
  assert(it != m_mpPressures.end());
  return *it->second;
}

const CWellCasingInternalPressure& CWellCasingNode::InternalPressure(const CDepletionStage& stage) const
{
  TPressureMap::const_iterator it = m_mpPressures.find(&stage);
  assert(it != m_mpPressures.end());
  return *it->second;
}

CWellCasingInternalTemperature& CWellCasingNode::InternalTemperature(const CDepletionStage& stage)
{
  TTemperatureMap::iterator it = m_mpTemperatures.find(&stage);
  assert(it != m_mpTemperatures.end());
  return *it->second;
}

const CWellCasingInternalTemperature& CWellCasingNode::InternalTemperature(const CDepletionStage& stage) const
{
  TTemperatureMap::const_iterator it = m_mpTemperatures.find(&stage);
  assert(it != m_mpTemperatures.end());
  return *it->second;
}

void CWellCasingNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  COpenGLNode::SaveStream(stream, progress);
  m_pSteel->SaveStream(stream, progress);
  m_pCementInterface->SaveStream(stream, progress);

  CModelBase& model = static_cast<CModelBase&>(Model());

  stream << int(model.NrOfDepletionStages());
  CDepletionStageEntry::iterator it;
  for(it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
  {
  InternalPressure(*it).SaveStream(stream, progress);
  InternalTemperature(*it).SaveStream(stream, progress);
  }
}

void CWellCasingNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CWellCasingModel& model = static_cast<CWellCasingModel&>(Model());
  UnLink(*Model().GraphEntry(MD_BASE_OPENGL_NODE));

  COpenGLNode::LoadStream(stream, version, progress);

  m_pSteel = new CWellCasingSteel(model);
  m_pSteel->LoadStream(stream, version, progress);

  m_pCementInterface = new CWellCasingCementInterface(model);
  m_pCementInterface->LoadStream(stream, version, progress);

  int nStages;
  stream >> nStages;

  assert(model.NrOfDepletionStages() == nStages);

  CDepletionStageEntry::iterator it;
  for(it = model.DepletionStageEntry().begin(); it != model.DepletionStageEntry().end(); ++it)
  {
  CWellCasingInternalPressure* pPressure = new CWellCasingInternalPressure(*it);
  pPressure->LoadStream(stream, version, progress);
  pPressure->reParent(this);

  CWellCasingInternalTemperature* pTemperature = new CWellCasingInternalTemperature(*it);
  pTemperature->LoadStream(stream, version, progress);
  pTemperature->reParent(this);
  }

  m_pSteel->reParent(this);
  m_pCementInterface->reParent(this);
  LinkTo(model.DepletionStageEntry());
}

long CWellCasingNode::SavedItems() const
{
  long lRet = COpenGLNode::SavedItems();

  lRet += m_pSteel->SavedItems();
  lRet += m_pCementInterface->SavedItems();

  TPressureMap::const_iterator itp;
  for(itp = m_mpPressures.begin(); itp != m_mpPressures.end(); ++itp)
  lRet += itp->second->SavedItems();

  TTemperatureMap::const_iterator itt;
  for(itt = m_mpTemperatures.begin(); itt != m_mpTemperatures.end(); ++itt)
  lRet += itt->second->SavedItems();

  return lRet;
}

void CWellCasingNode::UpdatePressuresAndTemperatures()
{
  CModelBase& model = static_cast<CModelBase&>(Model());
  CDepletionStageEntry& entry = model.DepletionStageEntry();
  CDepletionStageEntry::iterator it;
  std::set<const CDepletionStage*> stStages;

  for(it = entry.begin(); it != entry.end(); ++it)
  {
  CDepletionStage& stage = *it;
  stStages.insert(&stage);

  if(m_mpPressures.find(&stage) == m_mpPressures.end())
      (new CWellCasingInternalPressure(stage))->reParent(this);

  if(m_mpTemperatures.find(&stage) == m_mpTemperatures.end())
      (new CWellCasingInternalTemperature(stage))->reParent(this);
  }

  std::vector<CWellCasingInternalPressure*> vcDeletePressures;
  TPressureMap::iterator itp;;
  for(itp = m_mpPressures.begin(); itp != m_mpPressures.end(); ++itp)
  {
  if(stStages.find(itp->first) == stStages.end())
      vcDeletePressures.push_back(itp->second);
  }

  std::vector<CWellCasingInternalTemperature*> vcDeleteTemperatures;
  TTemperatureMap::iterator itt;
  for(itt = m_mpTemperatures.begin(); itt != m_mpTemperatures.end(); ++itt)
  {
  if(stStages.find(itt->first) == stStages.end())
      vcDeleteTemperatures.push_back(itt->second);
  }

  size_t i;
  for(i = 0; i < vcDeletePressures.size(); ++i)
  delete vcDeletePressures[i];

  for(i = 0; i < vcDeleteTemperatures.size(); ++i)
  delete vcDeleteTemperatures[i];
}
