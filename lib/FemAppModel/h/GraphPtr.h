// GraphPtr.h: interface for the GraphPtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHPTR_H__CBA677BD_6E92_48CD_BCBB_4DD043EBF6D6__INCLUDED_)
#define AFX_GRAPHPTR_H__CBA677BD_6E92_48CD_BCBB_4DD043EBF6D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphNode.h"

template <class T> class CGraphPtr : public CGraphNode {
  T *m_pTr;

public:
  CGraphPtr(const QString &sName = "") : CGraphNode(sName), m_pTr(0) {}

  CGraphPtr(T *pTr, const QString &sName = "") : CGraphNode(sName), m_pTr(0) {
    LinkTo(*m_pTr);
    assert(m_pTr == 0);
  }

  virtual void Clear() {
    if (m_pTr) {
      UnLink(*m_pTr);
      assert(m_pTr == 0);
    }
  }

  virtual void OnNeighbourDeleted(const CGraphNode &node) {
    if (&node == m_pTr)
      m_pTr = 0;
  }

  virtual void OnNewNeighbour(const CGraphNode &node) {
    const T *pTr = dynamic_cast<const T *>(&node);
    if (pTr) {
      if (m_pTr)
        UnLink(*m_pTr);

      assert(m_pTr == 0);

      m_pTr = const_cast<T *>(pTr);
    }
  }

  virtual unsigned int TypeId() const {
    assert(false);
    return 0;
  }

  virtual unsigned int IconId() const {
    assert(false);
    return 0;
  }

  const T *Ptr() const { return m_pTr; }

  T *Ptr() { return m_pTr; }
};

#endif // !defined(AFX_GRAPHPTR_H__CBA677BD_6E92_48CD_BCBB_4DD043EBF6D6__INCLUDED_)
