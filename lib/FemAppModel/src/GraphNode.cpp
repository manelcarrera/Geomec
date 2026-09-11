// GeoGraphItem.cpp: implementation of the CGraphNode class.
//
//////////////////////////////////////////////////////////////////////

#include "GraphNode.h"
#include "Delegate.h"
#include "GeomecStringTable.h"
#include <assert.h>
#include <vector>

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

// ##ModelId=3B653D010266
CGraphNode::CGraphNode(const QString &strName)
    : m_pSource(0), m_pCopy(0), m_strName(strName), m_eStatus(NO_ACTION), m_parent(0), m_delegate(0),
      m_modifiedByAttributesDialog(false) {}

// ##ModelId=3B653D010273
CGraphNode::CGraphNode(unsigned int uName)
    : m_pSource(0), m_pCopy(0), m_eStatus(NO_ACTION), m_parent(0), m_delegate(0), m_modifiedByAttributesDialog(false) {
  m_strName = getStringTableEntry(uName);
}

// Copies the name.
CGraphNode::CGraphNode(const CGraphNode &rhs)
    : m_pSource(0), m_pCopy(0), m_strName(rhs.m_strName), m_vcChild(rhs.m_vcChild), m_vcReference(rhs.m_vcReference),
      m_eStatus(rhs.m_eStatus), m_parent(rhs.m_parent), m_delegate(0),
      m_modifiedByAttributesDialog(rhs.m_modifiedByAttributesDialog) {
  assert(!rhs.IsCopied());
  assert(!rhs.IsCopy());
  // Assign the the pointer of the copy to the source ...
  m_pSource = const_cast<CGraphNode *>(&rhs);
  m_pSource->m_pCopy = this;

  AssertValid();
}

// ##ModelId=3B653D0102D0
CGraphNode::~CGraphNode() {
  AssertValid();

  m_eStatus = FORCE_DELETE;

  // Is a copy deleting?
  if (IsCopy()) {
    // Unlink all copied neighbour nodes
    // Because after unlinking the iterator of the neighbourhood invalidates a copy
    // is made.
    while (m_vcChild.size() > 0) {
      assert(m_vcChild[0]);
      assert(!m_vcChild[0]->IsCopy());
      if (m_vcChild[0]->IsCopied())
        delete &m_vcChild[0]->Copy();
      else
        m_vcChild.erase(m_vcChild.begin());
    }

    while (referenceSize() > 0) {
      CGraphNode *pNode = m_vcReference[0];
      assert(pNode);
      assert(!pNode->IsCopy());
      if (pNode->IsCopied())
        UnLink(pNode->Copy());
      else
        m_vcReference.erase(m_vcReference.begin());
    }

    if (m_parent) {
      assert(!m_parent->IsCopy());
      if (m_parent->IsCopied())
        UnLink(m_parent->Copy());
    }

    // Go back to original situation ...
    Source().m_pCopy = 0;

  } else {
    while (childSize() > 0)
      delete m_vcChild[0];
    while (referenceSize() > 0)
      m_vcReference[0]->UnLink(*this);
    if (m_parent)
      m_parent->UnLink(*this);
  }

  if (m_delegate != 0) {
    delete m_delegate;
    m_delegate = 0;
  }
}

void CGraphNode::create(CGraphNode *new_parent) {
  assert(!m_parent);
  reParent(new_parent);
}

