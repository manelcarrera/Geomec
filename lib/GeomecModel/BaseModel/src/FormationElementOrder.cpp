#include "FormationElementOrder.h"
#include "ModelBase.h"

CFormationElementOrder::CFormationElementOrder(const CModelBase& modelBase,
  bool inheritFromModel)
: CElementOrder(inheritFromModel ? modelBase.ElementOrder() : EO_LINEAR)
, m_pModelBase(&modelBase)
, m_inheritFromModel(inheritFromModel)
{
}

CFormationElementOrder::~CFormationElementOrder()
{
}

CFormationElementOrder::CFormationElementOrder(
  const CFormationElementOrder& rhs)
: CElementOrder(rhs)
, m_pModelBase(rhs.m_pModelBase)
, m_inheritFromModel(rhs.m_inheritFromModel)
{
}

CFormationElementOrder& CFormationElementOrder::operator = (
  CFormationElementOrder rhs)
{
  swap(*this, rhs);

  return *this;
}

CElementOrder::TElementOrder CFormationElementOrder::elementOrder() const
{
  return inheritFromModel() ? m_pModelBase->ElementOrder() : CElementOrder::elementOrder();
}

bool CFormationElementOrder::inheritFromModel() const
{
  return m_inheritFromModel;
}

void CFormationElementOrder::inheritFromModel(bool inheritFromModel)
{
  m_inheritFromModel = inheritFromModel;
}

void CFormationElementOrder::loadStream(CStorageNode::TSTREAM& stream,
  CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  if (version >= CStreamVersion(4, 1, 44))
  {
    CElementOrder::loadStream(stream, version, progress);

    int temporaryInheritFromModel;

    stream >> temporaryInheritFromModel;

    inheritFromModel(static_cast <bool> (temporaryInheritFromModel));
  }
}

void CFormationElementOrder::saveStream(CStorageNode::TSTREAM& stream,
  CStorageNode::TPROGRESS& progress)
{
  CElementOrder::saveStream(stream, progress);

  int temporaryInheritFromModel = static_cast <int> (inheritFromModel());

  stream << temporaryInheritFromModel;
}

// private

void CFormationElementOrder::swap(CFormationElementOrder& lhs,
  CFormationElementOrder& rhs)
{
  std::swap(lhs.m_pModelBase, rhs.m_pModelBase);
  std::swap(lhs.m_inheritFromModel, rhs.m_inheritFromModel);
}
