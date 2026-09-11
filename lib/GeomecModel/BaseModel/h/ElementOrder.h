#ifndef _ElementOrder_h_
#define _ElementOrder_h_

#include "StorageNode.h"

class CElementOrder {
public:
  enum TElementOrder {
    EO_LINEAR,
    EO_QUADRATIC,
    EO_INIQUAD // initialize with quadratic, then switch to linear
  };

  CElementOrder(TElementOrder elementOrder);
  virtual ~CElementOrder();

  CElementOrder(const CElementOrder &rhs);
  CElementOrder &operator=(CElementOrder rhs);

  TElementOrder elementOrder() const;
  void elementOrder(TElementOrder elementOrder);

  void loadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
  void saveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);

private:
  void swap(CElementOrder &lhs, CElementOrder &rhs);

  TElementOrder m_elementOrder;
};

#endif // _ElementOrder_h_
