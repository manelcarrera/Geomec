#pragma once

#include <iterator>
#include "Inventor\nodes\SoGroup.h"

class SoGroupIterator;
class SoGroup;

inline SoGroupIterator begin(SoGroup * group);
inline SoGroupIterator end(SoGroup * group);
inline int size (const SoGroup & group);
inline int size (const SoGroup * group);


/// STL type iterator for the SoGroup class, to allow STL type programming with SoGroup objects
/// TBD: This class requires more (unit) testing
class SoGroupIterator : public std::iterator<std::random_access_iterator_tag, SoNode *>
{
public:
  friend SoGroupIterator begin(SoGroup & group);
  friend SoGroupIterator end(SoGroup & group);
  friend SoGroupIterator insert (SoGroupIterator & iter, SoNode * node);
  friend SoGroupIterator erase (SoGroupIterator & iter);

  SoGroupIterator() : m_group(0), m_index(-1)
  {
  }

  SoGroupIterator (SoGroup & group, int index) : m_group(&group), m_index(index)
  {
  }

  SoGroupIterator (const SoGroupIterator & other) :
    m_group(other.m_group),
    m_index(other.m_index)
  {
  }

  SoGroupIterator & SoGroupIterator::operator= (const SoGroupIterator & other)
  {
    m_group = other.m_group;
    m_index = other.m_index;
    return * this;
  }
  
  // ideally, should be -> reference
  auto operator*() -> value_type
  {
    if (* this != end (m_group))
      return m_group->getChild(m_index);
    else
      return nullptr;
  }

  // ideally, should be -> pointer
  auto operator->() -> value_type
  {
    return operator*();
  }

  SoGroupIterator & operator++()
  {
    ++m_index;
    return *this;
  }

  SoGroupIterator operator++(int)
  {
    SoGroupIterator result(*this);
    ++(*this);
    return result;
  }

  SoGroupIterator & operator--()
  {
    --m_index;
    assert(m_index >= 0);
    return *this;
  }

  SoGroupIterator operator--(int)
  {
    SoGroupIterator result(*this);
    --(*this);
    return result;
  }

  bool operator==(const SoGroupIterator & that)
  {
    return m_group == that.m_group && (m_index == that.m_index || (m_index >= size(m_group) && that.m_index >= size(m_group)));
  }

  bool operator==(const SoGroupIterator * that)
  {
    assert (that);
    return this->operator==(*that);
  }

  bool operator!=(const SoGroupIterator &that)
  {
    return !(this->operator==(that));
  }

  bool operator!=(const SoGroupIterator * that)
  {
    assert (that);
    return !(this->operator==(that));
  }

  SoGroupIterator operator+ (difference_type n) const
  {
    SoGroupIterator it(* this);
    it += (n);
       return it;    
  }

  SoGroupIterator operator- (difference_type n) const
  {
    return operator+(-n);
  }

  auto operator- (const SoGroupIterator & other) -> difference_type const
  {
    return m_index - other.m_index;
  }

  bool operator<(const SoGroupIterator & other) const
  {
    return m_index < other.m_index;
  }

  SoGroupIterator & operator+= (difference_type n)
  {
    m_index += n;
    m_index = std::min(m_index, size(m_group));
    m_index = std::max(m_index, 0);

    return * this;
  }

  SoGroupIterator & operator-= (difference_type n)
  {
    return operator+=(-n);
  }

  bool operator>(const SoGroupIterator & other) const
  {
    return m_index > other.m_index;
  }

  bool operator<=(const SoGroupIterator & other) const
  {
    return m_index <= other.m_index;
  }

  bool operator>=(const SoGroupIterator & other) const
  {
    return m_index >= other.m_index;
  }

  auto operator[] (difference_type n)->value_type
  {
    return * operator+(n);
  }


private:
  SoGroup * m_group;
  int m_index;
};

inline SoGroupIterator operator+(SoGroupIterator::difference_type n, const SoGroupIterator& other)
{
  return other.operator+(n);
}

inline SoGroupIterator begin(SoGroup & group)
{
  return SoGroupIterator (group, 0);
}

inline SoGroupIterator end(SoGroup & group)
{
  return SoGroupIterator (group, INT_MAX); 
}

inline SoGroupIterator begin(SoGroup * group)
{
  assert (group);
  return begin (* group);
}

inline SoGroupIterator end(SoGroup * group)
{
  assert (group);
  return end (* group);
}

inline bool empty (SoGroup * group)
{
  return begin(group) == end(group);
}

inline SoGroupIterator insert (SoGroupIterator & iter, SoNode * node)
{
  iter.m_group->insertChild(node, iter.m_index);
  return iter;
}

inline int size (const SoGroup & group)
{
  return group.getNumChildren();
}

inline int size (const SoGroup * group)
{
  assert (group);
  return size (* group);
}

inline SoGroupIterator erase (SoGroupIterator & iter)
{
  assert (iter.m_index < size (iter.m_group));
  iter.m_group->removeChild(iter.m_index);
  return iter;
}


