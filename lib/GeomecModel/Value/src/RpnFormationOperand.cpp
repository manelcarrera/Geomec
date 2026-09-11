// RpnFormationOperand.cpp: implementation of the CRpnFormationOperand class.
//
//////////////////////////////////////////////////////////////////////

#include "FormationBase.h"
#include "RpnFormationOperand.h"
#include "BaseEntryTypes.h"
#include "MeshBase.h"
#include "Pressure.h"
#include "rpn_version.h"  
#include "LineInterface.h"
#include "FieldFactor.h"
#include "FemAppModel.h"
#include "ModelBase.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CFormationObserver
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CFormationObserver::CFormationObserver(CFormationBase& formation, rpn::CRpnOperand::IValueProxy& proxy)
: CGraphNode(""), m_proxy(proxy), m_formation(formation)
{
	assert(!formation.IsCopy());
	LinkTo(formation);
}

const CFormationBase& CFormationObserver::Formation() const
{
	assert(!m_formation.IsCopy());
	return m_formation;
}

CFormationBase& CFormationObserver::Formation()
{
	assert(!m_formation.IsCopy());
	return m_formation;
}

void CFormationObserver::OnNeighbourDeleted(const CGraphNode& item)
{
	if(&item == &m_formation)
	{
//		assert(m_proxy.m_pObserver == this);
		delete &m_proxy;
		// This will also destroy our selves
		return ;
	}

	CGraphNode::OnNeighbourDeleted(item);
}

unsigned int CFormationObserver::IconId() const
{
	 return 0;
}

unsigned int CFormationObserver::TypeId() const
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of IRpnFormationProxyBase
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

IRpnFormationProxyBase::IRpnFormationProxyBase(CFormationBase& formation, rpn::CRpnStack& NewStack)
: IValueProxy(NewStack),
  m_nIndex(-1)
{
	m_pObserver = new CFormationObserver(formation, *this);
}

IRpnFormationProxyBase::IRpnFormationProxyBase(rpn::CRpnStack& NewStack, const QString& sProxyId)
: IValueProxy(NewStack, sProxyId),
  m_pObserver(0),
  m_nIndex(-1)
{
}

IRpnFormationProxyBase::IRpnFormationProxyBase(const IRpnFormationProxyBase& proxy, rpn::CRpnStack& NewStack)
: IValueProxy(NewStack, proxy.ProxyId().toStdString().c_str()),
  m_nIndex(-1)
{
  if (proxy.m_pObserver != 0)
  {
    m_pObserver = new CFormationObserver(proxy.m_pObserver->Formation(), *this);
  }
  else
  {
    m_pObserver = 0;
    m_nIndex = proxy.m_nIndex;
  }
}

IRpnFormationProxyBase::~IRpnFormationProxyBase()
{
	assert(m_pObserver);
	delete m_pObserver;
}

const CFormationBase& IRpnFormationProxyBase::Formation() const
{
	assert(m_pObserver);
	return m_pObserver->Formation();
}

CFormationBase& IRpnFormationProxyBase::Formation()
{
	assert(m_pObserver);
	return m_pObserver->Formation();
}

int IRpnFormationProxyBase::Index() const
{
  return m_nIndex;
}

CFemAppModel *IRpnFormationProxyBase::FindModel(CFemAppModel& model)
{
  std::vector<CModelBase *> models;
  models.reserve(10);

  models.push_back(&static_cast<CModelBase&>(model).RootModel());

  const CZoominModelEntry* pEntry1 = dynamic_cast<const CZoominModelEntry *>(models[0]->GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (pEntry1)
  {
    const CZoominModelEntry::TNodeSet& zoominModels = pEntry1->EntryNodes();

    for (CZoominModelEntry::TNodeSet::const_iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
    {
      models.push_back(&(*it)->ChildModel());
    }
  }

  for (size_t i = 0; i < models.size(); ++i)
  {
  	TFormationBaseEntry *pEntry2 = (TFormationBaseEntry*)(models[i]->GraphEntry(MD_BASE_FORMATION));

  	TFormationBaseEntry::TNodeSet stNode = pEntry2->EntryNodes();
	  for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); ++it)
	  {
		  if((*it)->Index() == m_nIndex)
        return models[i];
	  }
  }

  return 0;
}

