// RpnValueSet.cpp: implementation of the CRpnValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "RpnValueSet.h"
#include "IPointSet.h"
#include "RpnFormationOperand.h"
#include "FormationBase.h"
#include "BaseEntryTypes.h"
#include "PointSet.h"
#include "FieldFactor.h"
#include "rpn_version.h"
#include "NodalValueSet.h"
#include "ValueComponent.h"
#include "ModelBase.h"
#include "resourceIDI.h"
#include "ProxyPersistant.h"
#include "IParallelInitializationCallback.h"

CRpnValueSet::CValueSetProxy::CValueSetObserver::CValueSetObserver(IValueSet& value_set)
: CGraphNode(""), m_value_set(value_set)
{
  assert(!value_set.IsCopy() && value_set.IsCopied());
}

CRpnValueSet::CValueSetProxy::CValueSetObserver::CValueSetObserver(IValueSet& value_set, CValueSetProxy& proxy)
: CGraphNode(""), m_value_set(value_set)
{
  assert(!value_set.IsCopy());
  Attach(proxy);
  LinkTo(value_set);
}

CRpnValueSet::CValueSetProxy::CValueSetObserver::CValueSetObserver(const CValueSetObserver& rhs, CValueSetProxy& proxy)
: CGraphNode(rhs), m_value_set(rhs.m_value_set)
{
  if(!IsLinkedTo(ValueSet()))
    LinkTo(ValueSet());

  assert(!m_value_set.IsCopy());
  Attach(proxy);
}

CRpnValueSet::CValueSetProxy::CValueSetObserver::~CValueSetObserver()
{
  assert(m_stProxy.size() == 0);
}

CRpnValueSet::CValueSetProxy::CValueSetObserver& CRpnValueSet::CValueSetProxy::CValueSetObserver::operator=(const CValueSetObserver& rhs)
{
  CGraphNode::operator =(rhs);
  return *this;
}

const IValueSet& CRpnValueSet::CValueSetProxy::CValueSetObserver::ValueSet() const
{
  if(m_value_set.IsCopied())
    return (const IValueSet&) m_value_set.Copy();
  return m_value_set;
}

IValueSet& CRpnValueSet::CValueSetProxy::CValueSetObserver::ValueSet()
{
  if(m_value_set.IsCopied())
    return (IValueSet&) m_value_set.Copy();
  return m_value_set;
}

void CRpnValueSet::CValueSetProxy::CValueSetObserver::OnNeighbourDeleted(const CGraphNode& item)
{
  if(&item == &m_value_set)
  {
    TValueProxySet stProxy = m_stProxy;
    if(m_stProxy.size() > 0)
    {
      for(TValueProxySet::iterator it = stProxy.begin(); it != stProxy.end(); it++)
        delete *it;
    }
    else
       delete this;
    // This will also destroy our selves
    return ;
  }

  CGraphNode::OnNeighbourDeleted(item);
}

unsigned int CRpnValueSet::CValueSetProxy::CValueSetObserver::IconId() const
{
   return 0;
}

unsigned int CRpnValueSet::CValueSetProxy::CValueSetObserver::TypeId() const
{
  return 0;
}

void CRpnValueSet::CValueSetProxy::CValueSetObserver::Attach(CValueSetProxy& proxy)
{
  VERIFY(m_stProxy.insert(&proxy).second);
}

void CRpnValueSet::CValueSetProxy::CValueSetObserver::Detach(CValueSetProxy& proxy)
{
  VERIFY(m_stProxy.erase(&proxy) == 1);
  if(m_stProxy.size() == 0)
  {
    // Prevent to delete a demon
    if(IsCopy())
    {
      CValueSetObserver& source = dynamic_cast<CValueSetObserver&>(Source());
      delete this;
      if(source.Empty())
        delete &source;
    }
    else
    {
      delete this;
    }
  }
}

bool CRpnValueSet::CValueSetProxy::CValueSetObserver::Empty() const
{
  if(m_stProxy.size() == 0)
  {
    return true;
  }
  return false;
}

void CRpnValueSet::CValueSetProxy::CValueSetObserver::SwapProxy()
{
  assert(IsCopied());
  CValueSetObserver& copy = dynamic_cast<CValueSetObserver&>(Copy());
  // Switch the observer in the proxies
  TValueProxySet::iterator it;
  for(it = m_stProxy.begin(); it != m_stProxy.end(); it++)
    (*it)->SetObserver(&copy);
  for(it = copy.m_stProxy.begin(); it != copy.m_stProxy.end(); it++)
    (*it)->SetObserver(this);
  // Swich the proxies in the observers
  TValueProxySet stTemp = m_stProxy;
  m_stProxy = copy.m_stProxy;
  copy.m_stProxy = stTemp;
}