void CGraphNode::reParent(CGraphNode *new_parent) {
  if (m_parent)
    UnLink(*m_parent);
  m_parent = new_parent;

  if (m_parent) {
    if (IsCopy()) {
      // Linkage of two copies ....
      assert(new_parent->IsCopy());
      //			assert( !isChild( Source() ) && !isReferenced( new_parent->Source() ) );
      //			assert( !rhs.isChild( Source() ) && !rhs.isReferenced( Source() ) );

      m_parent = &new_parent->Source();
      new_parent->m_vcChild.push_back(&Source());

      OnNewNeighbour(new_parent->Source()); // Inform nodes ...
      new_parent->OnNewNeighbour(Source()); // Inform nodes ...
    } else {
      // Linkage of two sources
      assert(!new_parent->IsCopy());
      //			assert( !isChild( rhs ) && !isReferenced( rhs ) );
      //			assert( !rhs.isChild( *this ) && !rhs.isReferenced( *this ) );

      m_parent = new_parent;
      new_parent->m_vcChild.push_back(this);

      OnNewNeighbour(*new_parent);       // Inform nodes ...
      new_parent->OnNewNeighbour(*this); // Inform nodes ...
    }

    assert(IsLinkedTo(*new_parent));
    assert(new_parent->IsLinkedTo(*this));
  } else {
    m_parent = 0;
  }
}

// ##ModelId=3B653D010297
void CGraphNode::OnNewNeighbour(const CGraphNode &node) {
  assert(this != &node);
  assert(!node.IsCopy());
}

// PRE	: Neighbour in set and no copy ..
// POST	: Neighbour not in set
// ##ModelId=3B653D0102A5
void CGraphNode::OnNeighbourDeleted(const CGraphNode &node) {
  assert(&node != this);
  assert(!node.IsCopy());
}

// PRE	: Can connect item is allows to connect
// POST	: Default we link to the graph node
// ##ModelId=3B653D0102B4
bool CGraphNode::ConnectItem(const CGraphNode &item) {
  if (CanConnectItem(item)) {
    LinkTo(const_cast<CGraphNode &>(item));
    return true;
  }

  return false;
}

// PRE	: Can this GeoGraphItem connect to us
// POST	: No in default no GeoGraphItem can connect to us
// ##ModelId=3B653D0102B7
bool CGraphNode::CanConnectItem(const CGraphNode & /*item*/) const { return false; }

bool CGraphNode::CanDisconnectItem(const CGraphNode & /*item*/) const { return true; }

void CGraphNode::OnDragLeave(const CGraphNode & /*item*/) const {
  // ignore
}

// ##ModelId=3B653D0102A2
void CGraphNode::OnNeighbourModified(const CGraphNode & /*node*/, enum ModifiedHint /*uHint*/) {
  // Default no action
}

void CGraphNode::OnDeselect(CGraphNode * /*NewSeleceted*/) {
  // Default no action
}

class CGraphNodeIterator {
  typedef enum { ST_PARENT, ST_CHILD, ST_REFERENCE } TState;
  CGraphNode &m_node;
  CGraphNode *m_current;
  TState m_state;
  size_t m_idx;

public:
  /*!
    Construction of the iterator
  */
  CGraphNodeIterator(CGraphNode &node) : m_node(node), m_current(0), m_state(ST_PARENT), m_idx(0) {
    if (m_node.parent()) {
      m_current = m_node.parent();
    } else if (m_node.childSize()) {
      m_current = &m_node.childAt(0);
      m_state = ST_CHILD;
    } else {
      m_state = ST_REFERENCE;
      if (m_node.referenceSize())
        m_current = &m_node.referenceAt(0);
    }
  }

  /*!
    Returns the current
  */
  CGraphNode *current() { return m_current; }

  /*!
    Returns the next and zero if the current was the last
  */
  CGraphNode *next() {
    switch (m_state) {
    case ST_PARENT:
      assert(m_idx == 0);
      if (m_node.childSize()) {
        m_current = &m_node.childAt(0);
        m_state = ST_CHILD;
      } else {
        m_state = ST_REFERENCE;
        m_current = 0;
        if (m_node.referenceSize())
          m_current = &m_node.referenceAt(0);
      }
      break;
    case ST_CHILD:
      // Only increase when the child is still the same...
      if (m_idx < m_node.childSize()) {
        if (&m_node.childAt(m_idx) == m_current)
          m_idx++;
      }
      if (m_idx < m_node.childSize()) {
        m_current = &m_node.childAt(m_idx);
      } else {
        m_state = ST_REFERENCE;
        m_idx = 0;
        m_current = 0;
        if (m_node.referenceSize())
          m_current = &m_node.referenceAt(0);
      }
      break;
    case ST_REFERENCE:
      if (m_idx < m_node.referenceSize()) {
        if (m_current == &m_node.referenceAt(m_idx))
          m_idx++;
      }
      if (m_idx < m_node.referenceSize())
        m_current = &m_node.referenceAt(m_idx);
      else
        m_current = 0;
      break;
    default:
      assert(false);
    }
    return m_current;
  }
};

