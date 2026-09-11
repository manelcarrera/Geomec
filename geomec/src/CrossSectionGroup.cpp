#include "stdafx.h"
#include "CrossSectionGroup.h" 
#include "MeshCrossSection.h"

MeshCrossSection* CrossSectionGroup::buildCrossSectionNode(CCrossSection& xsec) const
{
  MeshCrossSection* mcs = MeshCrossSection::build(*m_meshNodeMgr, &xsec);

  return mcs;
}

CrossSectionGroup::CrossSectionGroup(OIDIMeshNodeManager* mgr)
  : m_meshNodeMgr(mgr)
{
  setName("CrossSectionGroup");
}

void CrossSectionGroup::enableCrossSection(const CCrossSection* xsec)
{
  XSecMap::iterator iter = m_xsecMap.find(xsec);
  if(iter == m_xsecMap.end())
  {
  MeshCrossSection* mcs = buildCrossSectionNode(*const_cast<CCrossSection*>(xsec));
  addChild(mcs);

  m_xsecMap[xsec] = mcs;
  }
}

void CrossSectionGroup::disableCrossSection(const CCrossSection* xsec)
{
  XSecMap::iterator iter = m_xsecMap.find(xsec);
  if(iter != m_xsecMap.end())
  {
  MeshCrossSection* mcs = iter->second;

  removeChild(mcs);
  m_xsecMap.erase(iter);
  }
}

void CrossSectionGroup::setActiveCrossSection(const CCrossSection* xsec)
{
  XSecMap::iterator iter = m_xsecMap.find(xsec);
  if(iter != m_xsecMap.end())
  {
      SoNode* node = iter->second;

      // We're going to make this node the first child node of the group, by first
      // removing it from the parent, and then inserting it again in the new position.
      node->ref();
      removeChild(node);
      insertChild(node, 0);
      node->unref();
  }
}

MeshCrossSection* CrossSectionGroup::getCrossSectionNode(const CCrossSection* xsec) const
{
  XSecMap::const_iterator iter = m_xsecMap.find(xsec);
  return iter != m_xsecMap.end() ? iter->second : 0;
}

void CrossSectionGroup::SelectClipPlaneMode(bool enable)
{
  for (XSecMap::iterator it = m_xsecMap.begin(); it != m_xsecMap.end(); ++it)
  it->second->clip = enable;
}