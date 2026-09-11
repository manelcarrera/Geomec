#ifndef _ValueCache_h_
#define _ValueCache_h_

class CMeshBase;

#include "MeshObserver.h"
#include "ValueCacheBase.h"
#include "MeshBase.h"

template <typename T>
  class CValueCache : public CValueCacheBase
{
public:
  typedef typename std::pair <bool, T> TValuePair;

  static TValuePair emptyValuePair;

  CValueCache(CMeshBase* mesh);

  CValueCache(const CValueCache& rhs);
  CValueCache& operator = (const CValueCache& rhs);

  virtual void clearCache();

  const TValuePair& find(const geo::INode* node) const;
  void insert(const geo::INode* node, const T& value);

  const TValuePair& find(const geo::IElement& element) const;
  void insert(const geo::IElement& element, const T& value);

private:
  void swap(CValueCache <T> aValueCache);

  size_t m_NodeSize;
  std::vector <TValuePair> m_ValueCache;
  CMeshObserver m_MeshObserver;
  bool m_Linked;
};

template <typename T>
  typename CValueCache <T> ::TValuePair CValueCache <T> ::emptyValuePair;

template <typename T>
  CValueCache <T> ::CValueCache(CMeshBase* mesh)
  : CValueCacheBase()
  , m_NodeSize(0)
  , m_ValueCache(m_NodeSize, emptyValuePair)
  , m_MeshObserver(mesh, *this)
  , m_Linked(false)
{
}

template <typename T>
  CValueCache <T> ::CValueCache(const CValueCache& rhs)
  : CValueCacheBase(rhs)
  , m_NodeSize(rhs.m_NodeSize)
  , m_ValueCache(rhs.m_ValueCache)
  , m_MeshObserver(rhs.m_MeshObserver)
  , m_Linked(rhs.m_Linked)
{
}

template <typename T>
  CValueCache <T> & CValueCache <T> ::operator = (const CValueCache <T> & rhs)
{
  CValueCache <T> temporaryValueCache(rhs);

  temporaryValueCache.swap(*this);

  return *this;
}

template <typename T>
  void CValueCache <T> ::clearCache()
{
  m_ValueCache.clear();
  m_ValueCache.resize(m_NodeSize);
}

template <typename T>
#if (_MSC_VER == 1500)
typename
#endif  // (_MSC_VER == 1500)
  const typename CValueCache <T> ::TValuePair&
  CValueCache <T> ::find(const geo::INode* node) const
{
  size_t nodeIndex = node->Index();

  if (nodeIndex < m_NodeSize)
  {
  return m_ValueCache[nodeIndex];
  }

  return emptyValuePair;
}

template <typename T>
  void CValueCache <T> ::insert(const geo::INode* node, const T& value)
{
  size_t nodeIndex = node->Index();

  if (!m_Linked && (m_MeshObserver.getMesh() != 0) &&
  m_MeshObserver.getMesh()->IsMesh())
  {
  m_MeshObserver.LinkTo(*m_MeshObserver.getMesh());
  m_Linked = true;
  }

  if (nodeIndex >= m_NodeSize)
  {
  size_t nodeSize = node->Mesh().NodeSize();

  assert(nodeSize > nodeIndex);

  while (nodeSize >= m_NodeSize)
  {
      m_NodeSize = (2 * m_NodeSize) + 1;
  }

  m_ValueCache.resize(m_NodeSize, emptyValuePair);
  }

  m_ValueCache[nodeIndex] = std::make_pair(true, value);
}

template <typename T>
#if (_MSC_VER == 1500)
typename
#endif  // (_MSC_VER == 1500)
  const typename CValueCache <T> ::TValuePair&
  CValueCache <T> ::find(const geo::IElement& element) const
{
  size_t nodeIndex = element.Index();

  if (nodeIndex < m_NodeSize)
  {
  return m_ValueCache[nodeIndex];
  }

  return emptyValuePair;
}

template <typename T>
  void CValueCache <T> ::insert(const geo::IElement& element, const T& value)
{
  size_t nodeIndex = element.Index();

  if (!m_Linked && (m_MeshObserver.getMesh() != 0) &&
  m_MeshObserver.getMesh()->IsMesh())
  {
  m_MeshObserver.LinkTo(*m_MeshObserver.getMesh());
  m_Linked = true;
  }

  if (nodeIndex >= m_NodeSize)
  {
  size_t nodeSize = element.IndexingElementSet()->ElementSize();

  assert(nodeSize > nodeIndex);

  while (nodeSize >= m_NodeSize)
  {
      m_NodeSize = (2 * m_NodeSize) + 1;
  }

  m_ValueCache.resize(m_NodeSize, emptyValuePair);
  }

  m_ValueCache[nodeIndex] = std::make_pair(true, value);
}

// private

template <typename T>
  void CValueCache <T> ::swap(CValueCache <T> aValueCache)
{
  size_t temporaryNodeSize = aValueCache.m_NodeSize;
  std::vector <TValuePair> temporaryValueCache = aValueCache.m_ValueCache;
  CMeshObserver temporaryMeshObserver = aValueCache.m_MeshObserver;
  bool temporaryLinked = aValueCache.m_Linked;

  aValueCache.m_NodeSize = m_NodeSize;
  aValueCache.m_ValueCache = m_ValueCache;
  aValueCache.m_MeshObserver = m_MeshObserver;
  aValueCache.m_Linked = m_Linked;

  m_NodeSize = temporaryNodeSize;
  m_ValueCache = temporaryValueCache;
  m_MeshObserver = temporaryMeshObserver;
  m_Linked = temporaryLinked;
}

#endif  // _ValueCache_h_
