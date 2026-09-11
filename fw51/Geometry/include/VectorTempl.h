 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// VectorTempl.h: interface for the VectorTempl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORTEMPL_H__64F12B65_E7AE_42A4_A659_40D77808DA4C__INCLUDED_)
#define AFX_VECTORTEMPL_H__64F12B65_E7AE_42A4_A659_40D77808DA4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include <vector>
#include "Point.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {
class GEOMETRY_EXPORT  IArray : public IObject
{
  mutable CPoint m_vtMin;
  mutable CPoint m_vtMax;
protected:
  // Construction
  IArray();
  IArray(const IArray& rhs);

  void CalcMinMax() const;
  mutable bool m_bDirty;
public:


  // Assignment
  virtual const IObject& ObjectBase(size_t nIndex) const = 0;
  virtual IObject& ObjectBase(size_t nIndex) = 0;
  virtual void Remove(int nIndex) = 0;
  virtual size_t Size() const {assert(false); return 0;}

  // Implementation of IObject ....
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Mirror(const geo::IPlane& plane);

  virtual void Transform(const IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;
  virtual void Clear() = 0;

  virtual void ClearAndDelete() { Clear(); }

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleArray(*this); }
};

template<class T>
class CArray : public IArray
{
  std::vector<T> m_vcVec;
public:
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;

  // Construction ...
  CArray(int nSize = 0);
  CArray(const std::vector<T> vcVec);
  CArray(const CArray &rhs);

  virtual void Remove(int nIndex);
  virtual size_t Size() const;
  virtual void Clear();

  // Assignment
  virtual	void PushBack(T &object);
  virtual iterator Insert(iterator it, T& object); 
  virtual const IObject& ObjectBase(size_t nIndex) const;
  virtual IObject& ObjectBase(size_t nIndex);
  const T& Object(size_t nIndex) const;
  T& Object(size_t nIndex);
  virtual void Object(size_t nIndex, const T& object);

  // Iteration ...
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
};

template<class T>
class CPtrArray : public IArray
{
  std::vector<T*> m_vcVec;
public:
  typedef typename std::vector<T*>::iterator iterator;
  typedef typename std::vector<T*>::const_iterator const_iterator;

  // Construction ...
  CPtrArray(int nSize = 0);
  CPtrArray(const std::vector<T*> vcVec);
  CPtrArray(const CPtrArray &rhs);
  
  virtual void Remove(int nIndex);
  virtual size_t Size() const;
  virtual void Clear();

  // Assignment
  virtual void PushBack(T &object);
  virtual iterator Insert( iterator it,T& object); 
  virtual const IObject& ObjectBase(size_t nIndex) const;
  virtual IObject& ObjectBase(size_t nIndex);
  virtual const T& Object(size_t nIndex) const;
  T& Object(size_t nIndex);
  virtual void Object(size_t nIndex, T& object);
  virtual void ClearAndDelete();

  // Iteration ...
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
};

// Normal template
template<class T>
CArray<T>::CArray(int nSize)
: m_vcVec(nSize)
{
}

template<class T>
CArray<T>::CArray(const std::vector<T> vcVec)
: m_vcVec(vcVec)
{
}

template<class T>
CArray<T>::CArray(const CArray<T> &rhs)
: IArray(rhs), m_vcVec(rhs.m_vcVec)
{
}

template<class T>
size_t CArray<T>::Size() const
{
  return m_vcVec.size();
}

template<class T>
void CArray<T>::Clear()
{
  m_bDirty = true;
  m_vcVec.clear();
}

template<class T>
void CArray<T>::Remove(int nIndex)
{       
  m_vcVec.erase(m_vcVec.begin() + nIndex);
  m_bDirty = true;
}

template<class T>
void CArray<T>::PushBack(T &object)
{
  this->m_vcVec.push_back(object);
  this->m_bDirty = true;
}

template<class T>
typename CArray<T>::iterator CArray<T>::Insert(iterator it, T& object) 
{
  this->m_bDirty = true;
  return this->m_vcVec.insert(it, object);
}


template<class T>
const IObject& CArray<T>::ObjectBase(size_t nIndex) const
{
  assert(nIndex >= 0);
  assert(nIndex < this->m_vcVec.size());
  return (IObject&)(this->m_vcVec[nIndex]);
}

template<class T>
IObject& CArray<T>::ObjectBase(size_t nIndex) 
{
  assert(nIndex >= 0);
  assert(nIndex < this->m_vcVec.size());
  return (IObject&)(this->m_vcVec[nIndex]);
}