//////////////////////////////////////////////////////////////////////
// Implementation of CRpnValueSet::CValueSetCoordinateProxy
//////////////////////////////////////////////////////////////////////
CRpnValueSet::CValueSetCoordinateProxy::CValueSetCoordinateProxy(CRpnValueSet& value_set, unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: CCoordinateProxy(uName, stack, coor, sProxyId), m_pValueSet(&value_set)
{
}

CRpnValueSet::CValueSetCoordinateProxy::CValueSetCoordinateProxy(CRpnValueSet& value_set, const QString& sName, rpn::CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: CCoordinateProxy(sName, stack, coor, sProxyId), m_pValueSet(&value_set)
{
}

CRpnValueSet::CValueSetCoordinateProxy::CValueSetCoordinateProxy(rpn::CRpnStack& stack, const QString& sProxyId)
:  CCoordinateProxy(stack, sProxyId), m_pValueSet(0)
{
}

CRpnValueSet::CValueSetCoordinateProxy::CValueSetCoordinateProxy(CRpnValueSet& value_set, unsigned int uName, rpn::CRpnStack& stack, COORDINATE coor)
: CCoordinateProxy(uName, stack, coor), m_pValueSet(&value_set)
{
}

rpn::CRpnOperand::IValueProxy* CRpnValueSet::CValueSetCoordinateProxy::Clone(rpn::CRpnStack& NewStack) const
{
  return new CValueSetCoordinateProxy(*m_pValueSet, m_sName, NewStack, m_coor, ProxyId());
}

unsigned int CRpnValueSet::CValueSetCoordinateProxy::IconId() const
{
  return IDI_VALUETYPE_COORDINATE;
}

int CRpnValueSet::CValueSetCoordinateProxy::TypeId() const
{
  return COORDINATE_ID;
}

bool CRpnValueSet::CValueSetCoordinateProxy::Recursive(TParentSet /*stParent*/) const
{
  return false;
}

CRpnValueSet::TValue CRpnValueSet::CValueSetCoordinateProxy::Value(const geo::IPoint &pt, UNIT unit) const
{
  if(unit == CQuantity::SI_UNIT)
  {
    switch(m_coor)
    {
    case CO_X:
      return TValue(pt.X());
    case CO_Y:
      return TValue(pt.Y());
    case CO_Z:
      return TValue(pt.Z());
    default:
      assert(false);
    }
  }
  else
  {
    switch(m_coor)
    {
    case CO_X:
      return TValue(pt.X() * FF_FACTOR_LENGTH);
    case CO_Y:
      return TValue(pt.Y() * FF_FACTOR_LENGTH);
    case CO_Z:
      return TValue(pt.Z() * FF_FACTOR_LENGTH);
    default:
      assert(false);
    }

  }

  return TValue();
}

void CRpnValueSet::CValueSetCoordinateProxy::ValueSet(CRpnValueSet& value_set)
{
  m_pValueSet = &value_set;
}


//////////////////////////////////////////////////////////////////////
// Implementation of the CRpnValueSet::CValueSetProxy
//////////////////////////////////////////////////////////////////////

// This constructor delivers an enabled proxy...
CRpnValueSet::CValueSetProxy::CValueSetProxy(int nIndex, const QString& sProxyId, rpn::CRpnStack& NewStack)
: IValueProxy(NewStack, sProxyId), m_nIndex(nIndex)
{
}

CRpnValueSet::CValueSetProxy::CValueSetProxy(IValueSet& value_set, rpn::CRpnStack& NewStack, bool bObserverIsCopy)
: IValueProxy(NewStack), m_nIndex(-1)
{
  if(bObserverIsCopy)
  {
    CValueSetObserver* pSource = new CValueSetObserver(value_set);
    m_pObserver = new CValueSetObserver(*pSource, *this);
  }
  else
    m_pObserver = new CValueSetObserver(value_set, *this);
}

// This constructor delivers a disabled proxy. You must call enable before destruction..
CRpnValueSet::CValueSetProxy::CValueSetProxy(rpn::CRpnStack& NewStack, const QString& sProxyId)
: IValueProxy(NewStack, sProxyId), m_pObserver(0), m_nIndex(-1)
{
}

CRpnValueSet::CValueSetProxy::CValueSetProxy(const CValueSetProxy& rhs, rpn::CRpnStack& NewStack)
: IValueProxy(NewStack, rhs.ProxyId()), m_pObserver(rhs.m_pObserver), m_nIndex(-1)
{
  if (m_pObserver != 0)
  {
  m_pObserver->Attach(*this);
  }
  else
  {
  m_nIndex = rhs.m_nIndex;
  }
}

void CRpnValueSet::CValueSetProxy::CreateCopy(const CValueSetProxy& source)
{
  assert(m_pObserver && source.m_pObserver);
  assert(!m_pObserver->IsCopied() && !source.m_pObserver->IsCopied());
  assert(&m_pObserver->ValueSet() && &source.m_pObserver->ValueSet());
  m_pObserver->Detach(*this);
  m_pObserver = new CValueSetObserver(*source.m_pObserver, *this);
  source.m_pObserver->ValueSet().AssertValid();
  assert(m_pObserver->IsCopy());
}

void  CRpnValueSet::CValueSetProxy::Validate()
{
  if(m_pObserver->IsCopy())
  {
    // When the source is empty a new proxy is created and is not swapped by the 
    // source, because the source is empty. We swap it now
    CValueSetObserver& source = dynamic_cast<CValueSetObserver&>(m_pObserver->Source());
    if(source.Empty())
    {
      source = (*m_pObserver);
      source.SwapProxy();
    }
  }
  else
  {
    // Does the source have an copy? If not delete the proxy
    if(m_pObserver->IsCopied())
      m_pObserver->SwapProxy();			
    else
      delete this;
  }
}

CRpnValueSet::CValueSetProxy::CValueSetObserver& CRpnValueSet::CValueSetProxy::Observer()
{
  return *m_pObserver;
}

const CRpnValueSet::CValueSetProxy::CValueSetObserver& CRpnValueSet::CValueSetProxy::Observer() const
{
  return *m_pObserver;
}

void  CRpnValueSet::CValueSetProxy::SetObserver(CValueSetObserver *pObserver)
{
  m_pObserver = pObserver;
}

CRpnValueSet::CValueSetProxy::~CValueSetProxy()
{
  assert(m_nIndex == -1);
  if(m_pObserver)
    m_pObserver->Detach(*this);
}

rpn::CRpnOperand::IValueProxy* CRpnValueSet::CValueSetProxy::Clone(rpn::CRpnStack& NewStack) const
{
  // The proxy must be enabled
  // We can only insert sources by the 
  return new CValueSetProxy(*this, NewStack);
}

int CRpnValueSet::CValueSetProxy::TypeId() const
{
  return VALUE_ID;
}

const IValueSet& CRpnValueSet::CValueSetProxy::ValueSet() const
{
  // The proxy must be enabled
  assert(m_pObserver);
  assert(m_nIndex == -1);

   return m_pObserver->ValueSet();
}

unsigned int CRpnValueSet::CValueSetProxy::IconId() const
{
  assert(m_pObserver);

   return m_pObserver->ValueSet().IconId();
}

bool CRpnValueSet::CValueSetProxy::Recursive(TParentSet stParent) const
{
  CRpnValueSet *pValueSet = dynamic_cast<CRpnValueSet*>(&m_pObserver->ValueSet());
  if(pValueSet)
    return pValueSet->RpnStack().Recursive(stParent);

  return false;
}

IValueSet& CRpnValueSet::CValueSetProxy::ValueSet()
{
  // The proxy must be enabled
  assert(m_pObserver);
  assert(m_nIndex == -1);

  return m_pObserver->ValueSet();
}

QString CRpnValueSet::CValueSetProxy::TextTag() const
{
  assert(m_pObserver);
  return m_pObserver->ValueSet().Name();
}

void CRpnValueSet::CValueSetProxy::LoadStream(std::stringstream& stream, CStreamVersion& /*version*/)
{
  assert(m_pObserver == 0);
  stream >> m_nIndex;
}

void CRpnValueSet::CValueSetProxy::SaveStream(std::stringstream& stream)
{
  assert(m_pObserver);
  
  // Find out the value set index
  int nIndex = -1;
  for(int nValueSet = 0; nValueSet < ValueSet().PointSet().ValueSetSize(); nValueSet++)
  {
    if(&ValueSet() == &ValueSet().PointSet().ValueSet(nValueSet))
      nIndex = nValueSet;
  }
    
  assert(nIndex > -1);

  // Save index and name of the value set
  stream << nIndex << " ";
}
  
CRpnValueSet::TValue CRpnValueSet::CValueSetProxy::Value(const geo::IPoint &pt, UNIT unit) const
{
  // The proxy must be enabled
  assert(m_pObserver);
  assert(m_nIndex == -1);

  if ( dynamic_cast<const CRpnValueSet*>(&m_pObserver->ValueSet()) )
  return m_pObserver->ValueSet().ValuePoint(pt, geo::IParallelInitializationCallback::Sequential);

  // mantis 2729 wjrx Do unit conversion
  //
  CQuantity::UNIT valueUnit= m_pObserver->ValueSet().PointSet().ValueUnit();
  const CValueComponent * component= m_pObserver->ValueSet().Component();
  double fieldFactor= component ? component->FieldFactor() : 1.0;

  if ( unit != valueUnit )
  {
    if ( valueUnit==CQuantity::SI_UNIT ) 
      return m_pObserver->ValueSet().ValuePoint(pt, geo::IParallelInitializationCallback::Sequential) * fieldFactor;
    else
      return m_pObserver->ValueSet().ValuePoint(pt, geo::IParallelInitializationCallback::Sequential) / fieldFactor;
  }
  else return m_pObserver->ValueSet().ValuePoint(pt, geo::IParallelInitializationCallback::Sequential);
}

CRpnValueSet::TValueVec CRpnValueSet::CValueSetProxy::Value(const geo::IElement &el, UNIT unit) const
{
  // The proxy must be enabled
  assert(m_pObserver);
  assert(m_nIndex == -1);
  
  if ( dynamic_cast<const CRpnValueSet*>(&m_pObserver->ValueSet()) )
  return m_pObserver->ValueSet().ValueElement(el, geo::IParallelInitializationCallback::Sequential);

  // mantis 2729 wjrx Do unit conversion
  //
  CQuantity::UNIT valueUnit= m_pObserver->ValueSet().PointSet().ValueUnit();
  const CValueComponent * component= m_pObserver->ValueSet().Component();
  double fieldFactor= component ? component->FieldFactor() : 1.0;
  double fac= 1;

  if ( unit != valueUnit )
  {
    if ( valueUnit==CQuantity::SI_UNIT ) 
      fac= fieldFactor;
    else
      fac= 1 / fieldFactor;
  }

  CRpnValueSet::TValueVec::iterator it;
  CRpnValueSet::TValueVec vec;

  IValueSet::TValueVec vcValues = m_pObserver->ValueSet().ValueElement(el, geo::IParallelInitializationCallback::Sequential);
  
  for ( it = vcValues.begin(); it != vcValues.end(); ++it)
    vec.push_back(*it * fac);

  return vec;
}

void CRpnValueSet::CValueSetProxy::AttachPointSet(IPointSet& point_set)
{
  m_pObserver = new CValueSetObserver(point_set.ValueSet(m_nIndex), *this);	
  m_nIndex = -1;
}

// wjrx mantis 2521
bool CRpnValueSet::CValueSetProxy::showInDialog() const
{
  // is it a nodal value set?
  const CNodalValueSet* pnvset = dynamic_cast<const CNodalValueSet*>(&Observer().ValueSet());
  if(pnvset)
  {
  // get the component, see if it's a coordinate component
  const CValueComponent* pValueComp = pnvset->Component();
  if(pValueComp)
  {
      // we need to check the source, the copy does not contain enough type information
      if(pValueComp->IsCopy())
    pValueComp = (const CValueComponent*)&pValueComp->Source();
      assert(pValueComp != 0); // if we have a copy, we should have a source

      // only allow sets that are NOT coordinates (mantis 2521)
      const CPointSet::CCoordinateType::CCoordinateComponent* pComp =
    dynamic_cast<const CPointSet::CCoordinateType::CCoordinateComponent*>(pValueComp);
      return (pComp == 0);
  }
  }

  return true;
}

//////////////////////////////////////////////////////////////////////
// Implementation of the CRpnValueSet
//////////////////////////////////////////////////////////////////////

// Default constructor for invalid shit
CRpnValueSet::CRpnValueSet(IPointSet& point_set)
  : IValueSet("", CQuantity::SI_UNIT, point_set), m_pStack(0), m_model(-1)
{
  // Do not link to point set. We're invalid.
  // Link to the source point always
  if(!point_set.IsCopy())
    point_set.m_vcRpnValueSet.push_back(this);
}

CRpnValueSet::CRpnValueSet(const CRpnValueSet& rhs)
: IValueSet(rhs),
  m_pStack(0),
  m_stUsers(rhs.m_stUsers),
  m_model(-1)
{
  // We can have an empty stack. If not copy stack
  if(rhs.m_pStack)
  {
    m_pStack = new rpn::CRpnStack(*rhs.m_pStack);

    rpn::CProxyPersistant<CValueSetProxy> psProxy;
    rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy = psProxy.Proxies(*m_pStack);
    for(size_t i = 0; i < vcProxy.size(); i++)
    {
      CValueSetProxy& source = dynamic_cast<CValueSetProxy&>(*rhs.m_pStack->ValueProxy(vcProxy[i]->ProxyId()));
      vcProxy[i]->CreateCopy(source);
    }
  }

  // Check or we're registered in the m_vcRpnValueSet 
  bool bFound = false;
  for(size_t i = 0; i < PointSet().m_vcRpnValueSet.size(); i++)
  {
    if(PointSet().m_vcRpnValueSet[i] == &rhs)
      bFound = true;
  }

  if(!bFound)
    PointSet().m_vcRpnValueSet.push_back((CRpnValueSet*) &rhs);
}

CRpnValueSet::CRpnValueSet(IPointSet& point_set, const CRpnValueSet& value_set)
: IValueSet(value_set.Name().toStdString().c_str(), CQuantity::SI_UNIT, point_set),
  m_pStack(0),
  m_stUsers(value_set.m_stUsers),
  m_model(-1)
{
  // Copy the stack and swap model items by
  // I	CValueSetCoordinateProxy
  // II	CValueSetProxy
  // III	CFormationProxy
  // IV	CIsReservoir
  CModelBase& model = dynamic_cast<CModelBase&>(point_set.Model());
  m_pStack = new rpn::CRpnStack(*value_set.m_pStack);

  // Insert value set of copy in coordinate proxy
  rpn::CProxyPersistant<CValueSetCoordinateProxy> psCoorProxy;
  rpn::CProxyPersistant<CValueSetCoordinateProxy>::TProxyVec vcCoorProxy = psCoorProxy.Proxies(*m_pStack);
  for(size_t i = 0; i < vcCoorProxy.size(); i++)
    vcCoorProxy[i]->ValueSet(*this);

  // Check mesh in proxy
  rpn::CProxyPersistant<CRpnReservoirProxy> psResProxy;
  rpn::CProxyPersistant<CRpnReservoirProxy>::TProxyVec vcResProxy = psResProxy.Proxies(*m_pStack);
  assert(vcResProxy.size() == 1);
  vcResProxy[0]->Mesh(model.Mesh());

  if(!point_set.IsCopy())
    point_set.m_vcRpnValueSet.push_back(this);
}

CRpnValueSet::~CRpnValueSet()
{
  delete m_pStack;

  // When we are deleting a copy we must check that proxy observer copies attached to
  // the source are deleted.
  if(IsCopy())
  {
    CRpnValueSet& source = dynamic_cast<CRpnValueSet&>(Source());
    if(source.m_pStack)
    {
      rpn::CProxyPersistant<CValueSetProxy> psProxy;
      rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy = psProxy.Proxies(*source.m_pStack);
      for(size_t i = 0; i < vcProxy.size(); i++)
      {
        if(vcProxy[i]->Observer().IsCopied())
        {
          assert(dynamic_cast<CValueSetProxy::CValueSetObserver&>(vcProxy[i]->Observer().Copy()).Empty());
          delete &vcProxy[i]->Observer().Copy();
        }
      }
    }
  }
}

CRpnValueSet* CRpnValueSet::clone()
{
  /*
     Do not use the copy-constructor.
     The base-class 'CGraphNode' introduces an algorithm that limits the
     number of copies.
  */

  return new CRpnValueSet(PointSet(), *this);
}

bool CRpnValueSet::operator==(const CRpnValueSet& rhs) const
{
  if(! IValueSet::operator ==(rhs) )
    return false;

  return rhs.m_pStack->Formula() == m_pStack->Formula();
}

CRpnValueSet& CRpnValueSet::operator=(const CRpnValueSet& rhs)
{
  // Delete old stack and create a new one ...
  IValueSet::operator =(rhs);

  // Validate ValueSetProxies. We do the copies first...
  assert(rhs.m_pStack);

  m_model = rhs.m_model;
  
  // Collect all source observers
  std::set<CValueSetProxy::CValueSetObserver*> stObserver;
  rpn::CProxyPersistant<CValueSetProxy> psProxy1;
  rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy1 = psProxy1.Proxies(*rhs.m_pStack);
  for(size_t i = 0; i < vcProxy1.size(); i++)
  {
    assert(vcProxy1[i]->Observer().IsCopy());
    CValueSetProxy::CValueSetObserver& source = dynamic_cast<CValueSetProxy::CValueSetObserver&>(vcProxy1[i]->Observer().Source());
    stObserver.insert(&source);
  }

  if(m_pStack)
  {
    rpn::CProxyPersistant<CValueSetProxy> psProxy2;
    rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy2 = psProxy2.Proxies(*m_pStack);
    for(size_t i = 0; i < vcProxy2.size(); i++)
    {
      if(vcProxy2[i]->Observer().IsCopied())
        stObserver.insert(&vcProxy2[i]->Observer());	
      else
        delete &vcProxy2[i]->Observer();				// Remove unused observer
    }
  }

  // Swap all observers
  for(std::set<CValueSetProxy::CValueSetObserver*>::iterator it = stObserver.begin(); it != stObserver.end(); it++)
  {
    assert((*it)->IsCopied());
    CValueSetProxy::CValueSetObserver& copy = dynamic_cast<CValueSetProxy::CValueSetObserver&>((*it)->Copy());
    (**it) = copy;
    (*it)->SwapProxy();
  }

  // Swap stack
  rpn::CRpnStack *pTemp = m_pStack;
  m_pStack = rhs.m_pStack;
  const_cast<CRpnValueSet&>(rhs).m_pStack = pTemp;

  m_stUsers = rhs.m_stUsers;

  return *this;
}

unsigned int CRpnValueSet::IconId() const
{
  if(IsCopied())
  return Copy().IconId();

  if(Used())
  return IDI_RPN_VALUESET_USED;

  return IDI_RPN_VALUESET;
}

unsigned int CRpnValueSet::TypeId() const
{
  return 0;
}

  // Saving and loading
bool CRpnValueSet::Empty() const
{
  return m_pStack == 0;
}

void CRpnValueSet::UpdateProxy()
{
  assert(m_pStack);

  // Secondly we check the valueset
  for(int nValueSet = 0; nValueSet < PointSet().ValueSetSize(); nValueSet++)
  {
  IValueSet& value_set = PointSet().ValueSet(nValueSet);

  rpn::CProxyPersistant<CValueSetProxy> psProxy;
  rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy =
      psProxy.Proxies(*m_pStack);

  bool bCreateProxy = true;
  for(size_t i = 0; i < vcProxy.size(); i++)
  {
      if(&vcProxy[i]->ValueSet() == &value_set)
    bCreateProxy = false;
  }

  if((&value_set != this) && bCreateProxy)
  {
      // The proxy accepts only sources
      if(PointSet().ValueSet(nValueSet).IsCopy())
       new CValueSetProxy(
         (IValueSet&)PointSet().ValueSet(nValueSet).Source(),
          *m_pStack, IsCopy() == true);
      else
    new CValueSetProxy(
          PointSet().ValueSet(nValueSet), *m_pStack, IsCopy() == true);
  }
  }

  // As third we check the formations
  TFormationBaseEntry* pEntry =
  (TFormationBaseEntry*)Model().GraphEntry(MD_BASE_FORMATION);
  assert(pEntry);
  TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
  for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin();
      it != stNode.end(); it++)
  {
  // Do we have already this formation?
  bool bCreateProxy = true;

  rpn::CProxyPersistant<CRpnFormationProxy> psProxy;
  rpn::CProxyPersistant<CRpnFormationProxy>::TProxyVec vcProxy =
      psProxy.Proxies(*m_pStack);
  for(size_t i = 0; i < vcProxy.size(); i++)
  {
      if(&vcProxy[i]->Formation() == *it) bCreateProxy = false;
  }

  if(bCreateProxy) new CRpnFormationProxy(**it, *m_pStack);
  }
}

void CRpnValueSet::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  assert(m_pStack == 0);
  // Load base first
  IValueSet::LoadStream(stream, version, progress);

  // Create a stack
  m_pStack = new rpn::CRpnStack();

  if(CStreamVersion(3, 0, 25) < version)
  {
    QString sBuffer;
    stream >> sBuffer;
    std::stringstream string_stream(sBuffer.toStdString());
    LoadStream(string_stream);
  }
  else
  {
    // Insert X, Y, Z coordinates
    CModelBase& model = dynamic_cast<CModelBase&>(Model());
    new CValueSetCoordinateProxy(*this, IDS_RC_NORTHING, *m_pStack, CValueSetCoordinateProxy::CO_X, "Northing");
    new CValueSetCoordinateProxy(*this, IDS_RC_EASTING, *m_pStack, CValueSetCoordinateProxy::CO_Y, "Easting");
    new CValueSetCoordinateProxy(*this, IDS_RC_DEPTH, *m_pStack, CValueSetCoordinateProxy::CO_Z, "Depth");

    // Insert an IsReservoir
    new CRpnReservoirProxy(model.Mesh(), *m_pStack, "IsReservoir");
  
    // Load the proxies
    int nProxySize1;
    stream >> nProxySize1;
    for(int i = 0; i < nProxySize1; i++)
    {
      // Create a proxy and load it
      int nIndex;
      QString sName;
      stream >> nIndex;
      stream >> sName;
      /*CValueSetProxy* pProxy =*/ new CValueSetProxy(nIndex, sName.toStdString().c_str(), *m_pStack);
    }

    // Load the formation proxy
    if(CStreamVersion(3,0,21) < version)
    {
      int nProxySize2;
      stream >> nProxySize2;
      for(int i = 0; i < nProxySize2; i++)
      {
        QString sProxyId;
        stream >> sProxyId;
        CRpnFormationProxy* pProxy = new CRpnFormationProxy(*m_pStack, sProxyId.toStdString().c_str());
        pProxy->LoadStream(stream, version);
      }
    }


    // Load the formula ....
    QString sFormula;
    stream >> sFormula;
    std::stringstream string_stream(sFormula.toStdString());
    m_pStack->LoadStream(string_stream, version, *m_pStack);
  }
}

