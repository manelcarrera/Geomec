// TetraFormation.cpp: implementation of the CTetraFormation class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "TetraFormation.h"
#include "TetraEntryTypes.h" 
#include "TetraModel.h"
#include "TetraMesh.h"
#include "TetraHorizonBase.h"
#include "TetSurface.h"
#include "BodyGroup.h"
#include "SurfaceDesc.h"

//////////////////////////////////////////////////////////////////////
// TetraFormationVolume
//////////////////////////////////////////////////////////////////////

CTetraFormationVolume::CTetraFormationVolume(CTetraFormation& formation, geo::CBodyGroup& body_group)
: CFormationVolume(formation, body_group)
{
}

CTetraFormationVolume::CTetraFormationVolume(CTetraFormation& formation)
: CFormationVolume(formation)
{
}

bool CTetraFormationVolume::CanDestroy() const
{
  // only allow deletion when there is no mesh
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  return !model.IsMesh();
}

int CTetraFormationVolume::DisplayListSize() const
{
  int nRet = CFormationVolume::DisplayListSize();
  if(nRet) {
    // Get formation entry to see or we in skin mode or not
    const CTetraFormationEntry *pEntry = dynamic_cast<const CTetraFormationEntry*>(Model().GraphEntry(MD_TETRA_FORMATION));
    if(pEntry)
    {
      if(pEntry->ShowSkin())
        return Volume().SideSurfaceSize();
      else if(pEntry->ShowMidpoints())
    return 1;
    }
  }

  return nRet;
}

const geo::IObject& CTetraFormationVolume::DisplayList(int nIndex) const
{
  // Get formation entry to see or we in skin mode or not
  const CTetraFormationEntry *pEntry = dynamic_cast<const CTetraFormationEntry*>(Model().GraphEntry(MD_TETRA_FORMATION));
  assert(pEntry);

  const CModelBase& model = dynamic_cast<const CModelBase&>(Model());
  assert(model.IsMesh());
  
  if(pEntry->ShowSkin())
    return Volume().SideSurface(nIndex);
  else if(pEntry->ShowMidpoints())
  return ElementCenterPoints();

  return CFormationVolume::DisplayList(nIndex);
}

void CTetraFormationVolume::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  IFormationElementSet::LoadStream(stream, version, progress);
  TOpenGLNodeBaseEntry& opengl_entry = (TOpenGLNodeBaseEntry&)*Model().GraphEntry(MD_BASE_OPENGL_NODE);

  // Load identifiers
  int nIdentifierSize;
  stream >> nIdentifierSize;
  TGraphNodeSet stNode;
  for(int i = 0; i < nIdentifierSize; i++)
  {
    int nIndex;
    stream >> nIndex;
    CGraphNode* pNode = opengl_entry.FindIndex(nIndex);
    assert(pNode);
    stNode.insert(pNode);
    progress.Step();
  }
  Identifier(stNode);

}

TGraphNodeSet CTetraFormationVolume::Identifier() const
{
  return m_identifier;
}

void CTetraFormationVolume::Identifier(const TGraphNodeSet& identifier)
{
  // Unlink till identifier is empty
  while(m_identifier.size() != 0)
  {
    UnLink(**m_identifier.begin());
  }

  // And add the new identifier
  for(TGraphNodeSet::const_iterator it = identifier.begin(); it != identifier.end(); it++)
  {
    // Linking
    LinkTo((CGraphNode&)(**it));
  }

  m_identifier = identifier;
}

void CTetraFormationVolume::OnNeighbourDeleted(const CGraphNode &item)
{
  TGraphNodeSet::iterator it = m_identifier.find((CGraphNode*)(&item));
  if(it != m_identifier.end())
  {
    // Remove
    m_identifier.erase(it);
  }

  IFormationElementSet::OnNeighbourDeleted(item);
}

void CTetraFormationVolume::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  IFormationElementSet::SaveStream(stream, progress);

  int nIdentifierSize = m_identifier.size();
  stream << nIdentifierSize;

  for(TGraphNodeSet::const_iterator it = m_identifier.begin(); it != m_identifier.end(); it++)
  {
    const COpenGLNode *pNode = dynamic_cast<const COpenGLNode*>(*it);
    assert(pNode);
    stream << pNode->Index();
    progress.Step();
  }
}

long CTetraFormationVolume::SavedItems() const
{
  long lRet = IFormationElementSet::SavedItems();

  lRet += m_identifier.size();

  return lRet;
}