// ##ModelId=3B653D010296
/*!
  Modified start to inform parent, then children and then references.
*/
void CGraphNode::Modified(enum ModifiedHint uHint) {
  // Intialize routine
  CGraphNodeIterator it(*this);

  while (it.current()) {
    // Execute modified
    if (IsCopy()) {
      assert(!it.current()->IsCopy());
      if (it.current()->IsCopied())
        it.current()->Copy().OnNeighbourModified(Source(), uHint);

    } else {
      if (!it.current()->IsCopied())
        it.current()->OnNeighbourModified(*this, uHint);
    }

    // Make the current the next
    it.next();
  }
}

// ##ModelId=3B653D0102A8
bool CGraphNode::Destroy() {
  if (CanDestroy()) {
    delete this;
    return true;
  }

  return false;
}

// ##ModelId=3B653D0102B2
bool CGraphNode::CanDestroy() const { return true; }

// ##ModelId=3B653D010282
void CGraphNode::Name(const QString &strName) {
  if (Name() == strName)
    return;

  m_strName = strName;

  Modified(NAME_CHANGED);
}

// ##ModelId=3B653D010277
const QString &CGraphNode::Name() const { return m_strName; }

/*!
  Makes reference to two graphnodes
*/
void CGraphNode::LinkTo(CGraphNode &rhs) {
  // We can only link node which are disconnected ...
  assert(!IsLinkedTo(rhs));

  if (IsCopy()) {
    // Linkage of two copies ....
    assert(rhs.IsCopy());
    assert(isChild(rhs.Source()) == -1 && isReferenced(rhs.Source()) == -1);
    assert(rhs.isChild(Source()) == -1 && rhs.isReferenced(Source()) == -1);

    m_vcReference.push_back(&rhs.Source());
    rhs.m_vcReference.push_back(&Source());

    OnNewNeighbour(rhs.Source()); // Inform nodes ...
    rhs.OnNewNeighbour(Source()); // Inform nodes ...
  } else {
    // Linkage of two sources
    assert(!rhs.IsCopy());
    assert(isChild(rhs) == -1 && isReferenced(rhs) == -1);
    assert(rhs.isChild(*this) == -1 && rhs.isReferenced(*this) == -1);

    m_vcReference.push_back(&rhs);
    rhs.m_vcReference.push_back(this);

    OnNewNeighbour(rhs);       // Inform nodes ...
    rhs.OnNewNeighbour(*this); // Inform nodes ...
  }

  assert(IsLinkedTo(rhs));
}

