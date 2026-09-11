#include "OpenGLNodeSelection.h"

#include <algorithm>

#include "openglnode.h"
#include "HorizonBase.h"
#include "FemAppModel.h"
#include "BaseEntryTypes.h"
#include "FormationBase.h"
#include "HexaFormation.h"
#include "HexaEntryTypes.h"
#include "NewWellPath.h"


bool COpenGLNodeSelection::Less::operator()(const COpenGLNode *lhs, const COpenGLNode *rhs) const
{
  if (lhs && rhs)
  {
  if (lhs->Less(*rhs))
      return true;
  if (rhs->Less(*lhs))
      return false;
  }

  return lhs < rhs;
}


COpenGLNodeSelection::COpenGLNodeSelection()
{
}

COpenGLNodeSelection::COpenGLNodeSelection(const COpenGLNodeSelection& rhs)
: m_stSelection(rhs.m_stSelection)
{
}

COpenGLNodeSelection::~COpenGLNodeSelection()
{
  // delete registered observers
  for(TObserverSet::iterator it = m_stObservers.begin(); it != m_stObservers.end(); ++it)
  delete *it;
}

COpenGLNodeSelection& COpenGLNodeSelection::operator=(const COpenGLNodeSelection& rhs)
{
  TSelectionSet stRemoved;
  TSelectionSet stAdded;

  // items in my set but not in rhs are removed
  std::set_difference(m_stSelection.begin(), m_stSelection.end(), rhs.m_stSelection.begin(), rhs.m_stSelection.end(),
  std::inserter(stRemoved, stRemoved.end()));
  // items in rhs but not in my set are added
  std::set_difference(rhs.m_stSelection.begin(), rhs.m_stSelection.end(), m_stSelection.begin(), m_stSelection.end(),
  std::inserter(stAdded, stAdded.end()));

  m_stSelection = rhs.m_stSelection;

  // Don't inform observers, as that interferes with the initialization of the dialog

  return *this;
}

bool COpenGLNodeSelection::operator==(const COpenGLNodeSelection& rhs) const
{
  return m_stSelection == rhs.m_stSelection;
}

COpenGLNodeSelection::const_iterator COpenGLNodeSelection::begin() const
{
  return m_stSelection.begin();
}

COpenGLNodeSelection::const_iterator COpenGLNodeSelection::end() const
{
  return m_stSelection.end();
}

COpenGLNodeSelection::iterator COpenGLNodeSelection::begin()
{
  return m_stSelection.begin();
}

COpenGLNodeSelection::iterator COpenGLNodeSelection::end()
{
  return m_stSelection.end();
}

std::pair<COpenGLNodeSelection::iterator, bool> COpenGLNodeSelection::insert(const COpenGLNode& node)
{
  std::pair<iterator, bool> prInsert = m_stSelection.insert(&node);

  if(prInsert.second)
  {
  for(TObserverSet::iterator it = m_stObservers.begin(); it != m_stObservers.end(); ++it)
      (*it)->OnNodeInserted(node);
  }

  return prInsert;
}

void COpenGLNodeSelection::erase(iterator pos)
{
  const COpenGLNode& node = **pos;

  m_stSelection.erase(pos);

  for(TObserverSet::iterator it = m_stObservers.begin(); it != m_stObservers.end(); ++it)
  (*it)->OnNodeErased(node);
}

void COpenGLNodeSelection::erase(const COpenGLNode& node)
{
  iterator pos = find(node);
  if(pos != end())
  erase(pos);
}

COpenGLNodeSelection::iterator COpenGLNodeSelection::find(const COpenGLNode& node)
{
  return m_stSelection.find(&node);
}

COpenGLNodeSelection::const_iterator COpenGLNodeSelection::find(const COpenGLNode& node) const
{
  return m_stSelection.find(&node);
}

void COpenGLNodeSelection::clear()
{
  while(!m_stSelection.empty())
  erase(begin());
}

bool COpenGLNodeSelection::empty() const
{
  return m_stSelection.empty();
}

void COpenGLNodeSelection::RegisterObserver(CObserver& observer)
{
  m_stObservers.insert(&observer);

  // initialize observer
  for(const_iterator it = begin(); it != end(); ++it)
  observer.OnNodeInserted(**it);
}