void CRpnValueSet::RpnStack(const rpn::CRpnStack& rhs)
{
  delete m_pStack;
  m_pStack = new rpn::CRpnStack(rhs);
}

const rpn::CRpnStack& CRpnValueSet::RpnStack() const
{
  return *m_pStack;
}

// After loading all value set of the point set should the enable proxy
//  called to connect the proxy value sets
void CRpnValueSet::EnableProxy(const CModelBase* enablingModel)
{
  assert(m_pStack);
  assert(enablingModel != 0);

  CModelBase *pModel = &static_cast<CModelBase&>(Model());
  if (m_model == -1)
  {
  // let's try and find out if we have some model that we should use
  CFemAppModel *pReferencedModel = m_pStack->FindModel(*pModel);
  if (pReferencedModel)
      pModel = static_cast<CModelBase *>(pReferencedModel);
  }
  else
  {
  CModelBase *pReferencedModel = pModel->Model(m_model);
  if (pReferencedModel)
      pModel = pReferencedModel;
  }

  if (enablingModel == &Model())
  {
    // Attach pointsets to value proxies
    rpn::CProxyPersistant<CValueSetProxy> psValueSet;
    rpn::CProxyPersistant<CValueSetProxy>::TProxyVec vcProxy = psValueSet.Proxies(*m_pStack);
    for(size_t i = 0; i < vcProxy.size(); i++)
      vcProxy[i]->AttachPointSet(PointSet());

    // Attach model
    m_pStack->AttachToModel(*pModel);
  }
}

