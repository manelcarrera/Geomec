#ifndef _GlobalValueObserver_h_
#define _GlobalValueObserver_h_

class CGlobalValue;
class CValue;

template <typename V, typename G> class CGlobalValueObserver : public CGraphNode {
public:
  CGlobalValueObserver(V *pressure, G *globalValue);

  CGlobalValueObserver(const CGlobalValueObserver &rhs);
  CGlobalValueObserver &operator=(const CGlobalValueObserver &rhs);

  virtual ~CGlobalValueObserver();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, unsigned int hint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

private:
  void swap(CGlobalValueObserver &aGlobalValueObserver);

  V *m_Value;
  G *m_GlobalValue;
};

template <typename V, typename G>
CGlobalValueObserver<V, G>::CGlobalValueObserver(V *value, G *globalValue)
    : CGraphNode(""), m_Value(value), m_GlobalValue(globalValue) {
  LinkTo(*m_GlobalValue);
}

template <typename V, typename G>
CGlobalValueObserver<V, G>::CGlobalValueObserver(const CGlobalValueObserver<V, G> &rhs)
    : CGraphNode(rhs), m_Value(rhs.m_Value), m_GlobalValue(rhs.m_GlobalValue) {}

template <typename V, typename G>
CGlobalValueObserver<V, G> &CGlobalValueObserver<V, G>::operator=(const CGlobalValueObserver &rhs) {
  CGlobalValueObserver temporaryGlobalValueObserver(rhs);

  temporaryGlobalValueObserver.swap(*this);

  return *this;
}

template <typename V, typename G> CGlobalValueObserver<V, G>::~CGlobalValueObserver() {
  /*
   * The destructor of the base class CGraphNode will take care of un-linking
   * this observer from the subject m_GlobalValue.
   */
}

template <typename V, typename G> unsigned int CGlobalValueObserver<V, G>::IconId() const { return 0; }

template <typename V, typename G> unsigned int CGlobalValueObserver<V, G>::TypeId() const { return 0; }

template <typename V, typename G> void CGlobalValueObserver<V, G>::OnNewNeighbour(const CGraphNode & /*node*/) {}

template <typename V, typename G>
void CGlobalValueObserver<V, G>::OnNeighbourModified(const CGraphNode & /*node*/, unsigned int /*hint*/) {
  m_Value->Component().Modified();
}

template <typename V, typename G> void CGlobalValueObserver<V, G>::OnNeighbourDeleted(const CGraphNode & /*node*/) {}

// private

template <typename V, typename G> void CGlobalValueObserver<V, G>::swap(CGlobalValueObserver &aGlobalValueObserver) {
  V *temporaryValue = aGlobalValueObserver.m_Value;
  G *temporaryGlobalValue = aGlobalValueObserver.m_GlobalValue;

  aGlobalValueObserver.m_Value = m_Value;
  aGlobalValueObserver.m_GlobalValue = m_GlobalValue;

  m_Value = temporaryValue;
  m_GlobalValue = temporaryGlobalValue;
}

#endif // _GlobalValueObserver_h_
