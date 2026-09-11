#ifndef _OPENGLNODESELECTION_H_
#define _OPENGLNODESELECTION_H_

#include <set>
#include <cassert>

class COpenGLNode;

#include "StorageNode.h"

class COpenGLNodeSelection
{
  class Less
  {
  public:
    bool operator()(const COpenGLNode *lhs, const COpenGLNode *rhs) const;
  };

public:
  typedef std::set<const COpenGLNode*, Less> TSelectionSet;
  typedef TSelectionSet::const_iterator const_iterator;
  typedef TSelectionSet::iterator iterator;

public:
  class CObserver
  {
  public:
    CObserver();
    virtual ~CObserver();
    virtual void OnNodeErased(const COpenGLNode& node);
    virtual void OnNodeInserted(const COpenGLNode& node);
  };

public:
  COpenGLNodeSelection();
  COpenGLNodeSelection(const COpenGLNodeSelection& rhs);

  ~COpenGLNodeSelection(); // deletes all registered observers

  COpenGLNodeSelection& operator=(const COpenGLNodeSelection& rhs);
  bool operator==(const COpenGLNodeSelection& rhs) const;

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  std::pair<iterator, bool> insert(const COpenGLNode& node);
  void erase(iterator pos);
  void erase(const COpenGLNode& node);

  iterator find(const COpenGLNode& node);
  const_iterator find(const COpenGLNode& node) const;

  void clear();
  bool empty() const;

  void RegisterObserver(CObserver& observer); // transfers ownership (will be deleted in destructor)
  void UnregisterObserver(CObserver& observer); // returns ownership (will not be deleted in destructor)

  void LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& version,
    CStorageNode::TPROGRESS& progress, CFemAppModel& model);
  void SaveStream(CStorageNode::TSTREAM& stream,
    CStorageNode::TPROGRESS& progress);

  const TSelectionSet& getSelection() const {return m_stSelection;}

private:
  TSelectionSet m_stSelection;

  typedef std::set<CObserver*> TObserverSet;
  TObserverSet m_stObservers;
};

class IOpenGLNodeSubSelection
{
public:
  virtual bool CanSelect(const COpenGLNode& object) const = 0;
  virtual bool IsSelected(const COpenGLNode& object) const = 0;
  virtual void SetSelected(const COpenGLNode& object, bool bSelected) = 0;
  virtual void clear() = 0;
};

template <class T>
class CTypedSelection : public IOpenGLNodeSubSelection
{
public:
  template <class TT>
  class Less
  {
  public:
    bool operator()(const TT* lhs, const TT* rhs) const
    {
      if(lhs && rhs)
      {
        if(lhs->Less(*rhs))
          return true;
        if(rhs->Less(*lhs))
          return false;
      }

      return lhs < rhs;
    }
  };

  class CConstIterator
  {
  private:
    typedef COpenGLNodeSelection TContainer;

  public:
    CConstIterator();
    CConstIterator(const IOpenGLNodeSubSelection& selection, const TContainer& container, typename TContainer::const_iterator pos);
    CConstIterator(const CConstIterator& rhs);
    CConstIterator& operator=(const CConstIterator& rhs);
    bool operator==(const CConstIterator& rhs) const;
    bool operator!=(const CConstIterator& rhs) const;
    CConstIterator& operator++(); // pre-increment
    CConstIterator& operator++(int); // post-increment
    CConstIterator& operator--(); // pre-decrement
    CConstIterator& operator--(int); // post-decrement
    const T& operator*();
    const T* operator->();

  private:
    const IOpenGLNodeSubSelection* m_pSelection;
    const TContainer* m_pContainer;
    typename TContainer::const_iterator m_pos;
  };

  class CIterator : public CConstIterator
  {
  private:
    typedef COpenGLNodeSelection TContainer;

  public:
    CIterator();
    CIterator(const IOpenGLNodeSubSelection& selection, const TContainer& container, typename TContainer::const_iterator pos);
    CIterator(const CIterator& rhs);
    CIterator& operator=(const CIterator& rhs);
    using CConstIterator::operator==;
    using CConstIterator::operator!=;
    CIterator& operator++(); // pre-increment
    CIterator& operator++(int); // post-increment
    CIterator& operator--(); // pre-decrement
    CIterator& operator--(int); // post-decrement
  };

  typedef CIterator iterator;
  typedef CConstIterator const_iterator;

  template <class TT>
  class CFilter
  {
  public:
    virtual ~CFilter() {};
    virtual bool operator()(const TT& /*object*/) const { return true; }
  };

public:
  CTypedSelection(COpenGLNodeSelection& selection, const CFilter<T>* pFilter = 0);
  virtual ~CTypedSelection();

