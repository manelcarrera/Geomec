#ifndef _MEASUREDTOPDISPLACEMENTSNODE_H_
#define _MEASUREDTOPDISPLACEMENTSNODE_H_

#include "ValueVector.h"
#include "RpnOperand.h"
#include "StorageNode.h"

class TDisplacementVector;

#include "3dmodel.h"
#include "GeomecModelVisitor.h"

class CMeasuredTopDisplacementsNode : public CStorageNode
{
public:
  CMeasuredTopDisplacementsNode(const QString& sName, C3DModel& model);

	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint = Default);
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
  virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  const TDisplacementVector* DisplacementVector() const;

  ACCEPT_GEOMECMODELVISITORS(VisitMeasuredTopDisplacementsNode);

private:
  const TDisplacementVector* m_pDisplaVector;
};

template <int COMPONENT_INDEX, unsigned int TAG>
class CRpnTopDisplacementProxy : public rpn::CRpnOperand::IValueProxy
{
public:
  typedef rpn::CRpnObject::TValue TValue;
  typedef rpn::CRpnObject::TValueVec TValueVec;
  typedef rpn::CRpnObject::UNIT UNIT;

public:
  CRpnTopDisplacementProxy(C3DModel& model, rpn::CRpnStack& stack);
  CRpnTopDisplacementProxy(C3DModel& model, rpn::CRpnStack& stack, const QString& sProxyId);
  CRpnTopDisplacementProxy(rpn::CRpnStack& stack, const QString& sProxyId);
	virtual rpn::CRpnOperand::IValueProxy* Clone(rpn::CRpnStack& NewStack) const;
	virtual QString TextTag() const;
	virtual unsigned int IconId() const;
	virtual bool Recursive(TParentSet stParent) const;
  virtual int TypeId() const;
  virtual void SaveStream(std::stringstream& stream);
  virtual void LoadStream(std::stringstream& stream, CStreamVersion& version);
  virtual bool Defined() const;
  virtual bool exists() const;
	virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
	virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
	virtual void AttachToModel(CFemAppModel& model);

  int ComponentNr() const;

private:
  C3DModel* m_pModel;
};


template <int COMPONENT_INDEX, unsigned int TAG>
CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::CRpnTopDisplacementProxy(C3DModel& model, rpn::CRpnStack& stack)
: rpn::CRpnOperand::IValueProxy(stack),
  m_pModel(&model)
{
}

template <int COMPONENT_INDEX, unsigned int TAG>
CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::CRpnTopDisplacementProxy(C3DModel& model, rpn::CRpnStack& stack, const QString& sProxyId)
: rpn::CRpnOperand::IValueProxy(stack, sProxyId.toStdString().c_str()),
  m_pModel(&model)
{
}

template <int COMPONENT_INDEX, unsigned int TAG>
CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::CRpnTopDisplacementProxy(rpn::CRpnStack& stack, const QString& sProxyId)
: rpn::CRpnOperand::IValueProxy(stack, sProxyId.toStdString().c_str()),
  m_pModel(0)
{
}

template <int COMPONENT_INDEX, unsigned int TAG>
rpn::CRpnOperand::IValueProxy* CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::Clone(rpn::CRpnStack& NewStack) const
{
  assert(m_pModel);
  return new CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>(*m_pModel, NewStack, ProxyId().toStdString().c_str());
}

template <int COMPONENT_INDEX, unsigned int TAG>
QString CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::TextTag() const
{
  return getStringTableEntry(TAG);
}

template <int COMPONENT_INDEX, unsigned int TAG>
unsigned int CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::IconId() const
{
  return m_pModel->MeasuredTopDisplacementsNode().IconId();
}

template <int COMPONENT_INDEX, unsigned int TAG>
bool CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::Recursive(TParentSet /*stParent*/) const
{
  return false;
}

template <int COMPONENT_INDEX, unsigned int TAG>
int CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::TypeId() const
{
  return m_pModel->MeasuredTopDisplacementsNode().TypeId();
}

template <int COMPONENT_INDEX, unsigned int TAG>
void CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::SaveStream(std::stringstream& /*stream*/)
{
}

template <int COMPONENT_INDEX, unsigned int TAG>
void CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::LoadStream(std::stringstream& /*stream*/, CStreamVersion& /*version*/)
{
}

template <int COMPONENT_INDEX, unsigned int TAG>
bool CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::Defined() const
{
  return (m_pModel->MeasuredTopDisplacementsNode().DisplacementVector() != 0);
}

template <int COMPONENT_INDEX, unsigned int TAG>
bool CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::exists() const
{
  return (m_pModel->MeasuredTopDisplacementsNode().DisplacementVector() != 0);
}

template <int COMPONENT_INDEX, unsigned int TAG>
IValueDomainScalar::TValue CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::Value(const geo::IPoint &pt, UNIT unit) const
{
  const TDisplacementVector* pVector = m_pModel->MeasuredTopDisplacementsNode().DisplacementVector();
  if(!pVector)
    return TValue();

  const IValueComponentBase& comp = pVector->Component(COMPONENT_INDEX);
  return comp.ScalarData().ValuePoint(pt, unit);
}

template <int COMPONENT_INDEX, unsigned int TAG>
IValueDomainScalar::TValueVec CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::Value(const geo::IElement &el, UNIT unit) const
{
  TValueVec vc(el.NrOfNodes());
  const TDisplacementVector* pVector = m_pModel->MeasuredTopDisplacementsNode().DisplacementVector();
  if(!pVector)
    return vc;

  const IValueComponentBase& comp = pVector->Component(COMPONENT_INDEX);
  return comp.ScalarData().ValueElement(el, unit);
}

template <int COMPONENT_INDEX, unsigned int TAG>
void CRpnTopDisplacementProxy<COMPONENT_INDEX, TAG>::AttachToModel(CFemAppModel& model)
{
  if(!m_pModel)
  {
    assert(dynamic_cast<C3DModel*>(&model));
    m_pModel = static_cast<C3DModel*>(&model);
  }

  assert(&model == m_pModel);
}

#endif // _MEASUREDTOPDISPLACEMENTSNODE_H_
