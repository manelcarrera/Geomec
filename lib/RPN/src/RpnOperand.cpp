// RpnOperand.cpp: implementation of the CRpnOperand class.
//
//////////////////////////////////////////////////////////////////////

#include "RpnOperand.h"
#include "rpnobject.h"
#include "rpnstack.h"

namespace rpn {

//////////////////////////////////////////////////////////////////////
// Operand implementation ....
//////////////////////////////////////////////////////////////////////

CRpnOperand::IValueProxy::IValueProxy(CRpnStack &stack, const QString &sProxyId) : m_pStack(&stack) {
  if (stack.ValueProxy(sProxyId)) // don't overwrite...
    m_sProxyId = m_pStack->AttachValueProxy(*this);
  else
    m_sProxyId = m_pStack->AttachValueProxy(*this, sProxyId.toStdString().c_str());
}

CRpnOperand::IValueProxy::IValueProxy(CRpnStack &stack) : m_pStack(&stack) {
  m_sProxyId = m_pStack->AttachValueProxy(*this);
}

CRpnOperand::IValueProxy::~IValueProxy() { m_pStack->DetachValueProxy(*this); }

const QString &CRpnOperand::IValueProxy::ProxyId() const { return m_sProxyId; }

CFemAppModel *CRpnOperand::IValueProxy::FindModel(CFemAppModel & /*model*/) { return 0; }

void CRpnOperand::IValueProxy::AttachToModel(CFemAppModel & /*model*/) {}

void CRpnOperand::IValueProxy::SaveString(const QString &string, std::stringstream &stream) {
  std::string sName(string.toStdString());
  stream << '\"' << sName << "\" ";
}

QString CRpnOperand::IValueProxy::LoadFromString(std::stringstream &stream) {
  // Read the value name
  std::string sName;

  // Search for the first "
  while (stream.get() != '\"')
    ;

  // Read string till next "
  char ch;
  do {
    ch = stream.get();
    if (ch != '\"')
      sName.append(1, ch);
  } while (ch != '\"');

  return sName.c_str();
}

bool CRpnOperand::IValueProxy::HasProperties() const { return false; }

bool CRpnOperand::IValueProxy::Properties() {
  assert(false);
  return false;
}

bool CRpnOperand::IValueProxy::operator<(const IValueProxy &rhs) const { return TypeId() < rhs.TypeId(); }

bool CRpnOperand::IValueProxy::CanDelete() const { return false; }

void CRpnOperand::IValueProxy::Delete() { delete this; }

CRpnObject *CRpnOperand::IValueProxy::GetExpandedRpnObject(CRpnStack & /*targetstack*/,
                                                           std::list<std::string> & /*lstMessages*/) const {
  return 0;
}

///
/// Get the stack
///
CRpnStack *CRpnOperand::IValueProxy::Stack() const { return m_pStack; }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRpnOperand::CRpnOperand() : m_pProxy(0) {}

CRpnOperand::CRpnOperand(CRpnStack &stack, IValueProxy &proxy)
    : m_pProxy(&proxy), m_sDescription(proxy.ProxyId().toStdString()) {
  // Add our pointer to the proxy owning vector.
  m_pProxy = stack.AttachOperand(*this, proxy.ProxyId().toStdString().c_str());

  // Add our pointer on the stack
  stack.Push(*this);
}

CRpnOperand::~CRpnOperand() { assert(m_pProxy == 0); }

void CRpnOperand::ClearProxy() { m_pProxy = 0; }

CRpnOperand::TValue CRpnOperand::Value(const geo::IPoint &pt, UNIT unit) const {
  // Try to fetch the proxy pointer and retrieve value if there is any
  // out there.
  if (m_pProxy)
    return m_pProxy->Value(pt, unit);

  return TValue();
}

CRpnOperand::TValueVec CRpnOperand::Value(const geo::IElement &el, UNIT unit) const {
  // Try to fetch the proxy pointer and retrieve value if there is any
  // out there.
  if (m_pProxy)
    return m_pProxy->Value(el, unit);

  return TValueVec();
}

QString CRpnOperand::Formula() const {
  if (m_pProxy)
    return m_pProxy->TextTag();

  QString sRet("<");
  sRet += m_sDescription.c_str();
  sRet += ">";
  return sRet;
}

bool CRpnOperand::Recursive(TParentSet stParent) const {
  if (!stParent.insert(this).second)
    return true;

  if (m_pProxy)
    return m_pProxy->Recursive(stParent);

  return false;
}

bool CRpnOperand::Defined() const {
  if (!m_pProxy)
    return false;

  return m_pProxy->Defined();
}

bool CRpnOperand::exists() const {
  assert(m_pProxy != 0);

  return (m_pProxy != 0);
}

void CRpnOperand::Clear(CRpnStack &stack) {
  // Remove our selves from the stack ...
  assert(&stack.Top() == this);
  stack.Pop();
  stack.DetachOperand(*this);
  m_pProxy = 0;
  delete this;
}

void CRpnOperand::Proxy(IValueProxy &proxy) {
  // Detach operand from stack
  proxy.m_pStack->DetachOperand(*this);
  m_pProxy = proxy.m_pStack->AttachOperand(*this, proxy.ProxyId().toStdString().c_str());
  m_sDescription = proxy.ProxyId().toStdString();
  assert(m_pProxy == &proxy);
}

const CRpnOperand::IValueProxy *CRpnOperand::Proxy() const { return m_pProxy; }

void CRpnOperand::SaveStream(std::stringstream &stream) {
  // Let the base class do it's job
  CRpnObject::SaveStream(stream);

  // Save the string we need for searching in the stack's map
  if (m_pProxy) {
    // Save the proxy text tag in the stream. In the load phase we
    // we can refer to this string.
    m_sDescription = m_pProxy->ProxyId().toStdString();
  }
  stream << '\"' << m_sDescription << "\" ";
}

void CRpnOperand::LoadStream(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack) {
  assert(m_pProxy == 0);
  // Load the base class
  CRpnObject::LoadStream(stream, version, stack);

  // Search for the first "
  while (stream.get() != '\"')
    ;

  // Read string till next "
  char ch;
  do {
    ch = stream.get();
    if (ch != '\"')
      m_sDescription.append(1, ch);
  } while (ch != '\"');

  // Do we have a string? If we have search in the stack for proper proxy.
  if (!m_sDescription.empty())
    m_pProxy = stack.AttachOperand(*this, m_sDescription.c_str());
}

CRpnObject *CRpnOperand::Clone(CRpnStack &NewStack) const {
  CRpnOperand *pClone = new CRpnOperand();

  pClone->m_sDescription = m_sDescription;
  if (!m_sDescription.empty())
    pClone->m_pProxy = NewStack.AttachOperand(*pClone, m_sDescription.c_str());
  return pClone;
}

CRpnOperand::eObjectType CRpnOperand::ObjectType() const { return OT_OPERAND; }

CRpnObject *CRpnOperand::GetExpandedRpnObject(CRpnStack &targetstack, std::list<std::string> &lstMessages) const {
  if (Proxy()) {
    CRpnObject *pObject = Proxy()->GetExpandedRpnObject(targetstack, lstMessages);

    if (pObject) {
      targetstack.DetachOperand(const_cast<CRpnOperand &>(*this));
      return pObject;
    }

    if (Proxy()->Stack() != &targetstack) {
      CRpnOperand *pCopy = new CRpnOperand();
      pCopy->m_pProxy = Proxy()->Clone(targetstack);
      targetstack.DetachValueProxy(*pCopy->m_pProxy);
      QString sProxyId = targetstack.AttachValueProxy(*pCopy->m_pProxy);
      targetstack.AttachOperand(*pCopy, sProxyId);
      pCopy->m_sDescription = sProxyId.toStdString();
      return pCopy;
    }

    return const_cast<CRpnOperand *>(this);
  }

  assert(false);
  return 0;
}

} // namespace rpn