void CGraphNode::UnLink(CGraphNode &rhs) {
  // AssertValid is done for both argmuments in "IsLinkedTo"
  // We can only unlink nodes which are connected ...
  // We can only link node which are disconnected ...
  assert(IsLinkedTo(rhs));

  // Child parent relation ship?
  if (IsCopy()) {
    assert(rhs.IsCopy());
    if (m_parent == &rhs.Source()) {
      rhs.m_vcChild.erase(rhs.m_vcChild.begin() + rhs.isChild(Source()));
      m_parent = 0;
    } else if (rhs.m_parent == &Source()) {
      m_vcChild.erase(m_vcChild.begin() + isChild(rhs.Source()));
      rhs.m_parent = 0;
    } else {
      m_vcReference.erase(m_vcReference.begin() + isReferenced(rhs.Source()));
      rhs.m_vcReference.erase(rhs.m_vcReference.begin() + rhs.isReferenced(Source()));
    }
    OnNeighbourDeleted(rhs.Source()); // Inform nodes ...
    rhs.OnNeighbourDeleted(Source()); // Inform nodes ..
  } else {
    assert(!rhs.IsCopy());
    if (m_parent == &rhs) {
      if (rhs.IsCopied())
        rhs.Copy().m_vcChild.erase(rhs.Copy().m_vcChild.begin() + rhs.Copy().isChild(*this));
      else
        rhs.m_vcChild.erase(rhs.m_vcChild.begin() + rhs.isChild(*this));
      m_parent = 0;
    } else if (rhs.m_parent == this) {
      m_vcChild.erase(m_vcChild.begin() + isChild(rhs));
      if (IsCopied()) {
        int idx = Copy().isChild(rhs);
        if (idx >= 0)
          Copy().m_vcChild.erase(Copy().m_vcChild.begin() + idx);
      }
      if (rhs.IsCopied())
        rhs.Copy().m_parent = 0;
      else
        rhs.m_parent = 0;
    } else {
      m_vcReference.erase(m_vcReference.begin() + isReferenced(rhs));
      if (rhs.IsCopied())
        rhs.Copy().m_vcReference.erase(rhs.Copy().m_vcReference.begin() + rhs.Copy().isReferenced(*this));
      else
        rhs.m_vcReference.erase(rhs.m_vcReference.begin() + rhs.isReferenced(*this));
    }
    if (rhs.IsCopied())
      rhs.Copy().OnNeighbourDeleted(*this); // Inform nodes ...
    else
      rhs.OnNeighbourDeleted(*this);
    OnNeighbourDeleted(rhs); // Inform nodes ...
  }
}

int CGraphNode::isReferenced(const CGraphNode &rhs) const {
  if (!IsCopy() && !rhs.IsCopy()) {
    for (int i = 0; i < m_vcReference.size(); i++) {
      if (m_vcReference[i] == &rhs) {
        return i;
      }
    }
  } else if (IsCopy() && !rhs.IsCopy()) {
    for (int i = 0; i < m_vcReference.size(); i++) {
      if (m_vcReference[i] == &rhs) {
        return i;
      }
    }
  } else if (!IsCopy() && rhs.IsCopy()) {
    for (int i = 0; i < m_vcReference.size(); i++) {
      if (m_vcReference[i] == &rhs.Source()) {
        return i;
      }
    }
  } else {
    for (int i = 0; i < m_vcReference.size(); i++) {
      if (m_vcReference[i] == &rhs.Source()) {
        return i;
      }
    }
  }

  return -1;
}

int CGraphNode::isChild(const CGraphNode &rhs) const {
  if (!IsCopy() && !rhs.IsCopy()) {
    for (int i = 0; i < m_vcChild.size(); i++) {
      if (m_vcChild[i] == &rhs) {
        assert(rhs.m_parent == this);
        return i;
      }
    }
    assert(rhs.m_parent != this);
  } else if (IsCopy() && !rhs.IsCopy()) {
    for (int i = 0; i < m_vcChild.size(); i++) {
      if (m_vcChild[i] == &rhs) {
        return i;
      }
    }
  } else if (!IsCopy() && rhs.IsCopy()) {
    for (int i = 0; i < m_vcChild.size(); i++) {
      if (m_vcChild[i] == &rhs.Source()) {
        return i;
      }
    }
  } else {
    for (int i = 0; i < m_vcChild.size(); i++) {
      if (m_vcChild[i] == &rhs.Source()) {
        assert(rhs.m_parent == this);
        return i;
      }
    }
    assert(rhs.m_parent != this);
  }

  return -1;
}

std::size_t CGraphNode::childSize() const { return m_vcChild.size(); }

const CGraphNode &CGraphNode::childAt(size_t nIndex) const {
  assert(nIndex < m_vcChild.size());
  //  if(IsCopy() && m_vcReference[nIndex]->IsCopy()) return m_vcReference[nIndex]->Copy();
  return *m_vcChild[nIndex];
}