void CRpnValueSet::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  // Save base first
  IValueSet::SaveStream(stream, progress);

  // Save the rest in a string stream
  std::stringstream string_stream;
  SaveStream(string_stream);
  stream << QString(string_stream.str().c_str());
}

void CRpnValueSet::SaveStream(std::stringstream& stream)
{
  // Write versioning
  int nMajor = RPN_VERSION_MAJOR;
  int nMinor = RPN_VERSION_MINOR;
  int nRevision = RPN_VERSION_REVISION;
  stream << nMajor << " " << nMinor << " " << nRevision << " ";

  stream << static_cast<CModelBase&>(Model()).Index() << " ";

  // Save coordinate proxies
  rpn::CProxyPersistant<CValueSetCoordinateProxy> psCoordinate;
  psCoordinate.SaveStream(*m_pStack, stream);

  // Save formation proxies
  rpn::CProxyPersistant<CRpnFormationProxy> psFormation;
  psFormation.SaveStream(*m_pStack, stream);

  // Save value proxies
  rpn::CProxyPersistant<CValueSetProxy> psValueSet;
  psValueSet.SaveStream(*m_pStack, stream);

  // Save reservoir proxies
  rpn::CProxyPersistant<CRpnReservoirProxy> psReservoir;
  psReservoir.SaveStream(*m_pStack, stream);

  // Last but not least we're saving the formula
  m_pStack->SaveStream(stream);
}

