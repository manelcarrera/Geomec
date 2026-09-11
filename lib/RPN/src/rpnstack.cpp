// RpnStack.cpp: implementation of the CRpnStack class.
//
//////////////////////////////////////////////////////////////////////

#include "rpnstack.h"
#include "rpnconstantoperand.h" 
#include "GeomecStringTable.h"

namespace rpn {

//////////////////////////////////////////////////////////////////////
// Implementation of CRpnStack::CCoordinateProxy
//////////////////////////////////////////////////////////////////////
CRpnStack::CCoordinateProxy::CCoordinateProxy(unsigned int uName, CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: CRpnOperand::IValueProxy(stack, sProxyId), m_coor(coor)
{
  m_sName = getStringTableEntry(uName);
}

CRpnStack::CCoordinateProxy::CCoordinateProxy(const QString& sName, CRpnStack& stack, COORDINATE coor, const QString& sProxyId)
: CRpnOperand::IValueProxy(stack, sProxyId), m_coor(coor), m_sName(sName)
{
}

CRpnStack::CCoordinateProxy::CCoordinateProxy(unsigned int uName, CRpnStack& stack, COORDINATE coor)
: CRpnOperand::IValueProxy(stack), m_coor(coor)
{
  m_sName = getStringTableEntry(uName);
}

CRpnStack::CCoordinateProxy::CCoordinateProxy(CRpnStack& stack, const QString& sProxyId)
: CRpnOperand::IValueProxy(stack, sProxyId)
{

}

void CRpnStack::CCoordinateProxy::SaveStream(std::stringstream& stream)
{
  int nCoor = (int)m_coor;
  stream << nCoor << " ";
  SaveString(m_sName, stream);
}

void CRpnStack::CCoordinateProxy::LoadStream(std::stringstream& stream, CStreamVersion& /*version*/)
{
  int nCoor;
  stream >> nCoor;
  m_coor = (COORDINATE)nCoor;
  m_sName = LoadFromString(stream);
}

QString CRpnStack::CCoordinateProxy::TextTag() const
{
  return m_sName;
}

CRpnStack::TValue CRpnStack::CCoordinateProxy::Value(const geo::IPoint &pt, UNIT /*unit*/) const
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

  return TValue();
}

CRpnStack::TValueVec CRpnStack::CCoordinateProxy::Value(const geo::IElement &el, UNIT unit) const
{
  TValueVec vcRet(el.NrOfPoints());

  for(int i = 0; i < el.NrOfPoints(); i++)
  {
    vcRet[i] = Value(el.Point(i), unit);
  }

  return vcRet;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRpnStack::CRpnStack()
: m_pConstant(0)
{

}

CRpnStack::CRpnStack(const CRpnStack& rhs)
: m_pConstant(0)
{
  // We only support an empty or a valid stack
  assert((rhs.StackSize() == 1) || (rhs.StackSize() == 0));

  // Copying the values
  for(TProxyMap::const_iterator it = rhs.m_mpProxy.begin(); it != rhs.m_mpProxy.end(); it++)
  {
    CRpnOperand::IValueProxy *pClone = 0;
    if(it->second.first)
  {
      assert(it->first == it->second.first->ProxyId());
      pClone = it->second.first->Clone(*this);
      assert(it->first == pClone->ProxyId());
  }

    m_mpProxy.insert(TProxyMap::value_type(it->first, TProxy(pClone, TOperandSet())));
  }

  
  assert(m_mpProxy.size() == rhs.m_mpProxy.size());

  // Clone the first item of the stack if we have any
  if(rhs.StackSize() == 1)
    m_vcRpnStack.push_back(rhs.m_vcRpnStack[0]->Clone(*this));

  assert(StackSize() == rhs.StackSize());
}

CRpnStack::~CRpnStack()
{
  // Walk over the value proxies, set their stack pointer to zero and delete them.
  TValueProxyVec vcProxy = ValueProxies();
  for(size_t i = 0; i < vcProxy.size(); i++)
    delete vcProxy[i];
}

int CRpnStack::StackSize() const
{
  return m_vcRpnStack.size();
}

bool CRpnStack::Recursive(TParentSet stParent) const
{
  if(!stParent.insert(this).second)
    return true;
  if(StackSize() > 0)
    return Top().Recursive(stParent);
  return false;
}

const CRpnObject& CRpnStack::Top() const
{
  assert(StackSize() > 0);
  return *m_vcRpnStack[StackSize() - 1];
}

CRpnObject& CRpnStack::Top()
{
  assert(StackSize() > 0);
  return *m_vcRpnStack[StackSize() - 1];
}

const CRpnObject& CRpnStack::StackAt(int nIndex) const
{
  assert(nIndex > -1);
  assert(nIndex < StackSize());
  return *m_vcRpnStack[nIndex];
}

CRpnObject& CRpnStack::Pop()
{
  assert(StackSize() > 0);
  CRpnObject* pObject = &Top();
  m_vcRpnStack.resize(StackSize() - 1);
  m_pConstant = 0;
  return *pObject;
}

void CRpnStack::Push(CRpnObject& object)
{
  m_vcRpnStack.push_back(&object);
  if(&object != m_pConstant)
    m_pConstant = 0;
}

CRpnStack::TValue CRpnStack::Value(const geo::IPoint &pt, UNIT unit) const
{
  assert(StackSize() == 1);
  return Top().Value(pt, unit);
}

CRpnStack::TValueVec CRpnStack::Value(const geo::IElement &el, UNIT unit) const
{
  assert(StackSize() == 1);
  return Top().Value(el, unit);
}

QString CRpnStack::Formula() const
{
  QString sRet;
  for(int i = 0; i < StackSize(); i++)
  {
    sRet += m_vcRpnStack[i]->Formula();

    if ((i + 1) < StackSize()) {
      sRet += " ";
    }
  }

  return sRet;
}


void CRpnStack::Clear(CRpnStack &stack)
{
  assert(&stack == this);
  if(StackSize() > 0)
    Top().Clear(*this);
}

bool CRpnStack::IsConstant() const
{
  return m_pConstant != 0;
}

CRpnConstantOperand& CRpnStack::Constant()
{
  if(m_pConstant)
    return *m_pConstant;

  m_pConstant = new CRpnConstantOperand();
  Push(*m_pConstant);
  return *m_pConstant;
}

void CRpnStack::Enter()
{
  m_pConstant = 0;
}

void CRpnStack::SaveStream(std::stringstream& stream)
{
  // Assure we have a valid stack
  assert(StackSize() == 1);
  Top().SaveStream(stream);
}

void CRpnStack::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
  assert(StackSize() == 0);
  Push(*LoadRpnObject(stream, version, stack));
}

CRpnObject* CRpnStack::Clone(CRpnStack& /*NewStack*/) const
{
    assert(false);	// Not supported for the root
    return 0;
}

CRpnStack::eObjectType CRpnStack::ObjectType() const
{
  assert(false);	// Not supported for the root
  return OT_CONSTANT;
}

CRpnOperand::IValueProxy* CRpnStack::AttachOperand(CRpnOperand& operand, const QString& sProxyId)
{
  TProxyMap::iterator it = m_mpProxy.insert(TProxyMap::value_type(sProxyId, TProxy((CRpnOperand::IValueProxy*)0, TOperandSet()))).first;
  VERIFY(it->second.second.insert(&operand).second);
  return it->second.first;
}

void CRpnStack::DetachOperand(CRpnOperand& operand)
{
  // Search operand and remove it from the set
  for(TProxyMap::iterator it = m_mpProxy.begin(); it != m_mpProxy.end(); it++)
  {
    if(( it->second.second.erase(&operand) == 1 ) && (it->second.first == 0))
    {
      m_mpProxy.erase( it );
      return;
    }
  }
}

void CRpnStack::Swap(int nIndex1, int nIndex2)
{
  assert(nIndex1 >= 0);
  assert(nIndex1 < StackSize());
  assert(nIndex2 >= 0);
  assert(nIndex2 < StackSize());
  CRpnObject *pTemp = m_vcRpnStack[nIndex1];
  m_vcRpnStack[nIndex1] = m_vcRpnStack[nIndex2];
  m_vcRpnStack[nIndex2] = pTemp;
}

QString CRpnStack::AttachValueProxy(CRpnOperand::IValueProxy& proxy, const QString& sProxyId)
{
  TProxyMap::iterator it = m_mpProxy.insert(TProxyMap::value_type(sProxyId, TProxy((CRpnOperand::IValueProxy*)0, TOperandSet()))).first;
  assert(it->second.first == 0);
  it->second.first = &proxy;
  return sProxyId;
}

QString CRpnStack::AttachValueProxy(CRpnOperand::IValueProxy& proxy)
{
  // Create an unique key to register the proxy
  QString sProxyId;
  int iNumber = 0;
  do {
    sProxyId = QString("%1").arg(iNumber);
    iNumber++;
  } while(m_mpProxy.find(sProxyId) != m_mpProxy.end());
  return AttachValueProxy(proxy, sProxyId);
}

void CRpnStack::DetachValueProxy(CRpnOperand::IValueProxy& proxy)
{
  TProxyMap::iterator it = m_mpProxy.find(proxy.ProxyId().toStdString().c_str());
  assert(it != m_mpProxy.end());

  it->second.first = 0;	// Proxy is not registered anymore

  // Clear proxies in operands
  TOperandSet stOperand = it->second.second;
  for(TOperandSet::iterator itt = stOperand.begin(); itt != stOperand.end(); itt++)
    (*itt)->ClearProxy();

  if(stOperand.size() == 0)
    m_mpProxy.erase(it);
}

CRpnOperand::IValueProxy* CRpnStack::ValueProxy(const QString& sProxyId)
{
  TProxyMap::iterator it = m_mpProxy.find(sProxyId);
  if(it != m_mpProxy.end())
    return it->second.first;
  return 0;
}

std::vector<QString> CRpnStack::UnAssignedOperands() const
{
  std::vector<QString> vcRet;
  for(TProxyMap::const_iterator it = m_mpProxy.begin(); it != m_mpProxy.end(); it++)
  {
    if(it->second.first == 0)
      vcRet.push_back(it->first);
  }

  return vcRet;
}

void CRpnStack::SetUnassignedOperand(const QString& sOperand, CRpnOperand::IValueProxy& proxy)
{
  assert(m_mpProxy.find(proxy.ProxyId()) != m_mpProxy.end());
  assert(m_mpProxy.find(sOperand)->second.first == 0);
  TOperandSet stOperand = m_mpProxy.find(sOperand)->second.second;
  // Walk over unassigned operand and replace proxy
  for(TOperandSet::iterator it = stOperand.begin(); it != stOperand.end(); it++)
  {
    (*it)->Proxy(proxy);
  }
  assert(m_mpProxy.find(sOperand) == m_mpProxy.end());
}

bool CRpnStack::Defined() const
{
  for(size_t i = 0; i < m_vcRpnStack.size(); ++i)
  {
  if(!m_vcRpnStack[i]->Defined())
      return false;
  }

  return true;
}

bool CRpnStack::exists() const
{
  for (size_t i = 0; i < m_vcRpnStack.size(); ++i)
  {
  if (!m_vcRpnStack[i]->exists())
  {
      return false;
  }
  }

  return true;
}

void CRpnStack::CollectUsedObjects(TObjectSet& stObjects) const
{
  int i;
  for(i = 0; i < StackSize(); ++i)
  {
  stObjects.insert(&StackAt(i));
  StackAt(i).CollectUsedObjects(stObjects);
  }
}

CRpnStack::TValueProxyVec CRpnStack::ValueProxies() const
{
  TValueProxyVec vcRet;
  for(TProxyMap::const_iterator it = m_mpProxy.begin(); it != m_mpProxy.end(); it++)
  {
    if(it->second.first)
      vcRet.push_back(it->second.first);
  }

  return vcRet;
}

void CRpnStack::RemoveAllValueProxy()
{

}

CFemAppModel *CRpnStack::FindModel(CFemAppModel& model)
{
  for(TProxyMap::const_iterator it = m_mpProxy.begin(); it != m_mpProxy.end(); it++)
  {
  if (it->second.first)
  {
      CFemAppModel *pModel = it->second.first->FindModel(model);
      if (pModel)
    return pModel;
  }
  }

  return 0;
}

void CRpnStack::AttachToModel(CFemAppModel& model)
{
  for(TProxyMap::const_iterator it = m_mpProxy.begin(); it != m_mpProxy.end(); it++)
  {
    if(it->second.first)
      it->second.first->AttachToModel(model);
  }
}

void CRpnStack::Expand(std::list<std::string>& lstMessages)
{
  assert(StackSize() == 1);

  CRpnObject* pExpandedObject = StackAt(0).GetExpandedRpnObject(*this, lstMessages);
  assert(pExpandedObject);
  Pop();
  Push(*pExpandedObject);
}

CRpnObject* CRpnStack::GetExpandedRpnObject(CRpnStack& /*targetstack*/, std::list<std::string>& /*lstMessages*/) const
{
  assert(false);
  return 0;
}

}