CGraphNode &CGraphNode::childAt(size_t nIndex) {
  assert(nIndex < m_vcChild.size());
  //  if(IsCopy() && m_vcReference[nIndex]->IsCopy()) return m_vcReference[nIndex]->Copy();
  return *m_vcChild[nIndex];
}

std::size_t CGraphNode::referenceSize() const { return m_vcReference.size(); }

const CGraphNode &CGraphNode::referenceAt(size_t nIndex) const {
  assert(nIndex < m_vcReference.size());
  //  if(IsCopy() && m_vcReference[nIndex]->IsCopy()) return m_vcReference[nIndex]->Copy();
  return *m_vcReference[nIndex];
}

CGraphNode &CGraphNode::referenceAt(size_t nIndex) {
  assert(nIndex < m_vcReference.size());
  //  if(IsCopy() && m_vcReference[nIndex]->IsCopy()) return m_vcReference[nIndex]->Copy();
  return *m_vcReference[nIndex];
}

bool CGraphNode::IsLinkedTo(const CGraphNode &rhs) const {
  if (!IsCopy() && !rhs.IsCopy()) {
    if (rhs.m_parent == this || m_parent == &rhs)
      return true;
  } else if (IsCopy() && !rhs.IsCopy()) {
    if (rhs.m_parent == &Source() || m_parent == &rhs)
      return true;
  } else if (!IsCopy() && rhs.IsCopy()) {
    if (rhs.m_parent == this || m_parent == &rhs.Source())
      return true;
  } else {
    assert(IsCopy() && rhs.IsCopy());
    if (rhs.m_parent == &Source() || m_parent == &rhs.Source())
      return true;
  }

  if (isReferenced(rhs) > -1) {
    assert(rhs.isReferenced(*this) > -1);
    return true;
  }
  assert(rhs.isReferenced(*this) == -1);

  return false;
}

void CGraphNode::UnLinkAll() {
  while (childSize() > 0)
    m_vcChild[0]->UnLink(*this);
  while (referenceSize() > 0)
    m_vcReference[0]->UnLink(*this);
  if (m_parent)
    m_parent->UnLink(*this);
}

enum CGraphNode::eHandleStatus CGraphNode::HandleStatus() const { return m_eStatus; }

void CGraphNode::HandleStatus(enum CGraphNode::eHandleStatus status) { m_eStatus = status; }

// Default implementation for deleting objects.
unsigned int CGraphNode::IconId() const { return 0; }

unsigned int CGraphNode::TypeId() const { return 0; }

QString CGraphNode::TypeName() const { return QString(); }

bool CGraphNode::Less(const CGraphNode &node) const { return Name().compare(node.Name(), Qt::CaseInsensitive) < 0; }

// Return value :
// true		The graphnode has a copy
// false	The graph node is copy or is not copied
bool CGraphNode::IsCopied() const { return m_pCopy != 0; }

bool CGraphNode::IsCopy() const { return m_pSource != 0; }

const CGraphNode &CGraphNode::Source() const {
  assert(IsCopy());
  assert(!IsCopied());
  return (CGraphNode &)(*m_pSource);
}

const CGraphNode &CGraphNode::Copy() const {
  assert(IsCopied());
  assert(!IsCopy());
  return (CGraphNode &)(*m_pCopy);
}

CGraphNode &CGraphNode::Source() {
  assert(IsCopy());
  assert(!IsCopied());
  return (CGraphNode &)(*m_pSource);
}

CGraphNode &CGraphNode::Copy() {
  assert(IsCopied());
  assert(!IsCopy());
  return (CGraphNode &)(*m_pCopy);
}

bool CGraphNode::operator==(const CGraphNode &rhs) const {
  // Validate input ...
  AssertValid();
  rhs.AssertValid();

  // Extra validation
  assert(IsCopied());
  assert(rhs.IsCopy());
  assert(&Copy() == &rhs); // You can only validate with a copy belonging to the source ...

  // Are the names the same?
  if (Name() != rhs.Name())
    return false;

  // Check connectivity
  return m_vcChild == rhs.m_vcChild && m_vcReference == rhs.m_vcReference;
}