// get a vector of the horizons that enclose (created) this volume
CTetraFormationVolume::TEnclosingHorizonsSet CTetraFormationVolume::GetEnclosingHorizons() const
{
  TEnclosingHorizonsSet stHorizons;

  const CTetraModel& model = static_cast<const CTetraModel&>(Model());
  const CTetraMesh& tetramesh = static_cast<const CTetraMesh&>(model.Mesh());

  if(tetramesh.IsMesh())
  {
    // create a map from the surfaces used in the mesh to the geo surfaces
    typedef std::map<const geo::CSurfaceDesc*, COpenGLNode*> TSurfaceMap;
    TSurfaceMap mpSurfaceMap;

    int i;
    for(i = 0; i < tetramesh.InputSurfaceSize(); ++i)
    {
      mpSurfaceMap.insert(TSurfaceMap::value_type(tetramesh.InputSurface(i).first, tetramesh.InputSurface(i).second));
    }

    // get the descriptors for the source surfaces of this volume and use the map
    // to get their geomec counterparts
    for(i = 0; i < Volume().SideSurfaceSize(); ++i)
    {
      const geo::CTetSurface& SideSurface = static_cast<const geo::CTetSurface&>(Volume().SideSurface(i));
      const geo::CSurfaceDesc& surfdesc = SideSurface.SurfaceDesc();

      TSurfaceMap::iterator it = mpSurfaceMap.find(&surfdesc);
      if(it != mpSurfaceMap.end() && it->second != 0)
      {
        COpenGLNode& node = *it->second;

        for(size_t j = 0; j < node.referenceSize(); ++j)
        {
          const CTetraHorizonBase* pTetraHor = dynamic_cast<const CTetraHorizonBase*>(&node.referenceAt(j));
          if(pTetraHor)
            stHorizons.insert(pTetraHor);
        }
      }
    }
  }

  return stHorizons;
}

CTetraFormationVolume::TEnclosingSurfacesMap CTetraFormationVolume::GetEnclosingSurfaces() const
{
  TEnclosingSurfacesMap mpSurfaces;

  const CTetraModel& model = static_cast<const CTetraModel&>(Model());
  const CTetraMesh& tetramesh = static_cast<const CTetraMesh&>(model.Mesh());

  if (tetramesh.IsMesh())
  {
  // create a map from the surfaces used in the mesh to the geo surfaces
  typedef std::map<const geo::CSurfaceDesc*, COpenGLNode*> TSurfaceMap;
  TSurfaceMap mpSurfaceMap;

  int i;
  for (i = 0; i < tetramesh.InputSurfaceSize(); ++i)
  {
      mpSurfaceMap.insert(TSurfaceMap::value_type(tetramesh.InputSurface(i).first, tetramesh.InputSurface(i).second));
  }

  // get the descriptors for the source surfaces of this volume and use the map
  // to see whether they 
  for (i = 0; i < Volume().SideSurfaceSize(); ++i)
  {
      const geo::CTetSurface& SideSurface = static_cast<const geo::CTetSurface&>(Volume().SideSurface(i));
      const geo::CSurfaceDesc& surfdesc = SideSurface.SurfaceDesc();

      TSurfaceMap::iterator it = mpSurfaceMap.find(&surfdesc);
      if (it != mpSurfaceMap.end() && it->second != 0)
      {
    mpSurfaces.insert(TEnclosingSurfacesMap::value_type(QString(surfdesc.Name().c_str()), it->second->IconId()));
      }
  }
  }

  return mpSurfaces;
}

namespace
{

static unsigned int initialOrdering = 0;

} // anonymous namespace

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTetraFormation::CTetraFormation(const QString &strName, CModelBase& model)
: C3DFormation(strName, model)
, m_order(initialOrdering++)
{
  create( Model().GraphEntry(MD_TETRA_FORMATION) );
}

CTetraFormation::CTetraFormation(CFemAppModel& model)
: C3DFormation(model)
, m_order(initialOrdering++)
{
}

CTetraFormation::~CTetraFormation()
{
}

CTetraFormation::CTetraFormation(const CTetraFormation& rhs)
: C3DFormation(rhs)
, m_order(rhs.m_order)
{
}

bool CTetraFormation::operator==(const CTetraFormation& rhs) const
{
  return C3DFormation::operator ==(rhs);
}

CTetraFormation& CTetraFormation::operator=(const CTetraFormation& rhs)
{
  C3DFormation::operator =(rhs);

  m_order = rhs.m_order;

  return *this;
}

bool CTetraFormation::canMoveDown() const
{
  const CTetraFormationEntry* tetraFormationEntry =
  dynamic_cast <const CTetraFormationEntry*> (
      Model().GraphEntry(MD_TETRA_FORMATION));
  const CTetraFormationEntry::TSortedNodeSet sortedNodeSet =
  tetraFormationEntry->SortedEntryNodes();
  CTetraFormation* tetraFormation = const_cast <CTetraFormation*> (this);

  return ((sortedNodeSet.size() > 1) &&
  (sortedNodeSet.find(tetraFormation) != sortedNodeSet.end()) &&
  (sortedNodeSet.find(tetraFormation) != (--sortedNodeSet.end())));
}

bool CTetraFormation::canMoveUp() const
{
  const CTetraFormationEntry* tetraFormationEntry =
  dynamic_cast <const CTetraFormationEntry*> (
      Model().GraphEntry(MD_TETRA_FORMATION));
  const CTetraFormationEntry::TSortedNodeSet sortedNodeSet =
  tetraFormationEntry->SortedEntryNodes();
  CTetraFormation* tetraFormation = const_cast <CTetraFormation*> (this);

  return ((sortedNodeSet.size() > 1) &&
  (sortedNodeSet.find(tetraFormation) != sortedNodeSet.begin()));
}