template<class T>
const T& CArray<T>::Object(size_t nIndex) const
{
  assert(nIndex < this->m_vcVec.size());
  return this->m_vcVec[nIndex];
}

template<class T>
T& CArray<T>::Object(size_t nIndex)
{
  assert(nIndex < this->m_vcVec.size());
  this->m_bDirty = true;
  return this->m_vcVec[nIndex];
}

template<class T>
void CArray<T>::Object(size_t nIndex, const T& object)
{
  assert(nIndex < this->m_vcVec.size());
  this->m_bDirty = true;
  this->m_vcVec[nIndex] = object;
}

template<class T>  
typename CArray<T>::iterator CArray<T>::begin()
{
  return m_vcVec.begin();
}

template<class T>  
typename CArray<T>::const_iterator CArray<T>::end() const
{
  return m_vcVec.end();
}

template<class T>  
typename CArray<T>::const_iterator CArray<T>::begin() const
{
  return m_vcVec.begin();
}

template<class T>  
typename CArray<T>::iterator CArray<T>::end()
{
  return m_vcVec.end();
}


// Ptr template
template<class T>
CPtrArray<T>::CPtrArray(int nSize)
: m_vcVec(nSize)
{
}

template<class T>
CPtrArray<T>::CPtrArray(const std::vector<T*> vcVec)
: m_vcVec(vcVec)
{
}

template<class T>
CPtrArray<T>::CPtrArray(const CPtrArray<T> &rhs)
: IArray(rhs), m_vcVec(rhs.m_vcVec)
{
}

template<class T>
size_t CPtrArray<T>::Size() const
{
  return m_vcVec.size();
}

template<class T>
void CPtrArray<T>::Clear()
{
  m_bDirty = true;
  m_vcVec.clear();
}

template<class T>
void CPtrArray<T>::Remove(int nIndex)
{       
  m_vcVec.erase(m_vcVec.begin() + nIndex);
  m_bDirty = true;
}

template<class T>
void CPtrArray<T>::PushBack(T &object)
{
  this->m_bDirty = true;
  this->m_vcVec.push_back(&object);
}

template<class T>
typename CPtrArray<T>::iterator CPtrArray<T>::Insert(typename CPtrArray<T>::iterator it, T& object) 
{
  this->m_bDirty = true;
  return this->m_vcVec.insert(it, &object);
}


template<class T>
const IObject& CPtrArray<T>::ObjectBase(size_t nIndex) const
{
  assert(nIndex >= 0);
  assert(nIndex < this->m_vcVec.size());
  return (IObject&)(*(this->m_vcVec[nIndex]));
}

template<class T>
IObject& CPtrArray<T>::ObjectBase(size_t nIndex) 
{
  assert(nIndex >= 0);
  assert(nIndex < this->m_vcVec.size());
  return *(this->m_vcVec[nIndex]);
}

template<class T>
const T& CPtrArray<T>::Object(size_t nIndex) const
{
  assert(nIndex < this->m_vcVec.size());
  return *(this->m_vcVec[nIndex]);
}

template<class T>
T& CPtrArray<T>::Object(size_t nIndex)
{
  assert(nIndex < this->m_vcVec.size());
  this->m_bDirty = true;
  return *(this->m_vcVec[nIndex]);
}

template<class T>
void CPtrArray<T>::Object(size_t nIndex, T &object)
{
  assert(nIndex < this->m_vcVec.size());
  this->m_bDirty = true;
  this->m_vcVec[nIndex] = &object;
}

template<class T>
void CPtrArray<T>::ClearAndDelete()
{
  for ( size_t i = 0; i < Size(); ++i ) {
  geo::IObject* pObject = (geo::IObject*)&ObjectBase(i);

  // if IObject is an array do clear and delete recusive
  geo::IArray*  pArray  = dynamic_cast<geo::IArray*> (pObject);
  if ( pArray ) pArray->ClearAndDelete();

  delete pObject;
  }
  Clear();
}

template<class T>  
typename CPtrArray<T>::iterator CPtrArray<T>::begin()
{
  return m_vcVec.begin();
}

template<class T>  
typename CPtrArray<T>::const_iterator CPtrArray<T>::end() const
{
  return m_vcVec.end();
}

template<class T>  
typename CPtrArray<T>::const_iterator CPtrArray<T>::begin() const
{
  return m_vcVec.begin();
}

template<class T>  
typename CPtrArray<T>::iterator CPtrArray<T>::end()
{
  return m_vcVec.end();
}

}

#endif // !defined(AFX_VECTORTEMPL_H__64F12B65_E7AE_42A4_A659_40D77808DA4C__INCLUDED_)