CGraphNode &CGraphNode::operator=(const CGraphNode &rhs) {
  rhs.AssertValid();
  assert(IsCopied());   // Must be the source ...
  assert(rhs.IsCopy()); // Must be a copy ..

  // We can only assign temporary copies ...
  assert(this == rhs.m_pSource);

  // Assign names ...
  m_strName = rhs.m_strName;

  // Assign parent, children and references
  m_parent = rhs.m_parent;
  m_vcChild = rhs.m_vcChild;
  m_vcReference = rhs.m_vcReference;

  return *this;
}

void CGraphNode::AssertValid() const {
#ifdef _DEBUG
  // Check status
  if (IsCopy()) {
    assert(Source().IsCopied());      // The source must have a copy ...
    assert(&Source().Copy() == this); // The copy of the source is equal to this
  }

  if (IsCopied()) {
    assert(Copy().IsCopy());          // The copy must have a source ...
    assert(&Copy().Source() == this); // the source of the copy must be equal to this
  }
/*
  // Check connections ....
  for(const_iterator it = begin(); it != end(); it++)
  {
    CGraphNode *pNode = const_cast<CGraphNode*>(*it);
    CGraphNode *pThis = const_cast<CGraphNode*>(this);
    // The pointer of a copy is never connected in graph ..
    assert(pNode != pThis);
    assert(!pNode->IsCopy());

    if(pThis->IsCopy() && pNode->IsCopied())
    {
      assert(pThis->m_stNeighbours.find(pNode) != pThis->m_stNeighbours.end());
      assert(pNode->Copy().m_stNeighbours.find(&pThis->Source()) != pNode->Copy().m_stNeighbours.end());
    }

    if(!pThis->IsCopy())
    {
      assert(m_stNeighbours.find(pNode) != m_stNeighbours.end());
      assert((pThis->IsCopied()) || (pNode->m_stNeighbours.find(pThis) != pNode->m_stNeighbours.end()));
    }

  }
*/
#endif //_DEBUG
}

// private

/*!
 * \class CDelegateFactory
 * Factory that creates delegates for nodes.
 * The factory is implemented as a singleton. Use getInstance() to get to the
 * factory.
 * The Register method is called automatically via the REGISTER_DELEGATE
 * system.
 * Use the Create method to create a CDelegate for a specific node type.
 */

CGraphNode::CDelegateFactory *CGraphNode::CDelegateFactory::getInstance() {
  static CDelegateFactory fact;

  return &fact;
}

/*!
 * Register a type and creation function pointer for a specific node type.
 * This function is automatically called via the REGISTER_DELEGATE system.
 */

std::string CGraphNode::CDelegateFactory::Register(const std::type_info &tinfo, TFactory createMethod) {
  // if the VERIFY fails there is already a factory method registered for this
  // node type (there is already a delegate type for this node type registered
  // in the map)

  bool succeeded = m_mpFactory.insert(TFactoryMap::value_type(tinfo, createMethod)).second;

  assert(succeeded);

  return tinfo.name();
}

/*!
 * Create a delegate for a node.
 * Provide the node and the delegate is returned.
 */

CDelegate *CGraphNode::CDelegateFactory::Create(CGraphNode *node) {
  TFactoryMap::iterator it = m_mpFactory.find(typeid(*node));

  if (it == m_mpFactory.end()) {
    return 0;
  }

  return it->second(node);
}

// public

std::string CGraphNode::Register(const std::type_info &tinfo, CDelegateFactory::TFactory createMethod) {
  return CGraphNode::CDelegateFactory::getInstance()->Register(tinfo, createMethod);
}

CDelegate *CGraphNode::getDelegate() {
  if (m_delegate == 0) {
    m_delegate = CDelegateFactory::getInstance()->Create(this);
  }

  return m_delegate;
}
