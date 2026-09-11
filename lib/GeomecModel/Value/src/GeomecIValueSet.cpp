// GeomecIValueSet.cpp: implementation of the IValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "GeomecIValueSet.h"
#include "IPointSet.h"
#include "ValueComponent.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IValueSet::IValueSet(IPointSet &point_set) : CStorageNode(point_set.Model()), m_pPointSet(&point_set), m_pComponent(0) {
  assert(!point_set.IsCopy());
  reParent(&point_set);
  assert(IsLinkedTo(PointSet()));
}

IValueSet::IValueSet(const QString &sName, CQuantity::UNIT unit, IPointSet &point_set)
    : CStorageNode(sName, point_set.Model()), m_pPointSet(&point_set), m_pComponent(0), m_unit(unit) {
  // Link to the source point always
  if (point_set.IsCopy())
    m_pPointSet = (IPointSet *)&point_set.Source();
  else {
    m_pPointSet = &point_set;
    reParent(&point_set);
    assert(IsLinkedTo(PointSet()));
  }
}

IValueSet::IValueSet(const IValueSet &rhs)
    : CStorageNode(rhs), m_pPointSet(rhs.m_pPointSet), m_pComponent(rhs.m_pComponent), m_unit(rhs.m_unit) {
  if (Component()) {
    assert(Component()->IsCopy());
    assert(IsLinkedTo(*Component()));
  }

  assert(PointSet().IsCopy());
  if (!IsLinkedTo(PointSet()))
    reParent(&PointSet());

  m_Time.Set(rhs.m_Time.GetYear(), rhs.m_Time.GetMonth());
}

IValueSet::~IValueSet() {
  if (IsCopy()) {
    if (m_pComponent && m_pComponent->IsCopied())
      delete &m_pComponent->Copy();

    assert(m_pComponent == 0);
  }
}

void IValueSet::FileIndex(int iIndex) { Index(iIndex); }

bool IValueSet::operator==(const IValueSet &rhs) const {
  if (!CStorageNode::operator==(rhs))
    return false;

  return ((m_pComponent == rhs.m_pComponent) && (m_unit == rhs.m_unit));
}

IValueSet &IValueSet::operator=(const IValueSet &rhs) {
  CStorageNode::operator=(rhs);

  m_unit = rhs.m_unit;
  m_pComponent = rhs.m_pComponent;
  m_pPointSet = rhs.m_pPointSet;

  Modified();
  return *this;
}

void IValueSet::OnNewNeighbour(const CGraphNode &node) {
  const CValueComponent *pComponent = dynamic_cast<const CValueComponent *>(&node);
  if (pComponent) {
    if (m_pComponent)
      UnLink(*m_pComponent);

    m_pComponent = const_cast<CValueComponent *>(pComponent);
  }

  CStorageNode::OnNewNeighbour(node);
}

void IValueSet::OnNeighbourDeleted(const CGraphNode &item) {
  // Is it an owning type?
  if (&item == m_pComponent) {
    m_pComponent = 0;
  }

  // Is it the vertexset?
  if ((&item == m_pPointSet) && (!IsCopy())) {
    // Kill your self ...
    CStorageNode::OnNeighbourDeleted(item);
    delete this;
    return;
  }

  CStorageNode::OnNeighbourDeleted(item);
}

const IPointSet &IValueSet::PointSet() const {
  assert(!m_pPointSet->IsCopy());
  if (IsCopy() && m_pPointSet->IsCopied())
    return (const IPointSet &)(m_pPointSet->Copy());

  return *m_pPointSet;
}

IPointSet &IValueSet::PointSet() {
  assert(!m_pPointSet->IsCopy());
  if (IsCopy() && m_pPointSet->IsCopied())
    return (IPointSet &)(m_pPointSet->Copy());

  return *m_pPointSet;
}

const CValueComponent *IValueSet::Component() const {
  if (m_pComponent && m_pComponent->IsCopied())
    return (const CValueComponent *)(&m_pComponent->Copy());

  return m_pComponent;
}

CValueComponent *IValueSet::Component() {
  if (m_pComponent && m_pComponent->IsCopied())
    return (CValueComponent *)(&m_pComponent->Copy());

  return m_pComponent;
}

CQuantity::UNIT IValueSet::Unit() const { return m_unit; }

void IValueSet::Unit(CQuantity::UNIT unit) { m_unit = unit; }

bool IValueSet::Empty() const { return false; }
/*
bool IValueSet::PointInConvexHull(const geo::IPoint& pt) const
{
  return PointSet().PointInConvexHull(pt);
}


*/

bool IValueSet::PrepareMapping(const geo::IElementSet *pElementSet) {
  if (!m_pPointSet->PrepareMapping(pElementSet))
    return false;
  return true;
}