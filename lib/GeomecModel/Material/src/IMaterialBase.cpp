
#include "IMaterialBase.h"

#include "IMaterialServerBase.h"

IMaterialBase::IMaterialBase(const QString& strName, CFemAppModel& model)
: CStorageNode(strName, model)
{
}

IMaterialBase::IMaterialBase(const IMaterialBase& rhs)
: CStorageNode(rhs)
{
}

IMaterialBase::~IMaterialBase()
{
}

void IMaterialBase::OnNewNeighbour(const CGraphNode &node)
{
  const IMaterialServerBase* pServer = dynamic_cast<const IMaterialServerBase*>(&node);
  if(pServer)
  m_stServers.insert(pServer);
}

void IMaterialBase::OnNeighbourDeleted(const CGraphNode &node)
{
  // cannot use dynamic_cast here, since the CMaterialServer may have already been destroyed
  // use static cast and lookup the pointer in the set
  const IMaterialServerBase* pServer = static_cast<const IMaterialServerBase*>(&node);
  TServerSet::iterator it = m_stServers.find(pServer);
  if(it != m_stServers.end())
  m_stServers.erase(it);
}

bool IMaterialBase::ReadOnly() const
{
  TServerSet::const_iterator it;
  for(it = m_stServers.begin(); it != m_stServers.end(); ++it)
  {
  if((*it)->ReadOnly())
      return true;
  }

  return false;
}

bool IMaterialBase::FixedMaterialModel() const
{
  return false;
}

bool IMaterialBase::Less(const CGraphNode& node) const
{
  const IMaterialBase* pMat = dynamic_cast<const IMaterialBase*>(&node);
  if(pMat)
  {
  // sort on type
  return typeid(*this).before(typeid(*pMat));
  }

  return CStorageNode::Less(node);
}

const IMaterialBase::TServerSet& IMaterialBase::Servers() const
{
  return m_stServers;
}
