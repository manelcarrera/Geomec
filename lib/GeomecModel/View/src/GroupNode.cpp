#include "GroupNode.h"
#include "Delegate.h"
#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "resourceIDI.h"


CGroupNode::CGroupNode(CFemAppModel& model)
  : COpenGLNode(model)
  , m_entry(nullptr)
  , m_readOnly(false)
{
  CGroupNodeEntry *groupEntry = static_cast<CGroupNodeEntry *>(model.GraphEntry(MD_BASE_GROUPNODE));
  reParent(groupEntry);
}
  
CGroupNode::CGroupNode(const QString &strName, CFemAppModel& model, CGraphEntry *entry)
  : COpenGLNode(strName, model)
  , m_entry(entry)
  , m_readOnly(false)
{
  CGroupNodeEntry *groupEntry = static_cast<CGroupNodeEntry *>(model.GraphEntry(MD_BASE_GROUPNODE));
  reParent(groupEntry);

  if (entry)
    LinkTo(*entry);
}

CGroupNode::CGroupNode(const CGroupNode& rhs)
  : COpenGLNode(rhs)
  , m_entry(rhs.m_entry)
  , m_readOnly(rhs.m_readOnly)
  , m_description(rhs.m_description)
{
}

CGroupNode::~CGroupNode()
{
}

CGroupNode& CGroupNode::operator=(const CGroupNode &rhs)
{
  COpenGLNode::operator=(rhs);

  m_entry = rhs.m_entry;
  m_readOnly = rhs.m_readOnly;
  m_description = rhs.m_description;

  return *this;
}

bool CGroupNode::operator==(const CGroupNode &rhs) const
{
  return COpenGLNode::operator==(rhs) && m_entry == rhs.m_entry && m_readOnly == rhs.m_readOnly && m_description == rhs.m_description;
}


const QString& CGroupNode::Description() const
{
  return m_description;
}

void CGroupNode::Description(const QString& description)
{
  m_description = description;
}

bool CGroupNode::ReadOnly() const
{
  return m_readOnly;
}

void CGroupNode::ReadOnly(bool readOnly)
{
  m_readOnly = readOnly;
}

bool CGroupNode::Empty() const
{ 
  return true;
}

long CGroupNode::SavedItems() const
{ 
  return 1; 
}

void CGroupNode::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  COpenGLNode::LoadStream(stream, version, progress);

  stream >> m_description;

  int readOnly;
  stream >> readOnly;

  m_readOnly = readOnly;

  int entryIndex;
  stream >> entryIndex;

  m_entry = Model().GraphEntry(entryIndex);

  int groupSize;
  stream >> groupSize;

  if (m_entry)
  {
    LinkTo(*m_entry);

    if (groupSize > 0)
    {
      // technical debt: methods to find nodes by id are template-typed, and can't be used here; TODO: refactor storage node entry
      std::map<int, CStorageNode *> entryNodes;
      for (int i = 0; i < m_entry->childSize(); ++i)
      {
        CStorageNode *entryNode = dynamic_cast<CStorageNode *>(&m_entry->childAt(i));

        if (entryNode)
          entryNodes.insert(std::make_pair(entryNode->Index(), entryNode));
      }

      for (int i = 0; i < groupSize; ++i)
      {
        int index;
        stream >> index;

        std::map<int, CStorageNode *>::iterator node = entryNodes.find(index);

        if (node != entryNodes.end())
          node->second->LinkTo(*this);
      }
    }
  }
  else // shouldn't happen, but if it does, we need to make sure file format is not broken
  {
    for (int i = 0; i < groupSize; ++i)
    {
      int dummy;
      stream >> dummy;
    }
  }

  progress.Step();
}

void CGroupNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  COpenGLNode::SaveStream(stream, progress);

  stream << m_description;
  stream << (int)m_readOnly;

  stream << (int)m_entry->EntryId();

  std::set<CStorageNode *> nodes = Links<CStorageNode>();

  stream << (int)nodes.size();

  for (std::set<CStorageNode *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    stream << (*it)->Index();

  progress.Step();
}

int CGroupNode::DisplayListSize() const
{ 
  return 0;
}

const geo::IObject& CGroupNode::DisplayList(int nIndex) const
{ 
  geo::IObject *bogus = nullptr;
  return *bogus;
}

COpenGLNodeBase::TColor CGroupNode::Color() const
{ 
  return TColor();
}

bool CGroupNode::CanDestroy() const
{
  return !ReadOnly();
}

bool CGroupNode::ConnectItem(const CGraphNode &item)
{
  if (CanConnectItem(item))
  {
    if (CGraphNode::ConnectItem(item))
    {
      RemoveGroups(const_cast<CGraphNode&>(item), this);

      Modified();
      return true;
    }
  }
  
  return false;
}

bool CGroupNode::CanConnectItem(const CGraphNode &item) const
{
  return !ReadOnly() && item.IsLinkedTo(*m_entry);
}

bool CGroupNode::CanDisconnectItem(const CGraphNode& item) const
{
  return !ReadOnly() && item.IsLinkedTo(*this);
}

void CGroupNode::Modified(enum ModifiedHint uHint)
{
  COpenGLNode::Modified(uHint);
  if (m_entry) m_entry->Modified(uHint);
}

const CGraphEntry *CGroupNode::GraphEntry() const
{
  return m_entry;
}

void CGroupNode::RemoveGroups(CGraphNode& graphNode, const CGroupNode * keepGroup)
{
  std::set<CGroupNode *> nodes = graphNode.Links<CGroupNode>();

  bool isModified = false;

  for (std::set<CGroupNode *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    if (*it != keepGroup)
    {
      graphNode.UnLink(**it);
      isModified = true;
    }
  }
  if (isModified)
    graphNode.Modified();
}



CGroupNodeEntry::CGroupNodeEntry(C3DModel &model)
  : BASE_CLASS(MD_BASE_GROUPNODE, IDI_GROUPNODE, "GroupNodes", model)
{
}

CGroupNodeEntry::~CGroupNodeEntry()
{
}