  bool MultipleValueComponentsAllowed()
  {
      return true;
  }

  bool IsSelected(const T& object) const;
  void SetSelected(const T& object, bool bSelected);

  // called by CSelectionBranchObserver_Delegate
  virtual void BranchToggle(bool /*bBranchToggle*/) {}

  virtual bool CanSelect(const COpenGLNode& object) const;
  virtual bool IsSelected(const COpenGLNode& object) const;
  virtual void SetSelected(const COpenGLNode& object, bool bSelected);

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  std::pair<iterator, bool> insert(const T& object);
  void erase(iterator pos);
  void erase(const T& object);

  iterator find(const T& object);
  const_iterator find(const T& object) const;

  virtual void clear();
  bool empty() const;

  const COpenGLNodeSelection& getSelectionSource() const
    {return m_selectionSource;}

private:
  COpenGLNodeSelection& m_selectionSource;
  const CFilter<T>* m_pFilter;
};


///// implementation of CTypedSelection

template <class T>
CTypedSelection<T>::CTypedSelection(COpenGLNodeSelection& selection, const CFilter<T>* pFilter)
: m_selectionSource(selection),
  m_pFilter(pFilter)
{
  if(!m_pFilter)
    m_pFilter = new CFilter<T>;
}

template <class T>
CTypedSelection<T>::~CTypedSelection()
{
  delete m_pFilter;
}

template <class T>
bool CTypedSelection<T>::IsSelected(const T& object) const
{
  return find(object) != end();
}

template <class T>
void CTypedSelection<T>::SetSelected(const T& object, bool bSelected)
{
  if(bSelected)
    insert(object);
  else
    erase(object);
}

template <class T>
bool CTypedSelection<T>::CanSelect(const COpenGLNode& object) const
{
  return (dynamic_cast<const T*>(&object) != 0 && (*m_pFilter)(static_cast<const T&>(object)));
}

template <class T>
bool CTypedSelection<T>::IsSelected(const COpenGLNode& object) const
{
  return IsSelected(static_cast<const T&>(object));
}

template <class T>
void CTypedSelection<T>::SetSelected(const COpenGLNode& object, bool bSelected)
{
  assert(CanSelect(object));
  SetSelected(static_cast<const T&>(object), bSelected);
}

template <class T>
typename CTypedSelection<T>::const_iterator CTypedSelection<T>::begin() const
{
  for(COpenGLNodeSelection::const_iterator it = m_selectionSource.begin(); it != m_selectionSource.end(); ++it)
  {
    if(CanSelect(**it))
      return CConstIterator(*this, m_selectionSource, it);
  }

  return end();
}

template <class T>
typename CTypedSelection<T>::const_iterator CTypedSelection<T>::end() const
{
  return CConstIterator(*this, m_selectionSource, m_selectionSource.end());
}

template <class T>
typename CTypedSelection<T>::iterator CTypedSelection<T>::begin()
{
  for(COpenGLNodeSelection::const_iterator it = m_selectionSource.begin(); it != m_selectionSource.end(); ++it)
  {
    if(CanSelect(**it))
      return CIterator(*this, m_selectionSource, it);
  }

  return end();
}

template <class T>
typename CTypedSelection<T>::iterator CTypedSelection<T>::end()
{
  return CIterator(*this, m_selectionSource, m_selectionSource.end());
}

template <class T>
std::pair<typename CTypedSelection<T>::iterator, bool> CTypedSelection<T>::insert(const T& object)
{
  // Filter should accept this object too
  assert((*m_pFilter)(object));

  std::pair<COpenGLNodeSelection::iterator, bool> prInsert = m_selectionSource.insert(object);

  return std::make_pair(CIterator(*this, m_selectionSource, prInsert.first), prInsert.second);
}

template <class T>
void CTypedSelection<T>::erase(iterator pos)
{
  const T& object = *pos;
  m_selectionSource.erase(object);
}

template <class T>
void CTypedSelection<T>::erase(const T& object)
{
  m_selectionSource.erase(object);
}

template <class T>
typename CTypedSelection<T>::iterator CTypedSelection<T>::find(const T& object)
{
  return CIterator(m_selectionSource, m_selectionSource.find(&object));
}

template <class T>
typename CTypedSelection<T>::const_iterator CTypedSelection<T>::find(const T& object) const
{
  return CConstIterator(*this, m_selectionSource, m_selectionSource.find(object));
}

template <class T>
void CTypedSelection<T>::clear()
{
  while(!empty())
    erase(begin());
}

template <class T>
bool CTypedSelection<T>::empty() const
{
  return begin() == end();
}


///// CTypedSelection::CConstIterator

template <class T>
CTypedSelection<T>::CConstIterator::CConstIterator()
: m_pSelection(0),
  m_pContainer(0)
{
}

