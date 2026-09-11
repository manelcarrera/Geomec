/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
/*                                         Copyright (c) 2010 TNO DIANA BV */
#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "IObject.h"
#include "Loki.h"
#include <cassert>
#include <set>

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT CEpsilon {
public:
  double operator()() { return EPS; }
};

/*!

The abstract COctree class template is a container for objects in 3D space (e.g. points). It is meant to be
used with a tolerance value (epsilon) so objects are considered 'equal' if their coordinates (in
all 3 directions) differ less than the tolerance.

The KEY template parameter defines the type of the 3D object. The container sorts the objects based
upon this type. This class must support X(), Y() and Z() functions returning the coordinates as doubles.

The VALUE template parameter defines the type that the container should store. This type may be
identical to the KEY template parameter, but this is not necessary. A derived class must implement the
function

  virtual const KEY& key(const VALUE& val) const;

This function should return a reference to a KEY object given a VALUE object.

The optional EPSILON class should implement a function

  double operator();

which returns the tolerance (epsilon) to be used. By default the CEpsilon class is used.

The container stores the objects in a tree. Each tree node can have up to 26 children. The
subspace for the node is divided into 27 subsubspaces, 3 in each of the directions. The 3 are:
- less than the coordinate value minus the tolerance
- equal given the tolerance
- greater than the coordinate value plus the tolerance

The case where all three coordinate values are 'equal given the tolerance' is where the object
is considered equal, so there is no tree node for that case.

The tree nodes (and thus the values) are sorted in the 'subspace' order. The iterators
provided by the container iterate over the values in that same order. This means that
no assumptions can be made about the order in which the values are stored (consider the
container as 'unsorted'). The only assumption that can be made is that each iteration will
return the values in the same order unless any insert or erase operations have been performed.

If a value is erased from the container any iterators pointing to that value will become
invalid. Otherwise iterators will remain valid across insert and erase calls.

The iterators are bidirectional. Random access is not supported.

The value provided by the EPSILON class' operator() is retrieved and stored during
construction. Any change to the value returned by that operator will be ignored after
construction of the container.

*/
template <class KEY, class VALUE, class EPSILON = CEpsilon> class COctree {
private:
  template <class N_KEY, class N_VALUE, class N_EPSILON> class CNode {
  public:
    typedef COctree<N_KEY, N_VALUE, N_EPSILON> TContainer;
    typedef typename TContainer::size_type size_type;

  private:
    static const int NCHILDREN = 26;

  private:
    CNode *m_parent;
    int m_index;
    CNode *m_children[NCHILDREN];
    VALUE m_val;
    TContainer &m_cont;

  public:
    CNode(CNode *parent, int index, const N_VALUE &val, COctree<N_KEY, N_VALUE, N_EPSILON> &cont)
        : m_parent(parent), m_index(index), m_val(val), m_cont(cont) {
      assert(index >= 0 && index < NCHILDREN);
      for (int i = 0; i < NCHILDREN; ++i)
        m_children[i] = 0;
    }

    CNode(CNode &rhs) : m_parent(rhs.m_parent), m_index(rhs.m_index), m_val(rhs.m_val), m_cont(rhs.m_cont) {
      for (int i = 0; i < NCHILDREN; ++i) {
        if (rhs.m_children[i])
          m_children[i] = new CNode(this, i, rhs.m_children[i]->m_val, m_cont);
        else
          m_children[i] = 0;
      }
    }

    CNode(CNode &rhs, COctree<N_KEY, N_VALUE, N_EPSILON> &cont, CNode *parent = 0)
        : m_parent(parent), m_index(rhs.m_index), m_val(rhs.m_val), m_cont(cont) {
      for (int i = 0; i < NCHILDREN; ++i) {
        if (rhs.m_children[i])
          m_children[i] = new CNode(*rhs.m_children[i], m_cont, this);
        else
          m_children[i] = 0;
      }
    }

    ~CNode() { clear(); }

    N_VALUE &value() { return m_val; }

    CNode *find(const N_KEY &key) {
      bool bEqual;
      CNode *pNode = lowest_parent(key, bEqual);
      if (bEqual)
        return pNode;

      return 0;
    }

    std::pair<CNode *, bool> insert(const N_VALUE &val) {
      bool bEqual;
      CNode *pNode = lowest_parent(m_cont.key(val), bEqual);
      if (bEqual)
        return std::make_pair(pNode, false);

      int idxChild = pNode->childIndex(m_cont.key(val));
      assert(idxChild >= 0 && idxChild < NCHILDREN);
      assert(pNode->m_children[idxChild] == 0);
      pNode->m_children[idxChild] = new CNode(pNode, idxChild, val, m_cont);
      return std::make_pair(pNode->m_children[idxChild], true);
    }

    void erase() {
      // cannot erase root
      if (m_parent) {
        int i;

        // disconnect from parent
        i = m_parent->childIndex(*this);
        assert(i >= 0);
        m_parent->m_children[i] = 0;

        // reinsert children
        for (i = 0; i < NCHILDREN; ++i)
          if (m_children[i])
            m_parent->insert(m_children[i]->m_val);

        // die
        delete this;
      }
    }

    void clear() {
      for (int i = 0; i < NCHILDREN; ++i) {
        delete m_children[i];
        m_children[i] = 0;
      }
    }

    size_type size() const {
      size_type sz = 1;
      for (int i = 0; i < NCHILDREN; ++i)
        if (m_children[i])
          sz += m_children[i]->size();

      return sz;
    }

    CNode *begin() {
      int i;
      for (i = 0; i < NCHILDREN; ++i)
        if (m_children[i])
          return m_children[i]->begin();

      return this;
    }

    CNode *rbegin() { return this; }

    CNode *next() {
      if (m_parent) {
        int idx = m_parent->childIndex(*this);
        int i;
        for (i = idx + 1; i < NCHILDREN; ++i)
          if (m_parent->m_children[i] != 0)
            return m_parent->m_children[i]->begin();

        return m_parent;
      }

      return 0;
    }

    CNode *prev() {
      int i;
      for (i = NCHILDREN - 1; i >= 0; --i) {
        if (m_children[i] != 0)
          return m_children[i];
      }

      if (m_parent) {
        int idx = m_parent->childIndex(*this);
        for (i = idx - 1; i >= 0; --i)
          if (m_parent->m_children[i] != 0)
            return m_parent->m_children[i];

        return m_parent;
      }

      return 0;
    }

  private:
    int childIndex(CNode &node) {
      assert(node.m_parent == this);
      return node.m_index;
    }

    int childIndex(const N_KEY &key) {
      int idx = 0;
      const N_KEY &mykey = m_cont.key(m_val);

      if (m_cont.Z(key) < m_cont.Z(mykey) - m_cont.eps())
        ; // lower
      else if (m_cont.Z(key) > m_cont.Z(mykey) + m_cont.eps())
        idx += 18; // higher
      else
        idx += 9; // equal

      if (m_cont.Y(key) < m_cont.Y(mykey) - m_cont.eps())
        ; // lower
      else if (m_cont.Y(key) > m_cont.Y(mykey) + m_cont.eps())
        idx += 6; // higher
      else
        idx += 3; // equal

      if (m_cont.X(key) < m_cont.X(mykey) - m_cont.eps())
        ; // lower
      else if (m_cont.X(key) > m_cont.X(mykey) + m_cont.eps())
        idx += 2; // higher
      else
        idx += 1; // equal

      if (idx == 13)
        idx = -1; // equal values
      else if (idx > 13)
        --idx;

      assert(idx < NCHILDREN);
      return idx;
    }
    /*
      CNode* child(const KEY& key)
      {
          return m_children[childIndex(key)];
      }
    */
    CNode *lowest_parent(const N_KEY &key, bool &bEqual) {
      int idx = childIndex(key);
      if (idx == -1) {
        bEqual = true;
        return this;
      }

      CNode *pChild = m_children[idx];
      if (pChild)
        return pChild->lowest_parent(key, bEqual);

      bEqual = false;
      return this;
    }
  };

public:
  template <class CI_KEY, class CI_VALUE, class CI_EPSILON> class CConstIterator {
  public:
    typedef typename std::set<CI_KEY>::iterator::iterator_category iterator_category;
    typedef typename std::set<CI_KEY>::iterator::difference_type difference_type;
    typedef typename ::Loki::TypeTraits<CI_KEY>::ReferencedType ReferencedType;
    typedef ReferencedType reference;
    typedef reference *pointer;
    typedef CI_VALUE value_type;
    typedef CNode<CI_KEY, CI_VALUE, CI_EPSILON> TNode;
    typedef COctree<CI_KEY, CI_VALUE, CI_EPSILON> TContainer;

  public:
    CConstIterator() : m_pNode(0), m_pCont(0) {}

    CConstIterator(TNode *node, TContainer &cont) : m_pNode(node), m_pCont(&cont) {}

    CConstIterator(const CConstIterator &it) : m_pNode(it.m_pNode), m_pCont(it.m_pCont) {}

    CConstIterator &operator=(const CConstIterator &it) {
      m_pNode = it.m_pNode;
      m_pCont = it.m_pCont;
      return *this;
    }

    bool operator==(const CConstIterator &itRight) {
      return (container() == itRight.container() && Node() == itRight.Node());
    }

    bool operator!=(const CConstIterator &itRight) { return !operator==(itRight); }

    CConstIterator &operator++() // pre-increment
    {
      m_pNode = m_pNode->next();
      return *this;
    }

    CConstIterator operator++(int) // post-increment
    {
      CConstIterator it(*this);
      ++(*this);
      return it;
    }

    CConstIterator &operator--() // pre-decrement
    {
      m_pNode = m_pNode->prev();
      return *this;
    }

    CConstIterator operator--(int) // post-decrement
    {
      CConstIterator it(*this);
      --(*this);
      return it;
    }

    CI_VALUE &operator*() { return m_pNode->value(); }

    CI_VALUE *operator->() { return &m_pNode->value(); }

    TNode *Node() const { return m_pNode; }

    TContainer *container() const { return m_pCont; }

  private:
    TNode *m_pNode;
    TContainer *m_pCont;
  };

  template <class I_KEY, class I_VALUE, class I_EPSILON>
  class CIterator : public CConstIterator<I_KEY, I_VALUE, I_EPSILON> {
  public:
    typedef CConstIterator<I_KEY, I_VALUE, I_EPSILON> TBase;
    typedef typename TBase::TNode TNode;
    typedef typename TBase::TContainer TContainer;

  public:
    CIterator() {}

    CIterator(TNode *node, TContainer &cont) : TBase(node, cont) {}

    CIterator(const CIterator &it) : TBase(it) {}

    CIterator &operator=(const CIterator &it) {
      TBase::operator=(it);
      return *this;
    }

    using TBase::operator==;
    using TBase::operator!=;

    CIterator &operator++() // pre-increment
    {
      TBase::operator++();
      return *this;
    }

    CIterator operator++(int) // post-increment
    {
      CIterator it(*this);
      ++(*this);
      return it;
    }

    CIterator &operator--() // pre-decrement
    {
      TBase::operator--();
      return *this;
    }

    CIterator operator--(int) // post-decrement
    {
      CIterator it(*this);
      --(*this);
      return it;
    }
  };

  template <class CRI_KEY, class CRI_VALUE, class CRI_EPSILON> class CConstReverseIterator {
  public:
    typedef typename std::set<CRI_KEY>::iterator::iterator_category iterator_category;
    typedef typename std::set<CRI_KEY>::iterator::difference_type difference_type;
    typedef typename ::Loki::TypeTraits<CRI_KEY>::ReferencedType ReferencedType;
    typedef ReferencedType reference;
    typedef reference *pointer;
    typedef CRI_VALUE value_type;
    typedef CNode<CRI_KEY, CRI_VALUE, CRI_EPSILON> TNode;
    typedef COctree<CRI_KEY, CRI_VALUE, CRI_EPSILON> TContainer;

  public:
    CConstReverseIterator() : m_pNode(0), m_pCont(0) {}

    CConstReverseIterator(TNode *node, TContainer &cont) : m_pNode(node), m_pCont(&cont) {}

    CConstReverseIterator(const CConstReverseIterator &it) : m_pNode(it.m_pNode), m_pCont(it.m_pCont) {}

    CConstReverseIterator(const CConstIterator<CRI_KEY, CRI_VALUE, CRI_EPSILON> &it) : m_pCont(it.container()) {
      if (it.Node())
        m_pNode = it.Node()->prev();
      else
        m_pNode = m_pCont->rbegin().Node();
    }

    CConstReverseIterator &operator=(const CConstReverseIterator &it) {
      m_pNode = it.m_pNode;
      m_pCont = it.m_pCont;
      return *this;
    }

    bool operator==(const CConstReverseIterator &it) { return (m_pCont == it.m_pCont && m_pNode == it.m_pNode); }

    bool operator!=(const CConstReverseIterator &it) { return !operator==(it); }

    CConstReverseIterator &operator++() // pre-increment
    {
      m_pNode = m_pNode->prev();
      return *this;
    }

    CConstReverseIterator operator++(int) // post-increment
    {
      CConstReverseIterator it(*this);
      ++(*this);
      return it;
    }

    CConstReverseIterator &operator--() // pre-decrement
    {
      m_pNode = m_pNode->next();
      return *this;
    }

    CConstReverseIterator operator--(int) // post-decrement
    {
      CConstReverseIterator it(*this);
      --(*this);
      return it;
    }

    CRI_VALUE &operator*() { return m_pNode->value(); }

    CRI_VALUE *operator->() { return &m_pNode->value(); }

    TNode *Node() { return m_pNode; }

    CConstIterator<CRI_KEY, CRI_VALUE, CRI_EPSILON> base() {
      if (!m_pNode)
        return CConstIterator<CRI_KEY, CRI_VALUE, CRI_EPSILON>(m_pCont->begin(), m_pCont);

      return CConstIterator<CRI_KEY, CRI_VALUE, CRI_EPSILON>(m_pNode->next(), m_pCont);
    }

    TContainer *container() const { return m_pCont; }

  private:
    TNode *m_pNode;
    TContainer *m_pCont;
  };

  template <class RI_KEY, class RI_VALUE, class RI_EPSILON>
  class CReverseIterator : public CConstReverseIterator<RI_KEY, RI_VALUE, RI_EPSILON> {
  public:
    typedef CConstReverseIterator<RI_KEY, RI_VALUE, RI_EPSILON> TBase;
    typedef typename TBase::TNode TNode;
    typedef typename TBase::TContainer TContainer;

  public:
    CReverseIterator() {}

    CReverseIterator(TNode *node, TContainer &cont) : TBase(node, cont) {}

    CReverseIterator(const CIterator<RI_KEY, RI_VALUE, RI_EPSILON> &it) : TBase(it) {}

    CReverseIterator &operator=(const CReverseIterator &it) {
      TBase::operator=(it);
      return *this;
    }

    using TBase::operator==;
    using TBase::operator!=;

    CReverseIterator &operator++() // pre-increment
    {
      TBase::operator++();
      return *this;
    }

    CReverseIterator operator++(int) // post-increment
    {
      CReverseIterator it(*this);
      ++(*this);
      return it;
    }

    CReverseIterator &operator--() // pre-decrement
    {
      TBase::operator--();
      return *this;
    }

    CReverseIterator operator--(int) // post-decrement
    {
      CReverseIterator it(*this);
      --(*this);
      return it;
    }

    CIterator<RI_KEY, RI_VALUE, RI_EPSILON> base() {
      if (!this->Node())
        return CIterator<RI_KEY, RI_VALUE, RI_EPSILON>(this->container()->begin(), this->container());

      return CIterator<RI_KEY, RI_VALUE, RI_EPSILON>(this->Node()->next(), this->container());
    }
  };

  typedef CIterator<KEY, VALUE, EPSILON> iterator;
  typedef CConstIterator<KEY, VALUE, EPSILON> const_iterator;
  typedef CReverseIterator<KEY, VALUE, EPSILON> reverse_iterator;
  typedef CConstReverseIterator<KEY, VALUE, EPSILON> const_reverse_iterator;
  typedef unsigned int size_type;
  typedef typename ::Loki::TypeTraits<KEY>::ReferencedType ReferencedType;

private:
  typedef CNode<KEY, VALUE, EPSILON> TNode;

public:
  COctree() : m_pRoot(0), m_eps(EPSILON()()) {}

  COctree(const COctree &rhs) : m_pRoot(0), m_eps(EPSILON()()) { *this = rhs; }

  virtual ~COctree() {}

  COctree &operator=(const COctree &rhs) {
    assert(fabs(m_eps - rhs.m_eps) < m_eps * 1e-4);
    if (rhs.m_pRoot)
      m_pRoot = new TNode(*rhs.m_pRoot, *this);

    return *this;
  }

  bool operator<(const COctree &rhs) const {
    if (size() < rhs.size())
      return true;

    if (empty() && rhs.empty())
      return false;

    const_iterator itSelf;
    const_iterator itRhs;

    for (itSelf = begin(), itRhs = rhs.begin(); itSelf != end() && itRhs != rhs.end(); ++itRhs, ++itSelf) {
      const KEY &kSelf = key(*itSelf);
      const KEY &kRhs = key(*itRhs);

      if (X(kSelf) < X(kRhs) - m_eps)
        return true;
      else if (X(kSelf) > X(kRhs) + m_eps)
        return false;

      if (Y(kSelf) < Y(kRhs) - m_eps)
        return true;
      else if (Y(kSelf) > Y(kRhs) + m_eps)
        return false;

      if (Z(kSelf) < Z(kRhs) - m_eps)
        return true;
    }

    return false;
  }

  std::pair<iterator, bool> insert(const VALUE &val) {
    if (m_pRoot) {
      std::pair<TNode *, bool> prInsert = m_pRoot->insert(val);
      return std::make_pair(iterator(prInsert.first, *this), prInsert.second);
    }

    m_pRoot = new TNode(0, 0, val, *this);
    return std::make_pair(iterator(m_pRoot, *this), true);
  }

  iterator find(const KEY &key) {
    if (m_pRoot) {
      TNode *pNode = m_pRoot->find(key);
      if (!pNode)
        return end();

      return iterator(pNode, *this);
    }

    return end();
  }

  const_iterator find(const KEY &key) const { return (const_cast<COctree *>(this))->find(key); }

  void erase(iterator pos) {
    TNode *pNode = pos.Node();
    pos.Node()->erase();
    if (pNode == m_pRoot)
      m_pRoot = 0;
    // node was deleted, pos is invalid now !
  }

  void clear() {
    if (m_pRoot)
      m_pRoot->clear();

    delete m_pRoot;
    m_pRoot = 0;
  }

  size_type size() const {
    if (m_pRoot)
      return m_pRoot->size();

    return 0;
  }

  bool empty() const { return m_pRoot == 0; }

  iterator begin() {
    if (m_pRoot)
      return iterator(m_pRoot->begin(), *this);

    return end();
  }

  iterator end() {
    TNode *pEnd = 0;
    return iterator(pEnd, *this);
  }

  const_iterator begin() const { return (const_cast<COctree *>(this))->begin(); }

  const_iterator end() const { return (const_cast<COctree *>(this))->end(); }

  reverse_iterator rbegin() {
    if (m_pRoot)
      return reverse_iterator(m_pRoot->rbegin(), *this);

    return rend();
  }

  reverse_iterator rend() {
    TNode *pEnd = 0;
    return reverse_iterator(pEnd, *this);
  }

  const_reverse_iterator rbegin() const { return (const_cast<COctree *>(this))->rbegin(); }

  const_reverse_iterator rend() const { return (const_cast<COctree *>(this))->end(); }

  double X(const KEY &key) const {
    const ReferencedType &v = ::Loki::TypeTraits<KEY>::GetReference(key);
    return v.X();
  }

  double Y(const KEY &key) const {
    const ReferencedType &v = ::Loki::TypeTraits<KEY>::GetReference(key);
    return v.Y();
  }

  double Z(const KEY &key) const {
    const ReferencedType &v = ::Loki::TypeTraits<KEY>::GetReference(key);
    return v.Z();
  }

  virtual const KEY &key(const VALUE &val) const = 0;
  double eps() const { return m_eps; }

private:
  TNode *m_pRoot;
  double m_eps;
};

} // namespace geo

#endif // _OCTREE_H_
