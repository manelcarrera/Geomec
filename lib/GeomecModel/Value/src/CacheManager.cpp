#include "CacheManager.h"
#include "CacheInterface.h"
#include "CacheInterfaceDefault.h"
#include "IElementSet.h"

void CCacheManager::DestroyCaches() {
  for (TInterfaces::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it) {
    if (it->second->NeedsDelete()) {
      const_cast<geo::IElementSet *>(it->first)->CacheInterface(geo::CCacheInterfaceDefault::Instance());
      delete it->second;
    }
  }
  m_interfaces.clear();
}

void CCacheManager::RegisterElementSet(const geo::IElementSet *elt_set) {
  TInterfaces::iterator it = m_interfaces.find(elt_set);
  if (it != m_interfaces.end()) {
    const_cast<geo::IElementSet *>(elt_set)->CacheInterface(*it->second);
  } else if (elt_set->ElementSize() > 0) {
    CCacheInterface *ci = new CCacheInterface(*this, elt_set);
    const_cast<geo::IElementSet *>(elt_set)->CacheInterface(*ci);
    m_interfaces[elt_set] = ci;
  }
  if (elt_set->IsMeshForEntry())
    InvalidateMappings(elt_set);
}

void CCacheManager::UnregisterElementSet(const geo::IElementSet *elt_set) {
  TInterfaces::iterator it = m_interfaces.find(elt_set);
  if (it != m_interfaces.end()) {
    const_cast<geo::IElementSet *>(elt_set)->CacheInterface(geo::CCacheInterfaceDefault::Instance());
    if (it->second->NeedsDelete())
      delete it->second;
    m_interfaces.erase(it->first);
  }
  InvalidateMappings(elt_set);
}

void CCacheManager::InvalidateMappings(const geo::IElementSet *elt_set) {
  for (TInterfaces::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it)
    if (it->second->NeedsDelete())
      it->second->InvalidateMappings(elt_set);
}