template <class T>
CTypedSelection<T>::CConstIterator::CConstIterator(const IOpenGLNodeSubSelection& selection,
                                                   const TContainer& container,
                                                   typename TContainer::const_iterator pos)
: m_pSelection(&selection),
  m_pContainer(&container),
  m_pos(pos)
{
}

template <class T>
CTypedSelection<T>::CConstIterator::CConstIterator(const CConstIterator& rhs)
: m_pSelection(rhs.m_pSelection),
  m_pContainer(rhs.m_pContainer),
  m_pos(rhs.m_pos)
{
}

template <class T>
typename CTypedSelection<T>::CConstIterator& CTypedSelection<T>::CConstIterator::operator=(const CConstIterator& rhs)
{
  m_pSelection = rhs.m_pSelection;
  m_pContainer = rhs.m_pContainer;
  m_pos        = rhs.m_pos;
  return *this;
}

template <class T>
bool CTypedSelection<T>::CConstIterator::operator==(const CConstIterator& rhs) const
{
  if(!m_pContainer && !rhs.m_pContainer)
    return true;

  if(!m_pSelection && !rhs.m_pSelection)
    return true;

  return (
    m_pSelection == rhs.m_pSelection &&
    m_pContainer == rhs.m_pContainer &&
    m_pos        == rhs.m_pos);
}

template <class T>
bool CTypedSelection<T>::CConstIterator::operator!=(const CConstIterator& rhs) const
{
  return !operator==(rhs);
}

template <class T>
typename CTypedSelection<T>::CConstIterator& CTypedSelection<T>::CConstIterator::operator++() // pre-increment
{
  assert(m_pContainer);
  assert(m_pSelection);
  while(1)
  {
    ++m_pos;
    if(m_pos == m_pContainer->end() || m_pSelection->CanSelect(**m_pos))
      break;
  }

  return *this;
}

template <class T>
typename CTypedSelection<T>::CConstIterator& CTypedSelection<T>::CConstIterator::operator++(int) // post-increment
{
  CConstIterator it(*this);
  ++(*this);
  return it;
}

template <class T>
typename CTypedSelection<T>::CConstIterator& CTypedSelection<T>::CConstIterator::operator--() // pre-decrement
{
  assert(m_pContainer);
  assert(m_pSelection);
  while(1)
  {
    --m_pos;
    if(m_pSelection->CanSelect(*(*m_pos)))
      break;
  }

  return *this;
}

template <class T>
typename CTypedSelection<T>::CConstIterator& CTypedSelection<T>::CConstIterator::operator--(int) // post-decrement
{
  CConstIterator it(*this);
  --(*this);
  return it;
}

template <class T>
const T& CTypedSelection<T>::CConstIterator::operator*()
{
  assert(dynamic_cast<const T*>(*m_pos));
  return static_cast<const T&>(**m_pos);
}

template <class T>
const T* CTypedSelection<T>::CConstIterator::operator->()
{
  return *m_pos;
}


///// CTypedSelection::CIterator

template <class T>
CTypedSelection<T>::CIterator::CIterator()
{
}

template <class T>
CTypedSelection<T>::CIterator::CIterator(const IOpenGLNodeSubSelection& selection,
                                         const TContainer& container,
                                         typename TContainer::const_iterator pos)
: CTypedSelection<T>::CConstIterator(selection, container, pos)
{
}

template <class T>
CTypedSelection<T>::CIterator::CIterator(const CIterator& rhs)
: CTypedSelection<T>::CConstIterator(rhs)
{
}

template <class T>
typename CTypedSelection<T>::CIterator& CTypedSelection<T>::CIterator::operator=(const CIterator& rhs)
{
  CTypedSelection<T>::CConstIterator::operator=(rhs);
  return *this;
}

template <class T>
typename CTypedSelection<T>::CIterator& CTypedSelection<T>::CIterator::operator++() // pre-increment
{
  CTypedSelection<T>::CConstIterator::operator++();
  return *this;
}

template <class T>
typename CTypedSelection<T>::CIterator& CTypedSelection<T>::CIterator::operator++(int) // post-increment
{
  CIterator it(*this);
  ++(*this);
  return it;
}

template <class T>
typename CTypedSelection<T>::CIterator& CTypedSelection<T>::CIterator::operator--() // pre-decrement
{
  CTypedSelection<T>::CConstIterator::operator--();
  return *this;
}

template <class T>
typename CTypedSelection<T>::CIterator& CTypedSelection<T>::CIterator::operator--(int) // post-decrement
{
  CIterator it(*this);
  --(*this);
  return it;
}


#endif // _OPENGLNODESELECTION_H_