void CTetraFormation::moveDown()
{
  const CTetraFormationEntry* tetraFormationEntry =
  dynamic_cast <const CTetraFormationEntry*> (
      Model().GraphEntry(MD_TETRA_FORMATION));
  const CTetraFormationEntry::TSortedNodeSet sortedNodeSet =
  tetraFormationEntry->SortedEntryNodes();
  const CTetraFormationEntry::TSortedNodeSet::const_iterator nextFormation =
  ++(sortedNodeSet.find(this));

  std::swap(m_order, (*nextFormation)->m_order);

  Modified();
}

void CTetraFormation::moveUp()
{
  const CTetraFormationEntry* tetraFormationEntry =
  dynamic_cast <const CTetraFormationEntry*> (
      Model().GraphEntry(MD_TETRA_FORMATION));
  const CTetraFormationEntry::TSortedNodeSet sortedNodeSet =
  tetraFormationEntry->SortedEntryNodes();
  const CTetraFormationEntry::TSortedNodeSet::const_iterator previousFormation =
  --(sortedNodeSet.find(this));

  std::swap(m_order, (*previousFormation)->m_order);

  Modified();
}

unsigned int CTetraFormation::Order() const
{
  return m_order;
}

void CTetraFormation::Order(unsigned int order)
{
  m_order = order;
}

bool CTetraFormation::Less(const CGraphNode &node) const
{
  
  const CTetraFormation* tetraFormation =
      dynamic_cast <const CTetraFormation *> (& node);

  if (tetraFormation)
    return (m_order < tetraFormation->m_order);
 

  return C3DFormation::Less(node);
}

bool CTetraFormation::ShowCenterPoints() const
{
  const CTetraFormationEntry *pEntry = dynamic_cast<const CTetraFormationEntry*>(Model().GraphEntry(MD_TETRA_FORMATION));
  if(pEntry)
    return pEntry->ShowMidpoints();

  return false;
}

namespace
{

bool local_less(const CTetraFormation* lhs, const CTetraFormation* rhs)
{
  return (lhs->Name().compare(rhs->Name()) < 0);
}

} // anonymous namespace

void CTetraFormation::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  CFormationBase::LoadStream(stream, version, progress);

  // For the 3.0.4 version we create and load the formation volumes here ..
  if(CStreamVersion(3,0,3) < version)
  {
    int nFormationVolume;
    stream >> nFormationVolume;

    for(int i = 0; i < nFormationVolume; i++)
    {
      CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
      assert(pModel);
      geo::CBodyGroup *pGroup = new geo::CBodyGroup(pModel->Mesh().Mesh());
      CTetraFormationVolume *pVolume = new CTetraFormationVolume(*this, *pGroup);

      /* From IFormationElementSet constructor */
      pVolume->reParent(this);
//			pVolume->LinkTo(pModel->Mesh());
      /* ************************************* */

      pVolume->LoadStream(stream, version, progress);
    }

    assert(ElementSetSize() == nFormationVolume);
  }

  create( Model().GraphEntry(MD_TETRA_FORMATION) );

  if (CStreamVersion(4, 1, 17) < version)
  {
    stream >> m_order;
    initialOrdering = std::max(initialOrdering, m_order);
  }
  else
  {
    createInitialOrdering();
  }
}

void CTetraFormation::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CFormationBase::SaveStream(stream, progress);
  int nFormationVolume = ElementSetSize();
  stream << nFormationVolume;

  for(int i = 0; i < nFormationVolume; i++)
    ElementSet(i).SaveStream(stream, progress);

  stream << m_order;
}

long CTetraFormation::SavedItems() const
{
  long lRet = CFormationBase::SavedItems();

  for(int i = 0; i < ElementSetSize(); i++)
    lRet += ElementSet(i).SavedItems();

  return lRet;
}

void CTetraFormation::createInitialOrdering()
{
  const CTetraFormationEntry* tetraFormationEntry =
  dynamic_cast <const CTetraFormationEntry*> (
      Model().GraphEntry(MD_TETRA_FORMATION));
  const CTetraFormationEntry::TNodeSet nodeSet =
  tetraFormationEntry->EntryNodes();

  if (nodeSet.size() > 1)
  {
  std::vector <CTetraFormation*> formations;

  for (CTetraFormationEntry::TNodeSet::const_iterator node =
      nodeSet.begin(); node != nodeSet.end(); ++node)
  {
      formations.push_back(*node);
  }

  std::sort(formations.begin(), formations.end(), local_less);

  unsigned int order = 0;

  for (std::vector <CTetraFormation*>::iterator formation =
      formations.begin(); formation != formations.end(); ++formation)
  {
      (*formation)->m_order = order++;
  }

  initialOrdering = order;
  }
}

//////////////////////////////////////////////////////////////////////
// TetraFormationEntry
//////////////////////////////////////////////////////////////////////
CTetraFormationEntry::CTetraFormationEntry(CTetraModel& model)
: CFormationEntryTempl<CTetraFormation>(MD_TETRA_FORMATION, model)
{
}
