#include <algorithm>

#include "ElementOrder.h"

CElementOrder::CElementOrder(TElementOrder elementOrder) : m_elementOrder(elementOrder) {}

CElementOrder::~CElementOrder() {}

CElementOrder::CElementOrder(const CElementOrder &rhs) : m_elementOrder(rhs.m_elementOrder) {}

CElementOrder &CElementOrder::operator=(CElementOrder rhs) {
  swap(*this, rhs);

  return *this;
}

CElementOrder::TElementOrder CElementOrder::elementOrder() const { return m_elementOrder; }

void CElementOrder::elementOrder(TElementOrder elementOrder) { m_elementOrder = elementOrder; }

void CElementOrder::loadStream(CStorageNode::TSTREAM &stream, CStreamVersion &, CStorageNode::TPROGRESS &) {
  int temporaryElementOrder;

  stream >> temporaryElementOrder;

  elementOrder(static_cast<TElementOrder>(temporaryElementOrder));
}

void CElementOrder::saveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &) {
  int temporaryElementOrder = static_cast<int>(elementOrder());

  stream << temporaryElementOrder;
}

// private

void CElementOrder::swap(CElementOrder &lhs, CElementOrder &rhs) { std::swap(lhs.m_elementOrder, rhs.m_elementOrder); }
