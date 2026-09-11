#if !defined(QUANTITYENTRY_H__INCLUDED_)
#define QUANTITYENTRY_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorGradient.h"
#include "HotSpot.h"
#include "Boundary.h"
#include "Quantity.h" 


class IQuantityEntry
{
public:
  // Access to quantity nodes ...
  virtual void LinkToEntry(CGraphNode &node) = 0;

  // Access to statistics of quantity entries ...
  virtual double GlobalMin(const geo::IBox &box, const int nComponent = 0, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const = 0;
  virtual double GlobalMax(const geo::IBox &box, const int nComponent = 0, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const = 0;

  // Access to hotspot enz. 
  virtual const CHotSpot& HotSpot() const = 0;
  virtual CHotSpot& HotSpot() = 0;
  virtual const CColorGradient& GlobalColorGradient() const = 0;
  virtual CColorGradient& GlobalColorGradient() = 0;
  virtual const CColorGradient& LocalColorGradient() const = 0;
  virtual CColorGradient& LocalColorGradient() = 0;
};

template<class T>
class CQuantityEntry : public CGraphEntryTemp<T> , public IQuantityEntry
{
  CColorGradient *m_pLocal;
  CColorGradient *m_pGlobal;
  CHotSpot *m_pHotSpot;
public:
  CQuantityEntry(const int nEntryId, const CBoundary &boundary); 

  virtual void LinkToEntry(CGraphNode &node);

  double GlobalMin(const geo::IBox &box, const int nComponent = 0, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  double GlobalMax(const geo::IBox &box, const int nComponent = 0, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

  virtual const CHotSpot& HotSpot() const;
  virtual CHotSpot& HotSpot();
  virtual const CColorGradient& GlobalColorGradient() const;
  virtual CColorGradient& GlobalColorGradient();
  virtual const CColorGradient& LocalColorGradient() const;
  virtual CColorGradient& LocalColorGradient();
};

template<class T>
CQuantityEntry<T>::CQuantityEntry(const int nEntryId, const CBoundary &boundary)
: CGraphEntryTemp<T>(nEntryId, 0, 0)
{
//	LinkTo(boundary);
  // Note : Colorscales are destructing when CQuantityEntry destroys.
  m_pLocal = new CColorGradient(_T("Local"), *this);
  m_pGlobal = new CColorGradient(_T("Global"), *this);
  m_pHotSpot = new CHotSpot(_T("Hotspot"), *this);
}

template<class T>
void CQuantityEntry<T>::LinkToEntry(CGraphNode &node) 
{
  LinkTo(node);
}

template<class T>
double CQuantityEntry<T>::GlobalMin(const geo::IBox &box, const int nComponent, const CQuantity::UNIT unit) const
{
  double dRet = 0;
  BOOL bInit = FALSE;
  TNodeSet set = EntryNodes();
  for(TNodeSet::const_iterator it = set.begin(); it != set.end(); it++)
  {
    if(!bInit)
    {
      dRet = (*it)->Min(box, nComponent, unit);
      bInit = TRUE;
    }

    dRet = min(dRet, (*it)->Max(box, nComponent, unit));
  }

  ASSERT(bInit);

  return dRet;
}

template<class T>
double CQuantityEntry<T>::GlobalMax(const geo::IBox &box, const int nComponent, const CQuantity::UNIT unit) const
{
  double dRet = 0;
  BOOL bInit = FALSE;
  TNodeSet set = EntryNodes();
  for(TNodeSet::const_iterator it = set.begin(); it != set.end(); it++)
  {
    if(!bInit)
    {
      dRet = (*it)->Max(box, nComponent, unit);
      bInit = TRUE;
    }

    dRet = max(dRet, (*it)->Max(box, nComponent, unit));
  }

  ASSERT(bInit);

  return dRet;
}

template<class T>
const CHotSpot& CQuantityEntry<T>::HotSpot() const
{
  return *m_pHotSpot;
}

template<class T>
CHotSpot& CQuantityEntry<T>::HotSpot()
{
  return *m_pHotSpot;
}

template<class T>
const CColorGradient& CQuantityEntry<T>::GlobalColorGradient() const
{
  return *m_pGlobal;
}

template<class T>
CColorGradient& CQuantityEntry<T>::GlobalColorGradient()
{
  return *m_pGlobal;
}

template<class T>
const CColorGradient& CQuantityEntry<T>::LocalColorGradient() const
{
  return *m_pLocal;
}

template<class T>
CColorGradient& CQuantityEntry<T>::LocalColorGradient()
{
  return *m_pLocal;
}




#endif // !defined(QUANTITYENTRY_H__INCLUDED_)