void CRpnValueSet::InsertFormula(const CRpnValueSet& rhs)
{
  Name(rhs.Name());
  CStreamVersion version(RPN_VERSION_MAJOR, RPN_VERSION_MINOR, RPN_VERSION_REVISION);
  assert(m_pStack == 0);		// Must be empty
  assert(rhs.m_pStack);
  CModelBase& model = (CModelBase&)Model();
  model.NumberOpenGLNodes();	// Number the formations

  std::stringstream istream;
  // Save coordinate proxies
  rpn::CProxyPersistant<CValueSetCoordinateProxy> psCoordinate;
  psCoordinate.SaveStream(*rhs.m_pStack, istream);

  // Save formation proxies
  rpn::CProxyPersistant<CRpnFormationProxy> psFormation;
  psFormation.SaveStream(*rhs.m_pStack, istream);

  // Save reservoir proxies
  rpn::CProxyPersistant<CRpnReservoirProxy> psReservoir;
  psReservoir.SaveStream(*rhs.m_pStack, istream);

  // Last but not least we're saving the formula
  rhs.m_pStack->SaveStream(istream);

  std::stringstream ostream(istream.str());

  // Create a stack and load from
  m_pStack = new rpn::CRpnStack();
  // Load coordinate proxies
  psCoordinate.LoadStream(*m_pStack, ostream, version);

  // Load formation proxies
  psFormation.LoadStream(*m_pStack, ostream, version);

  // Load reservoir proxies
  psReservoir.LoadStream(*m_pStack, ostream, version);

  // Last but not least we're saving the formula
  m_pStack->LoadStream(ostream, version, *m_pStack);
}

