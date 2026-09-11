// NewWellPointMapper.h: interface for the CNewWellPointValueMap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef NEWWELLPOINTMAPPER_H
#define NEWWELLPOINTMAPPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include<list>
#include <qstring.h>

#include "Value.h"

class CDoubleQuantity;
class CNewWellPoint;

class INewWellPointMapper 
{
public:
  virtual const QString Description() const =0;
};


template<class T>
class CNewWellPointMapper : public INewWellPointMapper
{
public:
  
  CNewWellPointMapper(QString description,const std::list<CNewWellPoint> & List/*, bool bAutoDelete = true*/); 
  ~CNewWellPointMapper();
  virtual const QString Description() const;
  void Description(QString description);
  const T& GetItem(const CNewWellPoint* point) const;
  T& GetItem(const CNewWellPoint* point);
  const std::list<CNewWellPoint> &List() const;
  const CNewWellPoint *At(size_t index) const;
  void InsertItem(CNewWellPoint* point, const T& item );
  void ClearMap();
  //bool AutoDelete() const { return m_bDelete; }
  //void AutoDelete(bool bDelete) { m_bDelete = bDelete; }

  CNewWellPointMapper(const CNewWellPointMapper& rhs)
    :m_Map(rhs.m_Map),
    m_Description(rhs.m_Description),
    m_pList(rhs.m_pList)
  {
    
  }

  CNewWellPointMapper& operator=(const CNewWellPointMapper& rhs)
  {
    m_Map = rhs.m_Map;
    m_pList = rhs.m_pList;
    m_Description = rhs.m_Description;
    return *this;
  }

  virtual bool Empty() const { return m_Map.size() == 0; }
private:
  std::map<const CNewWellPoint*,T> m_Map;
  QString m_Description;
  bool m_bDelete;
protected: 
  
  const std::list<CNewWellPoint> * m_pList;
};

template<class T>
CNewWellPointMapper<T>::CNewWellPointMapper(QString description,const std::list<CNewWellPoint>  & List) :
  m_Description(description),
  m_pList(&List)
{

}

template<class T>
CNewWellPointMapper<T>::~CNewWellPointMapper()
{
  ClearMap();
}

template<class T>
const T& CNewWellPointMapper<T>::GetItem(const CNewWellPoint* point) const
{
  typename std::map <const CNewWellPoint*, T> ::const_iterator it =
    m_Map.find(point);
  assert(it != m_Map.end());
    
  return (*it).second;
}

template<class T>
T& CNewWellPointMapper<T>::GetItem(const CNewWellPoint* point)
{
  typename std::map<const CNewWellPoint*,T>::iterator it = m_Map.find(point);
  assert(it != m_Map.end());
    
  return (*it).second;
}

template<class T>
const std::list<CNewWellPoint>  &CNewWellPointMapper<T>::List() const
{
  return *m_pList;
}

template<class T>
const CNewWellPoint *CNewWellPointMapper<T>::At(size_t index) const
{
  if ( index >= m_pList->size() ) return 0;

  if ( ! m_pList ) return 0;

  std::list<CNewWellPoint>::const_iterator it= m_pList->begin();

  for (size_t ii = 0; ii< m_pList->size(); ++ii, ++it)
    if ( ii == index ) return &(*it);

  return 0;;
}

template<class T>
void CNewWellPointMapper<T>::InsertItem(CNewWellPoint* point,const T& item)
{
  typename std::map<const CNewWellPoint*,T>::iterator it = m_Map.find(point);
  assert(it == m_Map.end());
  m_Map.insert(std::make_pair(point,item));
}

template<class T>
void CNewWellPointMapper<T>::ClearMap()
{
  m_Map.clear();
}

template<class T>
const QString CNewWellPointMapper<T>::Description() const
{
  return m_Description;
}

template<class T>
void CNewWellPointMapper<T>::Description(QString description)
{
  m_Description = description;
}



//concrete wellpoint mappers

class CNewWellPointValueMap: public  CNewWellPointMapper<geo::CValue>
{
public:
  CNewWellPointValueMap();
  CNewWellPointValueMap(const QString& description, const std::list<CNewWellPoint> & List, CDoubleQuantity* pQuantity);
  virtual const CDoubleQuantity& QuantityIndicator();
  virtual ~CNewWellPointValueMap();
  CNewWellPointValueMap& operator=(const CNewWellPointValueMap& rhs);
  
  //interface
  geo::CValue GetItemByTMD(const double & TMD) const;


protected:

  CDoubleQuantity* m_pQuantity;
};

typedef CNewWellPointMapper<QString> CNewWellPointQStringMap;
typedef CNewWellPointMapper<bool>    CNewWellPointBoolMap;

#endif
