
#include "MeasuredTopDisplacementsNode.h"
#include "3dmodel.h"
#include "3DHorizon.h"
#include "FemAppEntryTypes.h"
#include "ValueVector.h"

CMeasuredTopDisplacementsNode::CMeasuredTopDisplacementsNode(const QString& sName, C3DModel& model)
: CStorageNode(sName, model),
  m_pDisplaVector(0)
{
}

void CMeasuredTopDisplacementsNode::OnNewNeighbour(const CGraphNode &node)
{
  const TDisplacementVector* pDisplaVector = dynamic_cast<const TDisplacementVector*>(&node);
  if(pDisplaVector)
  {
    if(m_pDisplaVector)
      UnLink(const_cast<TDisplacementVector&>(*m_pDisplaVector));
    m_pDisplaVector = pDisplaVector;
    Modified();
  }

  if(dynamic_cast<const C3DHorizon*>(&node))
    Modified();

  CStorageNode::OnNewNeighbour(node);
}

void CMeasuredTopDisplacementsNode::OnNeighbourDeleted(const CGraphNode &node)
{
  if(&node == m_pDisplaVector)
  {
    m_pDisplaVector = 0;
    Modified();
  }

  if(dynamic_cast<const C3DHorizon*>(&node))
    Modified();

  CStorageNode::OnNeighbourDeleted(node);
}

void CMeasuredTopDisplacementsNode::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
  if(&node == m_pDisplaVector)
    Modified();
}

unsigned int CMeasuredTopDisplacementsNode::IconId() const
{
  return IDI_MEASURED_DISPLACEMENT;
}

unsigned int CMeasuredTopDisplacementsNode::TypeId() const
{
  return 0;
}

bool CMeasuredTopDisplacementsNode::Empty() const
{
  return false;
}

long CMeasuredTopDisplacementsNode::SavedItems() const
{
  return 1;
}

void CMeasuredTopDisplacementsNode::LoadStream(TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& progress)
{
  int iVectorIndex;
  stream >> iVectorIndex;
  if(iVectorIndex >= 0)
  {
    CModelBase& model = static_cast<CModelBase&>(Model());
  	TValueCompositeEntry& composite_entry = static_cast<TValueCompositeEntry&>(*model.RootModel().GraphEntry(MD_BASE_VALUE_COMPOSITE));
    composite_entry.LinkNodeToIndex(*this, iVectorIndex);
  }

  progress.Step();
}

void CMeasuredTopDisplacementsNode::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  int iVectorIndex = -1;
  if(m_pDisplaVector)
  {
    assert(m_pDisplaVector->IsLinkedTo(*Model().GraphEntry(MD_BASE_VALUE_COMPOSITE)));
    iVectorIndex = m_pDisplaVector->Index();
  }

  stream << iVectorIndex;
  progress.Step();
}

bool CMeasuredTopDisplacementsNode::CanConnectItem(const CGraphNode& item) const
{
  return (dynamic_cast<const TDisplacementVector*>(&item) != 0) || CStorageNode::CanConnectItem(item);
}

bool CMeasuredTopDisplacementsNode::ConnectItem(const CGraphNode& item)
{
  if(dynamic_cast<const TDisplacementVector*>(&item))
  {
    LinkTo(const_cast<CGraphNode&>(item));
    return true;
  }

  return CStorageNode::ConnectItem(item);
}

const TDisplacementVector* CMeasuredTopDisplacementsNode::DisplacementVector() const
{
  return m_pDisplaVector;
}
