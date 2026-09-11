#ifndef _GraphPtr_Delegate_h_
#define _GraphPtr_Delegate_h_

#include "GraphNode_Delegate.h"
#include "GraphPtr.h"

template <class T> class CGraphPtr_Delegate : public CGraphNode_Delegate {
public:
  CGraphPtr_Delegate(CGraphPtr<T> *graphPtr);

private:
  CGraphPtr_Delegate(const CGraphPtr_Delegate &rhs);
  CGraphPtr_Delegate &operator=(const CGraphPtr_Delegate &rhs);

  CGraphPtr<T> *m_graphPtr;

  REGISTER_DELEGATE(CGraphPtr<T>, CGraphPtr_Delegate<T>);
};

template <class T>
CGraphPtr_Delegate<T>::CGraphPtr_Delegate(CGraphPtr<T> *graphPtr)
    : CGraphNode_Delegate(graphPtr), m_graphPtr(graphPtr) {
  ACTIVATE_TEMPLATE_DELEGATE(CGraphPtr<T>, CGraphPtr_Delegate<T>);
}

#endif // _GraphPtr_Delegate_h_
