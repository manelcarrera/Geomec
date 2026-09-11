#ifndef _MATERIAL_MAPPING_CACHE_
#define _MATERIAL_MAPPING_CACHE_

#include <map>
#include <tbb/concurrent_vector.h>

class CFFMaterial;
class IMaterialServerBase;

class CMaterialKey {
public:
  typedef std::map<unsigned int, double> TValueMap;

  CMaterialKey();
  CMaterialKey(TValueMap *values);

  void SetMap(TValueMap *values);

  bool operator<(const CMaterialKey &rhs) const;

private:
  TValueMap *m_pValueMap;
  TValueMap m_internalValueMap;
};

class CMaterialMappingCache {

  CMaterialMappingCache();
  ~CMaterialMappingCache();
  CMaterialMappingCache(const CMaterialMappingCache &);
  CMaterialMappingCache &operator=(const CMaterialMappingCache &);

public:
  typedef std::map<CMaterialKey, CFFMaterial *> TMaterialMap;

  static CMaterialMappingCache *instance();

  typedef enum { NORMAL, CIRCUMVENT } eCacheType;

  bool UseCache() const;
  void UseCache(bool value);

  // with no caching the caller can check whether it should cache, AND store material pointers here for deletion
  bool NoCaching() const;
  void NoCaching(bool value);

  void Add(const IMaterialServerBase *server, CMaterialKey key, CFFMaterial *mat);

private:
  struct ServerKeyMaterial {
    const IMaterialServerBase *server;
    CMaterialKey key;
    CFFMaterial *material;
  };

  bool m_bUseCache;
  bool m_bNoCaching;
  tbb::concurrent_vector<ServerKeyMaterial> m_vcCache;
};

#endif
