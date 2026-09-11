#include "MaterialMappingCache.h"

#include "Material.h"
#include "IMaterialServerBase.h"


CMaterialKey::CMaterialKey()
  : m_pValueMap(0)
{
}

CMaterialKey::CMaterialKey(TValueMap *values)
  : m_pValueMap(values)
{
}

void CMaterialKey::SetMap(TValueMap *values)
{
  m_internalValueMap = *values;
  m_pValueMap = 0;
}

bool CMaterialKey::operator<(const CMaterialKey& rhs) const
{
  const TValueMap *l = m_pValueMap ? m_pValueMap : &m_internalValueMap;
  const TValueMap *r = rhs.m_pValueMap ? rhs.m_pValueMap : &rhs.m_internalValueMap;

  return std::lexicographical_compare(l->begin(), l->end(), r->begin(), r->end());
}

CMaterialMappingCache::CMaterialMappingCache()
  : m_bUseCache(false)
  , m_bNoCaching(false)
{
}

CMaterialMappingCache::~CMaterialMappingCache()
{
}


CMaterialMappingCache* CMaterialMappingCache::instance()
{
  static CMaterialMappingCache _instance;
  return &_instance;
}

bool CMaterialMappingCache::UseCache() const
{
  return m_bUseCache;
}

void CMaterialMappingCache::UseCache(bool value)
{
  if (!value)
  {
    for (size_t i = 0; i < m_vcCache.size(); ++i)
    {
      m_vcCache[i].server->AddToCache(m_vcCache[i].key, m_vcCache[i].material);
    }
    m_vcCache.clear();
  }

  m_bUseCache = value;
}

bool CMaterialMappingCache::NoCaching() const
{
  return m_bNoCaching;
}

void CMaterialMappingCache::NoCaching(bool value)
{
  if (!value)
  {
    for (size_t i = 0; i < m_vcCache.size(); ++i)
    {
      delete m_vcCache[i].material;
    }
    m_vcCache.clear();
  }

  m_bNoCaching = value;
}

void CMaterialMappingCache::Add(const IMaterialServerBase *server, CMaterialKey key, CFFMaterial *material)
{
  ServerKeyMaterial skm;
  skm.server = server;
  skm.key = key;
  skm.material = material;

  m_vcCache.push_back(skm);
}