void COpenGLNodeSelection::UnregisterObserver(CObserver& observer)
{
  m_stObservers.erase(&observer);
}

/*
 * The only categories (at this moment) that cannot be retrieved
 * with the GraphEntry() function are CFormationPlane objects.
 * CFormationPlane objects are members of a CHexaFormation object
 * (as far as I can determine). Although they (sometimes) are indexed,
 * upon loading they do not retain their indexes since they are not
 * loaded, but created anew!
 * When saving an CFormationPlane object the type and index of
 * its parent are also saved. In this way the selection can be restored.
 */

void COpenGLNodeSelection::LoadStream(CStorageNode::TSTREAM& stream,
  CStreamVersion& /*version*/, CStorageNode::TPROGRESS& /*progress*/,
  CFemAppModel& model)
{
  THorizonBaseEntry* horizonBaseEntry =
  dynamic_cast <THorizonBaseEntry*> (model.GraphEntry(MD_BASE_HORIZON));
  TFormationBaseEntry* formationBaseEntry =
  dynamic_cast <TFormationBaseEntry*>(model.GraphEntry(MD_BASE_FORMATION));
  CHexaFormationEntry* hexaFormationEntry =
  dynamic_cast <CHexaFormationEntry*> (model.GraphEntry(MD_HEXA_FORMATION));
  CNewWellPathEntry* newWellPathEntry =
  dynamic_cast <CNewWellPathEntry*> (model.GraphEntry(MD_NEW_WELLPATH));
  TPointSetEntry* pointSetEntry =
  dynamic_cast <TPointSetEntry*> (model.GraphEntry(MD_BASE_POINTSET));
  unsigned long size = 0;

  stream >> size;

  for (unsigned long s = 0; s < size; ++s)
  {
  int index = 0;

  stream >> index;

  if ((horizonBaseEntry != 0) && (horizonBaseEntry->FindIndex(index) != 0))
  {
      m_stSelection.insert(horizonBaseEntry->FindIndex(index));
  }
  else if ((formationBaseEntry != 0) &&
      (formationBaseEntry->FindIndex(index) != 0))
  {
      m_stSelection.insert(formationBaseEntry->FindIndex(index));
  }
  else if ((newWellPathEntry != 0) &&
      (newWellPathEntry->FindIndex(index) != 0))
  {
      m_stSelection.insert(newWellPathEntry->FindIndex(index));
  }
  else if ((pointSetEntry != 0) && (pointSetEntry->FindIndex(index) != 0))
  {
      m_stSelection.insert(pointSetEntry->FindIndex(index));
  }
  else
  {
      int planeType = 0;
      int parent = 0;

      stream >> planeType;
      stream >> parent;

      if ((hexaFormationEntry != 0) &&
    (hexaFormationEntry->FindIndex(parent) != 0))
      {
    CHexaFormation* hexaFormation = hexaFormationEntry->FindIndex(parent);

    m_stSelection.insert(hexaFormation->FormationPlane(
          CFormationPlane::ePlaneType(planeType)).data());
      }
      else
      {
    assert(false);
      }
  }
  }
}

void COpenGLNodeSelection::SaveStream(CStorageNode::TSTREAM& stream,
  CStorageNode::TPROGRESS& /*progress*/)
{
  stream << (unsigned long) m_stSelection.size();

  for (TSelectionSet::const_iterator iterator = m_stSelection.begin();
  iterator != m_stSelection.end(); ++iterator)
  {
  stream << (*iterator)->Index();

  if (dynamic_cast <const CFormationPlane*> (*iterator) != 0)
  {
      const CFormationPlane* formationPlane =
    dynamic_cast <const CFormationPlane*> (*iterator);

      stream << formationPlane->PlaneType();
      stream <<
    dynamic_cast <CStorageNode*> (formationPlane->parent())->Index();
  }
  }
}

///// COpenGLNodeSelection::CObserver
COpenGLNodeSelection::CObserver::CObserver()
{
}

COpenGLNodeSelection::CObserver::~CObserver()
{
}

void COpenGLNodeSelection::CObserver::OnNodeErased(const COpenGLNode& /*node*/)
{
}

void COpenGLNodeSelection::CObserver::OnNodeInserted(const COpenGLNode& /*node*/)
{
}