void CRpnValueSet::LoadStream(std::stringstream& stream)
{
  // Loading version first
  int nMajor, nMinor, nRevision;
  stream >> nMajor;
  stream >> nMinor;
  stream >> nRevision;
  CStreamVersion version(nMajor, nMinor, nRevision);

  if (version >= CStreamVersion(1, 0, 12))
  {
  stream >> m_model;
  }

  // Load coordinate proxies
  rpn::CProxyPersistant<CValueSetCoordinateProxy> psCoordinate;
  psCoordinate.LoadStream(*m_pStack, stream, version);

  // Load formation proxies
  rpn::CProxyPersistant<CRpnFormationProxy> psFormation;
  psFormation.LoadStream(*m_pStack, stream, version);

  // Load value proxies
  rpn::CProxyPersistant<CValueSetProxy> psValueSet;
  psValueSet.LoadStream(*m_pStack, stream, version);

  // Load reservoir proxies
  rpn::CProxyPersistant<CRpnReservoirProxy> psReservoir;
  psReservoir.LoadStream(*m_pStack, stream, version);

  // Last but not least we're saving the formula
  m_pStack->LoadStream(stream, version, *m_pStack);
}

long CRpnValueSet::SavedItems() const
{
  return PointSet().ValueSetSize();
}

int CRpnValueSet::ValueSize() const
{
  return 0;
}