void IRpnFormationProxyBase::AttachToModel(CFemAppModel& model)
{
	assert(m_pObserver == 0);
	assert(m_nIndex > -1);		// Assure valid

	// Look-up the value composite by index
	CFormationBase *pFormation = 0;
	TFormationBaseEntry *pEntry = (TFormationBaseEntry*)(model.GraphEntry(MD_BASE_FORMATION));
	assert(pEntry);
	TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
	for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		if((*it)->Index() == m_nIndex)
			pFormation = *it;
	}

	if (pFormation)	// a pointset rpn formula may be explicitly linked to a zoom-in model, and not find the formations in the main model (Bug 115938)
	  m_pObserver = new CFormationObserver(*pFormation, *this);
}

void IRpnFormationProxyBase::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& /*version*/)
{
	assert(m_pObserver == 0);
	stream >> m_nIndex;
}

void IRpnFormationProxyBase::LoadStream(std::stringstream &stream, CStreamVersion & /*version*/)
{
	assert(m_pObserver == 0);
	stream >> m_nIndex;
}
			
void IRpnFormationProxyBase::SaveStream(std::stringstream &stream)
{
	m_nIndex = -1;
	if(m_pObserver)
		m_nIndex = m_pObserver->Formation().Index();

	stream << m_nIndex << " ";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CRpnFormationProxy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRpnFormationProxy::CRpnFormationProxy(CFormationBase& formation, rpn::CRpnStack& NewStack)
: IRpnFormationProxyBase(formation, NewStack)
{
}

CRpnFormationProxy::CRpnFormationProxy(const CRpnFormationProxy& proxy, rpn::CRpnStack& NewStack)
: IRpnFormationProxyBase(proxy, NewStack)
{
}

CRpnFormationProxy::CRpnFormationProxy(rpn::CRpnStack& NewStack, const QString& sProxyId)
: IRpnFormationProxyBase(NewStack, sProxyId.toStdString().c_str())
{
}

CRpnFormationProxy::~CRpnFormationProxy()
{
}

bool CRpnFormationProxy::Recursive(TParentSet /*stParent*/) const
{
	 return false;
}

rpn::CRpnOperand::IValueProxy* CRpnFormationProxy::Clone(rpn::CRpnStack& NewStack) const
{
	return new CRpnFormationProxy(*this, NewStack);
}

unsigned int CRpnFormationProxy::IconId() const
{
	return Formation().IconId();
}

int CRpnFormationProxy::TypeId() const
{
	return FORMATION_ID;
}

QString CRpnFormationProxy::TextTag() const
{
	return Formation().Name();
}

CRpnFormationProxy::TValue CRpnFormationProxy::Value(const geo::IPoint &pt, UNIT /*unit*/) const
{
	// Do we have a mesh?
	CModelBase& model = (CModelBase&)Formation().Model();
	if(!model.IsMesh())
		return TValue();	// No valid mesh, no valid value...

	std::vector<int> vcElement = model.Mesh().Mesh().ElementsAt(pt);
	for(int i = 0; i < vcElement.size(); i++)
	{
		const geo::IElement& element = model.Mesh().Mesh().Element(vcElement[i]);

		// don't try interface elements
		if(dynamic_cast<const geo::CLineInterface*>(&element))
			return TValue(0);

		if(model.Mesh().Formation(element) == &Formation())
			return TValue(1);
	}

	return TValue(0);
}

CRpnFormationProxy::TValueVec CRpnFormationProxy::Value(const geo::IElement &el, UNIT unit) const
{
	const CModelBase& model = dynamic_cast<const CModelBase&>(Formation().Model());
	TValueVec ret(el.NrOfPoints());
	if(model.Mesh().IsMesh())
	{
		// Find mesh element
		const geo::IElement* pElement = 0;
		const geo::IFace* pFace = dynamic_cast<const geo::IFace*>(&el);
		if(pFace)
		{
			if(pFace->Parent()) {
				if(&model.Mesh().Mesh() == pFace->Parent()->IndexingElementSet())
				  pElement = pFace->Parent();
			}
		}
		
		if(el.IndexingElementSet() == &model.Mesh().Mesh())
			pElement = &el;


		if(pElement)
		{
			// don't try interface elements
			const geo::CLineInterface* pLineInterface = dynamic_cast<const geo::CLineInterface*>(pElement);

			if(!pLineInterface && (model.Mesh().Formation(*pElement) == &Formation()))
			{
				for(int i = 0; i < ret.size(); i++)
					ret[i] = TValue(1);
			}
			else
			{
				for(int i = 0; i < ret.size(); i++)
					ret[i] = TValue(0);
			}
		}
		else
		{
			for(int i = 0; i < el.NrOfPoints(); i++)
				ret[i] = Value(el.Point(i), unit);
		}
	}
	
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CRpnFormationThicknessProxy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRpnFormationThicknessProxy::CRpnFormationThicknessProxy(CFormationBase& formation, rpn::CRpnStack& NewStack)
: IRpnFormationProxyBase(formation, NewStack)
{
}

CRpnFormationThicknessProxy::CRpnFormationThicknessProxy(rpn::CRpnStack& stack, const QString& sProxyId)
: IRpnFormationProxyBase(stack, sProxyId.toStdString().c_str())
{
}

CRpnFormationThicknessProxy::CRpnFormationThicknessProxy(const CRpnFormationThicknessProxy& proxy, rpn::CRpnStack& NewStack)
: IRpnFormationProxyBase(proxy, NewStack)
{
}

CRpnFormationThicknessProxy::~CRpnFormationThicknessProxy()
{
}

rpn::CRpnOperand::IValueProxy* CRpnFormationThicknessProxy::Clone(rpn::CRpnStack& NewStack) const
{
  return new CRpnFormationThicknessProxy(*this, NewStack);
}

QString CRpnFormationThicknessProxy::TextTag() const
{
  return Formation().Name() + " thickness";
}

CRpnFormationThicknessProxy::TValue CRpnFormationThicknessProxy::Value(const geo::IPoint &pt, UNIT unit) const
{
  // dummy top and bottom points
  geo::CPoint ptTop;
  geo::CPoint ptBottom;
  geo::CValue v = Formation().ThicknessAt(pt, ptTop, ptBottom, geo::IParallelInitializationCallback::Sequential);
  if(v.Valid() && unit == CQuantity::FIELD_UNIT)
    v = v.Value() * FF_FACTOR_LENGTH;

  return v;
}

CRpnFormationThicknessProxy::TValueVec CRpnFormationThicknessProxy::Value(const geo::IElement &el, UNIT unit) const
{
  TValueVec vcValue(el.NrOfNodes());
  int i;
  for(i = 0; i < el.NrOfNodes(); ++i)
    vcValue[i] = Value(el.Node(i), unit);

  return vcValue;
}

unsigned int CRpnFormationThicknessProxy::IconId() const
{
  return IDI_FORMATION_THICKNESS;
}

bool CRpnFormationThicknessProxy::Recursive(TParentSet /*stParent*/) const
{
  return false;
}

bool CRpnFormationThicknessProxy::Defined() const
{
  return (Formation().ElementSetSize() > 0);
}

bool CRpnFormationThicknessProxy::exists() const
{
  return Defined();
}

int CRpnFormationThicknessProxy::TypeId() const
{
  return FORMATION_THICKNESS_ID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CRpnReservoirProxy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRpnReservoirProxy::CRpnReservoirProxy(CMeshBase& mesh, rpn::CRpnStack& NewStack)
: rpn::CRpnOperand::IValueProxy(NewStack), m_pMesh(&mesh)
{
}

CRpnReservoirProxy::CRpnReservoirProxy(CMeshBase& mesh, rpn::CRpnStack& NewStack, const QString& sProxyId)
: rpn::CRpnOperand::IValueProxy(NewStack, sProxyId.toStdString().c_str()), m_pMesh(&mesh)
{
}

CRpnReservoirProxy::CRpnReservoirProxy(rpn::CRpnStack& stack, const QString& sProxyId)
: rpn::CRpnOperand::IValueProxy(stack, sProxyId.toStdString().c_str()), m_pMesh(0)
{
}

CRpnReservoirProxy::~CRpnReservoirProxy()
{
}

bool CRpnReservoirProxy::IsElementDepleting(const geo::IElement& element) const
{
	assert(&m_pMesh->Mesh() == element.IndexingElementSet());

	// don't try interface elements
	if(dynamic_cast<const geo::CLineInterface*>(&element))
		return false;

	const CFormationBase *pFormation = m_pMesh->Formation(element);
	assert(pFormation);
	const CDepletionStage* pStage = &((CModelBase&)m_pMesh->Model()).InitialDepletionStage();

	IValueDomainScalar::TValueVec vcInitialValues = pFormation->Pressure(*pStage).Component().ScalarData().ValueElement(element);

	while(pStage)
	{
		if(!pStage->Initial())
		{
			IValueDomainScalar::TValueVec vcCurrent = pFormation->Pressure(*pStage).Component().ScalarData().ValueElement(element);

			for(int nPoint = 0; nPoint < element.NrOfPoints(); nPoint++)
			{
				TValue initial = vcInitialValues[nPoint];
				if(!initial.Valid())
					return false;

				TValue current = vcCurrent[nPoint];
				if(!current.Valid())
					return false;

				if(fabs(current.Value() - initial.Value()) > (fabs(initial.Value()) * 0.001))
					return true;
			}
		}

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}

	return false;
}

rpn::CRpnOperand::IValueProxy* CRpnReservoirProxy::Clone(rpn::CRpnStack& NewStack) const
{
	return new CRpnReservoirProxy(*m_pMesh, NewStack, ProxyId().toStdString().c_str());
}

bool CRpnReservoirProxy::Recursive(TParentSet /*stParent*/) const
{
	 return false;
}

QString CRpnReservoirProxy::TextTag() const
{
	return "IsReservoir";
}

void CRpnReservoirProxy::Mesh(CMeshBase& mesh)
{
	m_pMesh = &mesh;
}

CRpnReservoirProxy::TValue CRpnReservoirProxy::Value(const geo::IPoint &pt, UNIT /*unit*/) const
{
	if(!m_pMesh->IsMesh())
		return TValue();

	std::vector<int> vcElement = m_pMesh->Mesh().ElementsAt(pt);
	if(vcElement.size() > 0)
	{
		for(int i = 0; i < vcElement.size(); i++)
		{
			if(IsElementDepleting(m_pMesh->Mesh().Element(vcElement[i])))
				return TValue(1);
		}

		return TValue(0);
	}

	return TValue();	// No mesh
}

CRpnReservoirProxy::TValueVec CRpnReservoirProxy::Value(const geo::IElement &el, UNIT unit) const
{
	bool b = false;

	int i;
	for(i = 0; i < el.NrOfPoints(); i++)
	{
		if(Value(el.Point(i), unit).Value() != 0)
		{
			b = true; // entire element is depleting if at least one node is depleting
			break;
		}
	}

	TValueVec ret(el.NrOfPoints());
	for(i = 0; i < ret.size(); ++i)
		ret[i] = (b ? 1 : 0);

	return ret;
}

unsigned int CRpnReservoirProxy::IconId() const
{
	return IDI_ISRESERVOIR;
}

int CRpnReservoirProxy::TypeId() const
{
	return RESERVOIR_ID;
}

void CRpnReservoirProxy::AttachToModel(CFemAppModel& model)
{
	CModelBase& base_model = dynamic_cast<CModelBase&>(model);
	m_pMesh = &base_model.Mesh();
}
