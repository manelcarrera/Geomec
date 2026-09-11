#ifndef _FormationElementOrder_h_
#define _FormationElementOrder_h_

class CModelBase;

#include "ElementOrder.h"

class CFormationElementOrder : public CElementOrder
{
  public:
  CFormationElementOrder(const CModelBase& modelBase, bool inheritFromModel);
  virtual ~CFormationElementOrder();

  CFormationElementOrder(const CFormationElementOrder& rhs);
  CFormationElementOrder& operator = (CFormationElementOrder rhs);

  TElementOrder elementOrder() const;
  using CElementOrder::elementOrder;

  bool inheritFromModel() const;
  void inheritFromModel(bool inheritFromModel);

  void loadStream(CStorageNode::TSTREAM& stream,
      CStreamVersion& version, CStorageNode::TPROGRESS& progress);
  void saveStream(CStorageNode::TSTREAM& stream,
      CStorageNode::TPROGRESS& progress);

  private:
  void swap(CFormationElementOrder& lhs, CFormationElementOrder& rhs);

  const CModelBase *m_pModelBase;
  bool m_inheritFromModel;
};

#endif  // _FormationElementOrder_h_
