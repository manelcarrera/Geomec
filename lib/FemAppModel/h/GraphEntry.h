// GraphEntry.h: interface for the CGraphEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHENTRY_H__0E06AD2D_EA02_43C4_A64D_57769AC93737__INCLUDED_)
#define AFX_GRAPHENTRY_H__0E06AD2D_EA02_43C4_A64D_57769AC93737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>

#include "GraphNode.h"

// Entry point into a GeoGraph
//
// Since in CGeoGraphItem there is a precondition that ppLinks
// is never empty ppLinks is initialised with 0. This breaks
// the GeoItem Idiom but is required
// ##ModelId=3B653D0202A1
class CFemAppModel;
class CGraphEntry : public CGraphNode {
  const unsigned int m_uIconId;
  const int m_nEntryId;

public:
  typedef std::set<CGraphNode *, CGraphNode::CPtrLess> TEntryNodeSet;
  int EntryId() const;
  virtual TEntryNodeSet GraphEntryNodes() const = 0;

  // Determs or the graph has an unique name in the entry
  virtual bool UniqueName(const QString &sName) const;

  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;

  CGraphEntry(int nEntryId, unsigned int uIconId, const QString &strName, CFemAppModel &model);
  CGraphEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model);
  virtual ~CGraphEntry();

  CFemAppModel &Model();
  const CFemAppModel &Model() const;

  virtual void OnIndexChanged(CGraphNode &node) = 0;

  QString createUniqueName(const QString &defaultName, size_t initialNumber) const;
  std::pair<QString, QString> createUniqueNames(const QString &firstName, const QString &secondName,
                                                size_t initialNumber) const;

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitGraphEntry(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitGraphEntry(*this); }
};

template <class T> class CGraphEntryTemp : public CGraphEntry {
public:
  typedef std::set<T *, CGraphNode::CPtrLess> TNodeSet;
  typedef std::set<T *, CGraphNode::CLess> TSortedNodeSet;
  typedef std::map<int, T *> TIndexMap;
  typedef std::map<int, std::set<CGraphNode *>> TDelayedLinkMap;
  typedef T value_type;

private:
  TNodeSet m_stNode;
  TIndexMap m_mpIndex;
  TDelayedLinkMap m_mpDelayedLink;

public:
  // Returns all entry nodes ...
  virtual TEntryNodeSet GraphEntryNodes() const {
    TEntryNodeSet stRet;

    for (typename TNodeSet::const_iterator it = m_stNode.begin(); it != m_stNode.end(); it++)
      stRet.insert(*it);

    return stRet;
  }

  T *FindIndex(int nIndex) {
    typename TIndexMap::iterator it = m_mpIndex.find(nIndex);
    if (it != m_mpIndex.end()) {
      assert(it->first == it->second->Index());
      return it->second;
    }
    return 0;
  }

  // returns true when actually linked, false when not (yet) linked
  bool LinkNodeToIndex(CGraphNode &node, int nIndex) {
    // try to link the node to the index, but delay if index is not available yet (e.g. while loading)
    T *pObject = FindIndex(nIndex);
    if (pObject) {
      node.LinkTo(*pObject);
      return true;
    }

    // delay link
    TDelayedLinkMap::iterator it =
        m_mpDelayedLink.insert(TDelayedLinkMap::value_type(nIndex, std::set<CGraphNode *>())).first;
    it->second.insert(&node);
    return false;
  }

  const TSortedNodeSet SortedEntryNodes() const {
    TSortedNodeSet stRet;

    for (typename TNodeSet::const_iterator it = m_stNode.begin(); it != m_stNode.end(); it++)
      stRet.insert(*it);

    return stRet;
  }

  const TNodeSet &EntryNodes() const { return m_stNode; }

  CGraphEntryTemp(int nEntryId, unsigned int uIconId, const QString &strName, CFemAppModel &model)
      : CGraphEntry(nEntryId, uIconId, strName, model) {
    return;
#if 0
    assert(false);
    OnNewNeighbour(*this);
    OnNeighbourDeleted(*this);
#endif
  }

  CGraphEntryTemp(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model)
      : CGraphEntry(nEntryId, uIconId, uNameId, model) {
    return;
#if 0
    assert(false);
    OnNewNeighbour(*this);
    OnNeighbourDeleted(*this);
#endif
  }

  ~CGraphEntryTemp() {
    while (m_stNode.begin() != m_stNode.end()) {
      delete *m_stNode.begin();
      // due to sideeffects of the delete mentioned above we have to determine the entrynodes over and over again. (some
      // other element can also be destroyed)
    }
  }

  virtual void OnNewNeighbour(const CGraphNode &node) {
    const T *pItem = dynamic_cast<const T *>(&node);
    if (pItem) {
      m_stNode.insert((T *)pItem);

      DelayedLink((T &)*pItem);

      m_mpIndex[pItem->Index()] = const_cast<T *>(pItem);
      Modified();
    }

    CGraphNode::OnNewNeighbour(node);
  }

  virtual void OnNeighbourDeleted(const CGraphNode &node) {
    typename TNodeSet::iterator it = m_stNode.find((T *)&node);
    if (it != m_stNode.end()) {
      while (*it != &node)
        it++;
      m_stNode.erase(it);
      const T *pItem = dynamic_cast<const T *>(&node);
      if (pItem)
        m_mpIndex.erase(pItem->Index());
      Modified();
    }

    CGraphNode::OnNeighbourDeleted(node);
  }

  virtual void OnIndexChanged(CGraphNode &node) {
    // delayed linking?
    T *pItem = dynamic_cast<T *>(&node);
    if (pItem) {
      DelayedLink(*pItem);
      for (typename TIndexMap::iterator it = m_mpIndex.begin(); it != m_mpIndex.end(); ++it) {
        if (it->second == pItem) {
          m_mpIndex.erase(it);
          break;
        }
      }

      m_mpIndex[pItem->Index()] = pItem;
    }
  }

  virtual bool Destroy() {
    TNodeSet stNodes = EntryNodes();
    for (typename TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); it++)
      if ((*it)->CanDestroy())
        (*it)->Destroy();

    return true;
  }

  virtual bool CanDestroy() const {
    for (typename TNodeSet::const_iterator it = m_stNode.begin(); it != m_stNode.end(); it++)
      if ((*it)->CanDestroy())
        return true;

    return false;
  }

private:
  void DelayedLink(T &item) {
    int nIndex = item.Index();
    TDelayedLinkMap::iterator it = m_mpDelayedLink.find(nIndex);
    if (it != m_mpDelayedLink.end()) {
      // link all subscribed nodes
      std::set<CGraphNode *>::iterator its;
      for (its = it->second.begin(); its != it->second.end(); ++its)
        (*its)->LinkTo(item);

      // done, remove this index from the delayed link map
      m_mpDelayedLink.erase(it);
    }
  }
};

#endif // !defined(AFX_GRAPHENTRY_H__0E06AD2D_EA02_43C4_A64D_57769AC93737__INCLUDED_)