CRpnValueSet::TValue CRpnValueSet::Min() const
{
  geo::CValue min;
  for(int i = 0; i < PointSet().PointSize(); i++)
  min = min.Min(ValuePoint(PointSet().PointAt(i), geo::IParallelInitializationCallback::Sequential));

  return min;
}

CRpnValueSet::TValue CRpnValueSet::Max() const
{
  geo::CValue max;
  for(int i = 0; i < PointSet().PointSize(); i++)
  max = max.Max(ValuePoint(PointSet().PointAt(i), geo::IParallelInitializationCallback::Sequential));

  return max;
}

CRpnValueSet::TValue CRpnValueSet::ValuePoint(const geo::IPoint& pt, geo::IParallelInitializationCallback * /*cb*/) const
{
  if(m_pStack)
  {
    // Using nearest neighbour in pointset to execute formula
    const CPointSet *pPointSet = dynamic_cast<const CPointSet*>(&PointSet());
    if(pPointSet)
    {
      geo::CPoint point = pPointSet->PointAt(*pPointSet->PointAt(pt).begin());
      if(pPointSet->Dimension() == IPointSet::DIM_2D)
        point.Z(pt.Z());
      return m_pStack->Value(point, CQuantity::SI_UNIT);
    }
    else
      return m_pStack->Value(pt, CQuantity::SI_UNIT);
  }

  return TValue();
}

