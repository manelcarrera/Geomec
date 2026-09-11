#ifndef _HISTORYLIST_H_
#define _HISTORYLIST_H_

#include <list>

// HISTORYITEM must support copy construction

template <class HISTORYITEM>
class CHistoryList
{
public:
  CHistoryList();

  void Append(const HISTORYITEM& item);

  bool CanGoBack() const;
  void GoBack();

  bool CanGoForward() const;
  void GoForward();

  bool IsEmpty() const;
  const HISTORYITEM& Current() const;

  void Clear();

private:
  typedef std::list<HISTORYITEM> THistoryList;
  typedef typename THistoryList::iterator iterator;
  typedef typename THistoryList::const_iterator const_iterator;

private:
  THistoryList m_lstItems;
  iterator m_current;
};

template <class HISTORYITEM>
CHistoryList<HISTORYITEM>::CHistoryList()
: m_current(m_lstItems.end())
{
}

template <class HISTORYITEM>
void CHistoryList<HISTORYITEM>::Append(const HISTORYITEM& item)
{
  if(m_current != m_lstItems.end())
  {
  iterator next = m_current;
  ++next;
  if(next != m_lstItems.end())
  {
      // remove tail
      m_lstItems.erase(next, m_lstItems.end());
  }
  }

  m_lstItems.push_back(item);
  m_current = m_lstItems.end();
  --m_current;
}

template <class HISTORYITEM>
bool CHistoryList<HISTORYITEM>::CanGoBack() const
{
  return m_current != m_lstItems.begin();
}

template <class HISTORYITEM>
void CHistoryList<HISTORYITEM>::GoBack()
{
  assert(CanGoBack());
  --m_current;
}

template <class HISTORYITEM>
bool CHistoryList<HISTORYITEM>::CanGoForward() const
{
  if(m_current == m_lstItems.end())
  return false;

  iterator next = m_current;
  ++next;
  return next != m_lstItems.end();
}

template <class HISTORYITEM>
void CHistoryList<HISTORYITEM>::GoForward()
{
  assert(CanGoForward());
  ++m_current;
}

template <class HISTORYITEM>
bool CHistoryList<HISTORYITEM>::IsEmpty() const
{
  return m_lstItems.empty();
}

template <class HISTORYITEM>
const HISTORYITEM& CHistoryList<HISTORYITEM>::Current() const
{
  assert(!IsEmpty());
  assert(m_current != m_lstItems.end());
  return *m_current;
}

template <class HISTORYITEM>
void CHistoryList<HISTORYITEM>::Clear()
{
  m_lstItems.clear();
  m_current = m_lstItems.end();
}

#endif // _HISTORYLIST_H_
