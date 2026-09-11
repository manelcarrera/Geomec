// WellPointValueMap.h: interface for the CWellPointValueMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPOINTVALUEMAP_H__B7629982_612E_48B7_80A7_91DF6C3E5B69__INCLUDED_)
#define AFX_WELLPOINTVALUEMAP_H__B7629982_612E_48B7_80A7_91DF6C3E5B69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WellPointList.h"
#include <QMap>

class CDoubleQuantity;
class CWellPoint;

namespace well {


class IWellPointMapper 
{
public:
  virtual ~IWellPointMapper();
  virtual const QString Description() const =0;
};


template<class T>
class CWellPointMapper : public IWellPointMapper
{
public:
  
  CWellPointMapper(QString description,const CWellPointList & List/*, bool bAutoDelete = true*/); 
  virtual ~CWellPointMapper();
  virtual const QString Description() const;
  void Description(QString description);
  const T& GetItem(const CWellPoint* point) const;
  T& GetItem(const CWellPoint* point);
  const CWellPointList &List() const;
  void InsertItem(CWellPoint* point, const T& item );
  void ClearMap();
  //bool AutoDelete() const { return m_bDelete; }
  //void AutoDelete(bool bDelete) { m_bDelete = bDelete; }

  CWellPointMapper(const CWellPointMapper& rhs)
    :m_Map(rhs.m_Map),
    m_Description(rhs.m_Description),
    m_pList(rhs.m_pList)
  {
    
  }

  CWellPointMapper& operator=(const CWellPointMapper& rhs)
  {
    m_Map = rhs.m_Map;
    m_pList = rhs.m_pList;
    m_Description = rhs.m_Description;
    return *this;
  }

  virtual bool Empty() const { return m_Map.size() == 0; }

private:
  QMap<const CWellPoint*,T> m_Map;
  QString m_Description;
  bool m_bDelete;
protected: 
  
  const CWellPointList* m_pList;
};

template<class T>
CWellPointMapper<T>::CWellPointMapper(QString description,const CWellPointList & List) :
  m_Description(description),
  m_pList(&List)
{

}

template<class T>
CWellPointMapper<T>::~CWellPointMapper()
{
  ClearMap();
}

template<class T>
const T& CWellPointMapper<T>::GetItem(const CWellPoint* point) const
{
  typename QMap <const CWellPoint*, T> ::const_iterator it =
    m_Map.find(point);
  assert(it != m_Map.end());
    
  return *it;
}

template<class T>
T& CWellPointMapper<T>::GetItem(const CWellPoint* point)
{
  typename QMap<const CWellPoint*,T>::iterator it = m_Map.find(point);
  assert(it != m_Map.end());
    
  return *it;
}

template<class T>
const CWellPointList &CWellPointMapper<T>::List() const
{
  return *m_pList;
}

template<class T>
void CWellPointMapper<T>::InsertItem(CWellPoint* point,const T& item)
{
  assert(m_pList->Exist(point));

  typename QMap<const CWellPoint*,T>::iterator it = m_Map.find(point);
  assert(it == m_Map.end());
  m_Map.insert(point,item);
}

template<class T>
void CWellPointMapper<T>::ClearMap()
{
  //if(m_bDelete)
//	{
//		QMap<CWellPoint*,T*>::iterator it;
//		for(it = m_Map.begin(); it != m_Map.end(); ++it)
//		{
//			if(it.data() != 0) // make sure it's not a NULL pointer
//			{
//				delete (it.data());
//			}
//		}
//
//	}



  m_Map.clear();
}

template<class T>
const QString CWellPointMapper<T>::Description() const
{
  return m_Description;
}

template<class T>
void CWellPointMapper<T>::Description(QString description)
{
  m_Description = description;
}



//concrete wellpoint mappers

class CWellPointValueMap: public  CWellPointMapper<geo::CValue>
{
public:
  CWellPointValueMap();
  CWellPointValueMap(const QString& description, const CWellPointList & List, CDoubleQuantity* pQuantity);
  virtual const CDoubleQuantity& QuantityIndicator();
  virtual ~CWellPointValueMap();
  CWellPointValueMap& operator=(const CWellPointValueMap& rhs);
  
  //interface
  geo::CValue GetItemByTMD(const double & TMD) const;


protected:

  CDoubleQuantity* m_pQuantity;
};

typedef CWellPointMapper<QString> CWellPointQStringMap;
typedef CWellPointMapper<bool>    CWellPointBoolMap;

}
#endif // !defined(AFX_WELLPOINTVALUEMAP_H__B7629982_612E_48B7_80A7_91DF6C3E5B69__INCLUDED_)