CRpnValueSet::TValueVec CRpnValueSet::ValueElement(const geo::IElement& element, geo::IParallelInitializationCallback* /*cb*/) const
{
  if(m_pStack)
    return m_pStack->Value(element, CQuantity::SI_UNIT);

  return TValueVec(element.NrOfPoints());
}

void CRpnValueSet::OnNewNeighbour(const CGraphNode &node)
{
  const CValueComponent* pValueType = dynamic_cast<const CValueComponent*>(&node);
  if(pValueType)
  m_stUsers.insert(pValueType);

  IValueSet::OnNewNeighbour(node);
}

void CRpnValueSet::OnNeighbourDeleted(const CGraphNode &node)
{
  const CValueComponent* pValueType = dynamic_cast<const CValueComponent*>(&node);
  if(pValueType)
  {
  //const_cast <CValueComponent*> (pValueType)->clearCache();
  m_stUsers.erase(pValueType);
  }

  IValueSet::OnNeighbourDeleted(node);
}

bool CRpnValueSet::Used() const
{
  return !m_stUsers.empty();
}

bool CRpnValueSet::CanDestroy() const
{
  return !Used();
}

rpn::CRpnStack* CRpnValueSet::getStack() const
{
  return m_pStack;
}

void CRpnValueSet::setStack(rpn::CRpnStack* stack)
{
  m_pStack = stack;
}
